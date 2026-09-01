#include "machine/instruction.h"
#include "machine/machine.h"
#include "machine/machineconfig.h"
#include "machine/memory/backend/memory.h"
#include "machine/memory/memory_utils.h"
#include "mainwindow/mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDockWidget>
#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QSysInfo>
#include <QTemporaryDir>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <iterator>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <vector>

using namespace machine;

namespace {

constexpr uint64_t DEFAULT_WORK_UNITS = 1120;
constexpr unsigned DEFAULT_SAMPLES = 7;
constexpr unsigned DEFAULT_WARMUPS = 2;
constexpr uint32_t MMU_ROOT_PAGE_TABLE = 0x1000;
constexpr uint32_t MMU_LEAF_PAGE_TABLE = 0x2000;
constexpr uint32_t MMU_CODE_PHYSICAL = 0x3000;
constexpr uint32_t MMU_DATA_PHYSICAL = 0x4000;
constexpr uint32_t MMU_VIRTUAL_BASE = 0x40000000;
constexpr uint32_t MMU_DATA_VIRTUAL = MMU_VIRTUAL_BASE + 0x1000;
constexpr uint32_t MMU_PAGE_SIZE = 0x1000;
constexpr uint32_t MMU_DATA_PAGES = 32;
constexpr uint32_t MMU_PTE_FLAGS = 0xcf;

enum ViewFlag : unsigned {
    VIEW_NONE = 0,
    VIEW_CORE = 1U << 0,
    VIEW_REGISTERS = 1U << 1,
    VIEW_PROGRAM = 1U << 2,
    VIEW_MEMORY = 1U << 3,
    VIEW_CACHES = 1U << 4,
    VIEW_PREDICTOR = 1U << 5,
    VIEW_CSR = 1U << 6,
    VIEW_PERIPHERALS = 1U << 7,
    VIEW_TLBS = 1U << 8,
};

struct MachineProfile {
    const char *name;
    ConfigPresets preset;
    bool pipelined;
    bool caches_enabled;
    bool predictor_enabled;
};

constexpr MachineProfile MACHINE_PROFILES[] = {
    { "single", CP_SINGLE, false, false, false },
    { "single-cache", CP_SINGLE_CACHE, false, true, false },
    { "pipeline", CP_PIPE, true, false, false },
    { "pipeline-cache", CP_PIPE, true, true, false },
    { "pipeline-cache-predictor", CP_PIPE, true, true, true },
};

struct ViewProfile {
    const char *name;
    unsigned views;
};

constexpr ViewProfile VIEW_PROFILES[] = {
    { "none", VIEW_NONE },
    { "core", VIEW_CORE },
    { "registers-program", VIEW_REGISTERS | VIEW_PROGRAM },
    { "memory", VIEW_MEMORY },
    { "caches", VIEW_CACHES },
    { "predictor", VIEW_PREDICTOR },
    { "csr", VIEW_CSR },
    { "peripherals", VIEW_PERIPHERALS },
    { "tlbs", VIEW_TLBS },
    { "all",
      VIEW_CORE | VIEW_REGISTERS | VIEW_PROGRAM | VIEW_MEMORY | VIEW_CACHES | VIEW_PREDICTOR
          | VIEW_CSR | VIEW_PERIPHERALS | VIEW_TLBS },
};

struct SpeedProfile {
    const char *name;
    unsigned time_chunk_ms;
};

constexpr SpeedProfile SPEED_PROFILES[] = {
    { "max", 100 },
    { "unlimited", 0 },
};

enum class WorkloadKind {
    HASH,
    MEMORY,
    PERIPHERAL,
    CLASSIFIER,
    MMU,
};

struct WorkloadProfile {
    const char *name;
    WorkloadKind kind;
    uint64_t instructions_per_iteration;
};

constexpr WorkloadProfile WORKLOAD_PROFILES[] = {
    { "hash", WorkloadKind::HASH, 8 },
    { "memory", WorkloadKind::MEMORY, 8 },
    { "peripheral", WorkloadKind::PERIPHERAL, 8 },
    { "classifier", WorkloadKind::CLASSIFIER, 8 },
    { "mmu", WorkloadKind::MMU, 10 },
};

struct Scenario {
    QString name;
    const MachineProfile *machine_profile;
    const WorkloadProfile *workload_profile;
    const ViewProfile *view_profile;
    const SpeedProfile *speed_profile;
    bool cli_baseline;
};

std::vector<Scenario> make_scenarios() {
    std::vector<Scenario> scenarios;
    scenarios.reserve(
        std::size(MACHINE_PROFILES) * std::size(WORKLOAD_PROFILES)
        * (std::size(VIEW_PROFILES) * std::size(SPEED_PROFILES) + 1));
    for (const MachineProfile &machine_profile : MACHINE_PROFILES) {
        for (const WorkloadProfile &workload_profile : WORKLOAD_PROFILES) {
            scenarios.push_back({
                QString("cli-%1-%2").arg(machine_profile.name, workload_profile.name),
                &machine_profile,
                &workload_profile,
                nullptr,
                nullptr,
                true,
            });
            for (const ViewProfile &view_profile : VIEW_PROFILES) {
                for (const SpeedProfile &speed_profile : SPEED_PROFILES) {
                    scenarios.push_back({
                        QString("gui-%1-%2-%3-%4")
                            .arg(
                                machine_profile.name, workload_profile.name, view_profile.name,
                                speed_profile.name),
                        &machine_profile,
                        &workload_profile,
                        &view_profile,
                        &speed_profile,
                        false,
                    });
                }
            }
        }
    }
    return scenarios;
}

struct RunResult {
    uint64_t duration_ns;
    uint64_t cycles;
    uint64_t instructions;
    uint64_t checksum;
    uint64_t itlb_hits;
    uint64_t itlb_misses;
    uint64_t dtlb_hits;
    uint64_t dtlb_misses;
};

MachineConfig
make_machine_config(const MachineProfile &profile, const WorkloadProfile &workload_profile) {
    MachineConfig config;
    config.preset(profile.preset);
    config.set_delay_slot(false);
    config.set_osemu_enable(false);
    config.access_cache_program()->set_enabled(profile.caches_enabled);
    config.access_cache_data()->set_enabled(profile.caches_enabled);
    config.access_cache_level2()->set_enabled(false);
    config.set_bp_enabled(profile.predictor_enabled);
    if (profile.predictor_enabled) {
        config.set_bp_type(PredictorType::SMITH_2_BIT_HYSTERESIS);
        config.set_bp_init_state(PredictorState::WEAKLY_NOT_TAKEN);
    }
    config.set_vm_enabled(workload_profile.kind == WorkloadKind::MMU);
    return config;
}

class GuiWorkload {
public:
    GuiWorkload(QApplication &app, const Scenario &scenario)
        : workload_profile(scenario.workload_profile)
        , app(app) {
        MachineConfig config
            = make_machine_config(*scenario.machine_profile, *scenario.workload_profile);
        if (scenario.cli_baseline) {
            cli_machine = std::make_unique<Machine>(config, false, false);
            machine = cli_machine.get();
            machine->set_speed(0);
        } else {
            if (!settings_dir.isValid()) {
                throw std::runtime_error("Cannot create temporary GUI benchmark settings.");
            }

            auto *settings
                = new QSettings(settings_dir.filePath("settings.ini"), QSettings::IniFormat);
            window = std::make_unique<MainWindow>(settings);
            window->resize(1280, 900);
            for (QDockWidget *dock : window->findChildren<QDockWidget *>()) {
                dock->hide();
            }
            if ((scenario.view_profile->views & VIEW_CORE) == 0) {
                window->show_hide_coreview(false);
            }

            window->create_core(config, false);
            machine = window->current_machine();
            if (machine == nullptr) {
                throw std::runtime_error("GUI benchmark failed to create a machine.");
            }

            show_views(scenario.view_profile->views);
            machine->set_speed(0, scenario.speed_profile->time_chunk_ms);
            window->show();
        }

        if (machine == nullptr) {
            throw std::runtime_error("Benchmark failed to create a machine.");
        }

        load_program();
        app.processEvents(QEventLoop::AllEvents);
    }

    RunResult run(uint64_t work_units) {
        machine->restart();
        machine->get_tlb_program_rw()->reset();
        machine->get_tlb_data_rw()->reset();
        reset_workload_memory();
        machine->set_instruction_limit(0);
        if (workload_profile->kind == WorkloadKind::MMU) { prepare_mmu_execution(); }
        app.processEvents(QEventLoop::AllEvents);

        const uint64_t initial_cycles = machine->core()->get_cycle_count();
        const uint64_t initial_instructions = machine->core()->get_instruction_count();
        machine->set_instruction_limit(initial_instructions + work_units);
        const auto start = std::chrono::steady_clock::now();
        machine->play();
        while (!machine->exited()) {
            app.processEvents(QEventLoop::AllEvents);
        }
        app.processEvents(QEventLoop::AllEvents);
        const auto end = std::chrono::steady_clock::now();

        validate(work_units, initial_instructions);
        return {
            static_cast<uint64_t>(
                std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()),
            machine->core()->get_cycle_count() - initial_cycles,
            machine->core()->get_instruction_count() - initial_instructions,
            checksum(),
            machine->get_tlb_program()->get_hit_count(),
            machine->get_tlb_program()->get_miss_count(),
            machine->get_tlb_data()->get_hit_count(),
            machine->get_tlb_data()->get_miss_count(),
        };
    }

private:
    void show_views(unsigned views) {
        if ((views & VIEW_REGISTERS) != 0) { window->show_registers(); }
        if ((views & VIEW_PROGRAM) != 0) { window->show_program(); }
        if ((views & VIEW_MEMORY) != 0) { window->show_memory(); }
        if ((views & VIEW_CACHES) != 0) {
            window->show_cache_program();
            window->show_cache_data();
        }
        if ((views & VIEW_PREDICTOR) != 0) {
            window->show_bp_btb();
            window->show_bp_bht();
            window->show_bp_info();
        }
        if ((views & VIEW_CSR) != 0) { window->show_csrdock(); }
        if ((views & VIEW_PERIPHERALS) != 0) { window->show_peripherals(); }
        if ((views & VIEW_TLBS) != 0) {
            window->show_tlb_program();
            window->show_tlb_data();
        }
    }

    void prepare_mmu_execution() {
        for (unsigned step = 0;
             step < 32 && machine->registers()->read_pc().get_raw() != MMU_VIRTUAL_BASE;
             ++step) {
            machine->step();
            if (machine->exited()) {
                throw std::runtime_error("MMU setup trapped before entering translated code.");
            }
        }

        if (machine->registers()->read_pc().get_raw() != MMU_VIRTUAL_BASE
            || machine->core()->get_current_privilege() != CSR::PrivilegeLevel::SUPERVISOR) {
            throw std::runtime_error("MMU setup did not enter Supervisor mode.");
        }
    }

    void reset_workload_memory() {
        if (workload_profile->kind == WorkloadKind::MEMORY) {
            for (uint32_t slot = 0; slot < 16; ++slot) {
                memory_write_u32(machine->memory_rw(), 0x100 + slot * 4, 0);
            }
        } else if (workload_profile->kind == WorkloadKind::MMU) {
            for (uint32_t page = 0; page < MMU_DATA_PAGES; ++page) {
                memory_write_u32(
                    machine->memory_rw(), MMU_DATA_PHYSICAL + page * MMU_PAGE_SIZE, 0);
            }
        }
    }

    void load_program() {
        const uint64_t loop_target = machine->registers()->read_pc().get_raw();
        switch (workload_profile->kind) {
        case WorkloadKind::HASH:
            write_program({
                "addi x5, x5, 1",
                "xor x6, x6, x5",
                "slli x7, x6, 5",
                "srli x8, x6, 2",
                "xor x6, x7, x8",
                "add x6, x6, x5",
                "xor x9, x9, x6",
                QString("jal x0, %1").arg(loop_target),
            });
            break;
        case WorkloadKind::MEMORY:
            write_program({
                "addi x5, x5, 1",
                "slli x10, x5, 2",
                "andi x10, x10, 60",
                "lw x6, 256(x10)",
                "add x6, x6, x5",
                "xor x7, x7, x6",
                "sw x6, 256(x10)",
                QString("jal x0, %1").arg(loop_target),
            });
            break;
        case WorkloadKind::PERIPHERAL:
            write_program({
                "addi x5, x5, 1",
                "slli x6, x5, 1",
                "xor x6, x6, x5",
                "lui x9, 0xffffc",
                "sw x6, 260(x9)",
                "srli x7, x6, 8",
                "sw x7, 272(x9)",
                QString("jal x0, %1").arg(loop_target),
            });
            break;
        case WorkloadKind::CLASSIFIER:
            write_program({
                "addi x5, x5, 1",
                "andi x6, x5, 3",
                QString("beq x6, x0, %1").arg(loop_target + 20),
                "addi x7, x7, 1",
                QString("jal x0, %1").arg(loop_target + 28),
                "addi x8, x8, 1",
                QString("jal x0, %1").arg(loop_target + 28),
                "add x9, x7, x8",
                "xor x10, x10, x6",
                QString("jal x0, %1").arg(loop_target),
            });
            break;
        case WorkloadKind::MMU:
            load_mmu_program();
            break;
        }
    }

    void write_program(const QStringList &instructions) {
        write_program_at(machine->registers()->read_pc(), instructions);
    }

    void write_program_at(Address pc, const QStringList &instructions) {
        for (const QString &instruction : instructions) {
            uint32_t code[2] = {};
            size_t size;
            try {
                size = Instruction::code_from_string(code, sizeof(code), instruction, pc);
            } catch (const Instruction::ParseError &error) {
                throw std::runtime_error(
                    QString("Failed to encode workload instruction '%1': %2")
                        .arg(instruction, error.message)
                        .toStdString());
            }
            if (size != sizeof(uint32_t)) {
                throw std::runtime_error(
                    QString("Failed to encode workload instruction '%1'.")
                        .arg(instruction)
                        .toStdString());
            }
            memory_write_u32(machine->memory_rw(), pc.get_raw(), code[0]);
            pc += sizeof(uint32_t);
        }
    }

    void load_mmu_program() {
        const uint32_t root_index = MMU_VIRTUAL_BASE >> 22U;
        const uint32_t root_pte = ((MMU_LEAF_PAGE_TABLE >> 12U) << 10U) | 1U;
        memory_write_u32(
            machine->memory_rw(), MMU_ROOT_PAGE_TABLE + root_index * 4U, root_pte);

        const uint32_t code_pte
            = ((MMU_CODE_PHYSICAL >> 12U) << 10U) | MMU_PTE_FLAGS;
        memory_write_u32(machine->memory_rw(), MMU_LEAF_PAGE_TABLE, code_pte);
        for (uint32_t page = 0; page < MMU_DATA_PAGES; ++page) {
            const uint32_t physical = MMU_DATA_PHYSICAL + page * MMU_PAGE_SIZE;
            const uint32_t pte = ((physical >> 12U) << 10U) | MMU_PTE_FLAGS;
            memory_write_u32(
                machine->memory_rw(), MMU_LEAF_PAGE_TABLE + (page + 1U) * 4U, pte);
        }

        write_program({
            "lui x5, 0x80000",
            "addi x5, x5, 1",
            "csrrw x0, satp, x5",
            "lui x5, 0x40000",
            "csrrw x0, mepc, x5",
            "lui x5, 1",
            "csrrc x0, mstatus, x5",
            "addi x5, x5, -2048",
            "csrrs x0, mstatus, x5",
            "mret",
        });
        write_program_at(
            Address(MMU_CODE_PHYSICAL),
            {
                "addi x10, x10, 1",
                "andi x11, x10, 31",
                "slli x11, x11, 12",
                "lui x12, 0x40001",
                "add x12, x12, x11",
                "lw x13, 0(x12)",
                "add x13, x13, x10",
                "sw x13, 0(x12)",
                "xor x14, x14, x13",
                QString("jal x0, %1").arg(MMU_CODE_PHYSICAL),
            });
    }

    void validate(uint64_t work_units, uint64_t initial_instructions) const {
        if (machine->stop_reason() != Machine::SR_INSTRUCTION_LIMIT
            || machine->core()->get_instruction_count() != initial_instructions + work_units) {
            throw std::runtime_error("Benchmark workload did not reach its instruction limit.");
        }

        const uint32_t loops
            = static_cast<uint32_t>(work_units / workload_profile->instructions_per_iteration);
        const Registers *registers = machine->registers();
        bool valid = false;
        switch (workload_profile->kind) {
        case WorkloadKind::HASH: {
            uint32_t state = 0;
            uint32_t shifted_left = 0;
            uint32_t shifted_right = 0;
            uint32_t digest = 0;
            for (uint32_t iteration = 1; iteration <= loops; ++iteration) {
                state ^= iteration;
                shifted_left = state << 5U;
                shifted_right = state >> 2U;
                state = (shifted_left ^ shifted_right) + iteration;
                digest ^= state;
            }
            valid = registers->read_gp(5).as_u32() == loops
                    && registers->read_gp(6).as_u32() == state
                    && registers->read_gp(7).as_u32() == shifted_left
                    && registers->read_gp(8).as_u32() == shifted_right
                    && registers->read_gp(9).as_u32() == digest;
            break;
        }
        case WorkloadKind::MEMORY: {
            const uint32_t expected_offset = (loops << 2U) & 60U;
            std::array<uint32_t, 16> expected_memory {};
            uint32_t last_value = 0;
            uint32_t digest = 0;
            for (uint32_t iteration = 1; iteration <= loops; ++iteration) {
                const uint32_t slot = iteration & 15U;
                last_value = expected_memory[slot] + iteration;
                expected_memory[slot] = last_value;
                digest ^= last_value;
            }
            valid = registers->read_gp(5).as_u32() == loops
                    && registers->read_gp(6).as_u32() == last_value
                    && registers->read_gp(7).as_u32() == digest
                    && registers->read_gp(10).as_u32() == expected_offset;
            for (uint32_t slot = 0; slot < 16 && valid; ++slot) {
                valid = memory_read_u32(machine->memory(), 0x100 + slot * 4)
                        == expected_memory[slot];
            }
            break;
        }
        case WorkloadKind::PERIPHERAL: {
            const uint32_t pattern = (loops << 1U) ^ loops;
            const uint32_t color = pattern >> 8U;
            valid = registers->read_gp(5).as_u32() == loops
                    && registers->read_gp(6).as_u32() == pattern
                    && registers->read_gp(7).as_u32() == color
                    && registers->read_gp(9).as_u32() == UINT32_C(0xffffc000)
                    && memory_read_u32(machine->peripheral_spi_led(), 4) == pattern
                    && memory_read_u32(machine->peripheral_spi_led(), 16) == color;
            break;
        }
        case WorkloadKind::CLASSIFIER: {
            uint32_t digest = 0;
            for (uint32_t iteration = 1; iteration <= loops; ++iteration) {
                digest ^= iteration & 3U;
            }
            valid = registers->read_gp(5).as_u32() == loops
                    && registers->read_gp(6).as_u32() == (loops & 3U)
                    && registers->read_gp(7).as_u32() == loops - loops / 4U
                    && registers->read_gp(8).as_u32() == loops / 4U
                    && registers->read_gp(9).as_u32() == loops
                    && registers->read_gp(10).as_u32() == digest;
            break;
        }
        case WorkloadKind::MMU: {
            const uint32_t expected_page = loops & (MMU_DATA_PAGES - 1U);
            std::array<uint32_t, MMU_DATA_PAGES> expected_memory {};
            uint32_t last_value = 0;
            uint32_t digest = 0;
            for (uint32_t iteration = 1; iteration <= loops; ++iteration) {
                const uint32_t page = iteration & (MMU_DATA_PAGES - 1U);
                last_value = expected_memory[page] + iteration;
                expected_memory[page] = last_value;
                digest ^= last_value;
            }
            valid = registers->read_gp(10).as_u32() == loops
                    && registers->read_gp(11).as_u32() == expected_page * MMU_PAGE_SIZE
                    && registers->read_gp(12).as_u32()
                           == MMU_DATA_VIRTUAL + expected_page * MMU_PAGE_SIZE
                    && registers->read_gp(13).as_u32() == last_value
                    && registers->read_gp(14).as_u32() == digest;
            for (uint32_t page = 0; page < MMU_DATA_PAGES && valid; ++page) {
                valid = memory_read_u32(
                            machine->memory(), MMU_DATA_PHYSICAL + page * MMU_PAGE_SIZE)
                        == expected_memory[page];
            }
            valid = valid && machine->get_tlb_program()->get_miss_count() > 0
                    && machine->get_tlb_data()->get_miss_count() > 0;
            break;
        }
        }
        if (!valid) {
            throw std::runtime_error("Benchmark workload final machine state is incorrect.");
        }
    }

    uint64_t checksum() const {
        uint64_t hash = UINT64_C(1469598103934665603);
        auto add_value = [&hash](uint32_t value) {
            hash ^= value;
            hash *= UINT64_C(1099511628211);
        };
        switch (workload_profile->kind) {
        case WorkloadKind::HASH:
            for (unsigned reg : { 5U, 6U, 7U, 8U, 9U }) {
                add_value(machine->registers()->read_gp(reg).as_u32());
            }
            break;
        case WorkloadKind::MEMORY:
            for (unsigned reg : { 5U, 6U, 7U, 10U }) {
                add_value(machine->registers()->read_gp(reg).as_u32());
            }
            for (uint32_t slot = 0; slot < 16; ++slot) {
                add_value(memory_read_u32(machine->memory(), 0x100 + slot * 4));
            }
            break;
        case WorkloadKind::PERIPHERAL:
            for (unsigned reg : { 5U, 6U, 7U, 9U }) {
                add_value(machine->registers()->read_gp(reg).as_u32());
            }
            add_value(memory_read_u32(machine->peripheral_spi_led(), 4));
            add_value(memory_read_u32(machine->peripheral_spi_led(), 16));
            break;
        case WorkloadKind::CLASSIFIER:
            for (unsigned reg : { 5U, 6U, 7U, 8U, 9U, 10U }) {
                add_value(machine->registers()->read_gp(reg).as_u32());
            }
            break;
        case WorkloadKind::MMU:
            for (unsigned reg : { 10U, 11U, 12U, 13U, 14U }) {
                add_value(machine->registers()->read_gp(reg).as_u32());
            }
            for (uint32_t page = 0; page < MMU_DATA_PAGES; ++page) {
                add_value(memory_read_u32(
                    machine->memory(), MMU_DATA_PHYSICAL + page * MMU_PAGE_SIZE));
            }
            break;
        }
        return hash;
    }

    const WorkloadProfile *workload_profile;
    QApplication &app;
    QTemporaryDir settings_dir;
    std::unique_ptr<MainWindow> window;
    std::unique_ptr<Machine> cli_machine;
    Machine *machine = nullptr;
};

const Scenario *find_scenario(const std::vector<Scenario> &scenarios, const QString &name) {
    for (const Scenario &scenario : scenarios) {
        if (name == scenario.name) { return &scenario; }
    }
    return nullptr;
}

uint64_t parse_unsigned(
    const QCommandLineParser &parser,
    const QString &option,
    uint64_t default_value) {
    if (!parser.isSet(option)) { return default_value; }
    bool ok;
    const uint64_t value = parser.value(option).toULongLong(&ok);
    if (!ok) {
        throw std::runtime_error(
            QString("Option --%1 must be an unsigned integer.").arg(option).toStdString());
    }
    return value;
}

QJsonObject benchmark_scenario(
    QApplication &app,
    const Scenario &scenario,
    uint64_t work_units,
    unsigned warmups,
    unsigned samples) {
    GuiWorkload workload(app, scenario);
    for (unsigned i = 0; i < warmups; ++i) {
        workload.run(work_units);
    }

    std::vector<uint64_t> durations;
    durations.reserve(samples);
    uint64_t expected_cycles = 0;
    uint64_t expected_checksum = 0;
    uint64_t expected_itlb_hits = 0;
    uint64_t expected_itlb_misses = 0;
    uint64_t expected_dtlb_hits = 0;
    uint64_t expected_dtlb_misses = 0;
    for (unsigned i = 0; i < samples; ++i) {
        const RunResult result = workload.run(work_units);
        if (i == 0) {
            expected_cycles = result.cycles;
            expected_checksum = result.checksum;
            expected_itlb_hits = result.itlb_hits;
            expected_itlb_misses = result.itlb_misses;
            expected_dtlb_hits = result.dtlb_hits;
            expected_dtlb_misses = result.dtlb_misses;
        } else if (
            result.cycles != expected_cycles || result.instructions != work_units
            || result.checksum != expected_checksum || result.itlb_hits != expected_itlb_hits
            || result.itlb_misses != expected_itlb_misses
            || result.dtlb_hits != expected_dtlb_hits
            || result.dtlb_misses != expected_dtlb_misses) {
            throw std::runtime_error("GUI benchmark samples produced inconsistent work results.");
        }
        durations.push_back(result.duration_ns);
    }

    std::vector<uint64_t> sorted = durations;
    std::sort(sorted.begin(), sorted.end());
    const double median = sorted.size() % 2 == 0
                              ? (static_cast<double>(sorted[sorted.size() / 2 - 1])
                                 + static_cast<double>(sorted[sorted.size() / 2]))
                                    / 2.0
                              : static_cast<double>(sorted[sorted.size() / 2]);
    const double mean = std::accumulate(durations.begin(), durations.end(), 0.0)
                        / static_cast<double>(durations.size());

    QJsonArray sample_values;
    for (uint64_t duration : durations) {
        sample_values.append(static_cast<double>(duration));
    }

    return {
        { "name", scenario.name },
        { "unit", "ns" },
        { "workload", QString("%1-loop-v1").arg(scenario.workload_profile->name) },
        { "primary_metric", "cycles_per_second" },
        { "higher_is_better", true },
        { "config",
          QJsonObject {
              { "frontend", scenario.cli_baseline ? "cli" : "gui" },
              { "machine_profile", scenario.machine_profile->name },
              { "pipelined", scenario.machine_profile->pipelined },
              { "branch_predictor",
                scenario.machine_profile->predictor_enabled ? "smith-2-bit-hysteresis"
                                                            : "disabled" },
              { "hazard_unit", scenario.machine_profile->pipelined ? "forward" : "none" },
              { "program_cache", scenario.machine_profile->caches_enabled },
              { "data_cache", scenario.machine_profile->caches_enabled },
              { "virtual_memory", scenario.workload_profile->kind == WorkloadKind::MMU },
              { "view_profile",
                scenario.cli_baseline ? "none" : scenario.view_profile->name },
              { "gui_speed",
                scenario.cli_baseline ? "unlimited" : scenario.speed_profile->name },
              { "time_chunk_ms",
                static_cast<double>(
                    scenario.cli_baseline ? 0 : scenario.speed_profile->time_chunk_ms) },
              { "platform", QGuiApplication::platformName() },
          } },
        { "work",
          QJsonObject {
              { "instructions", static_cast<double>(work_units) },
              { "cycles", static_cast<double>(expected_cycles) },
              { "checksum", QString("0x%1").arg(expected_checksum, 16, 16, QLatin1Char('0')) },
              { "itlb_hits", static_cast<double>(expected_itlb_hits) },
              { "itlb_misses", static_cast<double>(expected_itlb_misses) },
              { "dtlb_hits", static_cast<double>(expected_dtlb_hits) },
              { "dtlb_misses", static_cast<double>(expected_dtlb_misses) },
          } },
        { "samples_ns", sample_values },
        { "summary",
          QJsonObject {
              { "min_ns", static_cast<double>(sorted.front()) },
              { "median_ns", median },
              { "mean_ns", mean },
              { "max_ns", static_cast<double>(sorted.back()) },
              { "cycles_per_second",
                static_cast<double>(expected_cycles) * 1e9 / median },
              { "instructions_per_second",
                static_cast<double>(work_units) * 1e9 / median },
          } },
    };
}

void write_output(const QJsonDocument &document, const QString &path) {
    const QByteArray bytes = document.toJson(QJsonDocument::Indented);
    if (path.isEmpty()) {
        if (fwrite(bytes.constData(), 1, bytes.size(), stdout) != size_t(bytes.size())) {
            throw std::runtime_error("Failed to write benchmark results to stdout.");
        }
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        throw std::runtime_error(
            QString("Cannot open benchmark output %1: %2")
                .arg(path, file.errorString())
                .toStdString());
    }
    if (file.write(bytes) != bytes.size()) {
        throw std::runtime_error(
            QString("Cannot write benchmark output %1: %2")
                .arg(path, file.errorString())
                .toStdString());
    }
}

} // namespace

int main(int argc, char **argv) {
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("qtrvsim_gui_benchmark");

    QCommandLineParser parser;
    parser.setApplicationDescription("Deterministic QtRvSim GUI benchmark runner.");
    parser.addHelpOption();
    parser.addOption({ "list", "List available benchmark scenarios." });
    parser.addOption({ "scenario", "Run a benchmark scenario. May be repeated.", "NAME" });
    parser.addOption(
        { "samples", "Number of measured samples per scenario.", "COUNT",
          QString::number(DEFAULT_SAMPLES) });
    parser.addOption(
        { "warmups", "Number of unmeasured warmup samples per scenario.", "COUNT",
          QString::number(DEFAULT_WARMUPS) });
    parser.addOption(
        { "work-units", "Retired instructions per sample.", "COUNT",
          QString::number(DEFAULT_WORK_UNITS) });
    parser.addOption({ "output", "Write JSON results to a file instead of stdout.", "FNAME" });
    parser.process(app);
    const std::vector<Scenario> scenarios = make_scenarios();

    if (parser.isSet("list")) {
        for (const Scenario &scenario : scenarios) {
            printf("%s\n", scenario.name.toUtf8().constData());
        }
        return EXIT_SUCCESS;
    }

    try {
        const uint64_t work_units
            = parse_unsigned(parser, "work-units", DEFAULT_WORK_UNITS);
        const uint64_t samples_value = parse_unsigned(parser, "samples", DEFAULT_SAMPLES);
        const uint64_t warmups_value = parse_unsigned(parser, "warmups", DEFAULT_WARMUPS);
        if (work_units == 0) { throw std::runtime_error("--work-units must be positive."); }
        if (samples_value == 0 || samples_value > std::numeric_limits<unsigned>::max()
            || warmups_value > std::numeric_limits<unsigned>::max()) {
            throw std::runtime_error("Sample counts are outside the supported range.");
        }

        QStringList requested = parser.values("scenario");
        if (requested.empty()) {
            for (const Scenario &scenario : scenarios) {
                requested.append(scenario.name);
            }
        }

        QJsonArray benchmarks;
        for (const QString &name : requested) {
            const Scenario *scenario = find_scenario(scenarios, name);
            if (scenario == nullptr) {
                throw std::runtime_error(
                    QString("Unknown benchmark scenario '%1'.").arg(name).toStdString());
            }
            if (work_units % scenario->workload_profile->instructions_per_iteration != 0) {
                throw std::runtime_error(
                    QString("--work-units must be a multiple of %1 for workload '%2'.")
                        .arg(scenario->workload_profile->instructions_per_iteration)
                        .arg(scenario->workload_profile->name)
                        .toStdString());
            }
            benchmarks.append(benchmark_scenario(
                app, *scenario, work_units, static_cast<unsigned>(warmups_value),
                static_cast<unsigned>(samples_value)));
        }

        QJsonObject root = {
            { "schema", "qtrvsim.component-benchmark" },
            { "version", 1 },
            { "environment",
              QJsonObject {
                  { "build_type", QTRVSIM_BENCHMARK_BUILD_TYPE },
                  { "compiler", QTRVSIM_BENCHMARK_COMPILER },
                  { "qt_version", qVersion() },
                  { "cpu_architecture", QSysInfo::currentCpuArchitecture() },
                  { "kernel_type", QSysInfo::kernelType() },
                  { "kernel_version", QSysInfo::kernelVersion() },
              } },
            { "parameters",
              QJsonObject {
                  { "samples", static_cast<double>(samples_value) },
                  { "warmups", static_cast<double>(warmups_value) },
                  { "work_units", static_cast<double>(work_units) },
              } },
            { "benchmarks", benchmarks },
        };
        write_output(QJsonDocument(root), parser.value("output"));
    } catch (const std::exception &exception) {
        fprintf(stderr, "Benchmark failed: %s\n", exception.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
