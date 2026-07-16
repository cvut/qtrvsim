#include "machine/core.h"
#include "machine/csr/controlstate.h"
#include "machine/instruction.h"
#include "machine/memory/backend/memory.h"
#include "machine/memory/memory_bus.h"
#include "machine/memory/memory_utils.h"
#include "machine/predictor.h"
#include "machine/registers.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSysInfo>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <limits>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <vector>

using namespace machine;

namespace {

constexpr uint64_t DEFAULT_WORK_UNITS = 100000;
constexpr unsigned DEFAULT_SAMPLES = 10;
constexpr unsigned DEFAULT_WARMUPS = 2;
constexpr uint64_t LOOP_INSTRUCTION_COUNT = 5;

struct Scenario {
    const char *name;
    bool pipelined;
    bool predictor_enabled;
};

constexpr Scenario SCENARIOS[] = {
    { "core-single", false, false },
    { "core-pipeline", true, false },
    { "core-pipeline-predictor", true, true },
};

struct RunResult {
    uint64_t duration_ns;
    uint64_t cycles;
    uint64_t instructions;
    uint64_t checksum;
};

class CoreWorkload {
public:
    explicit CoreWorkload(const Scenario &scenario)
        : memory_backend(LITTLE)
        , memory(&memory_backend)
        , control_state(Xlen::_32, config_isa_word_default)
        , predictor(
              scenario.predictor_enabled,
              scenario.predictor_enabled ? PredictorType::BTFNT : PredictorType::SMITH_1_BIT,
              PredictorState::NOT_TAKEN) {
        load_program();
        if (scenario.pipelined) {
            core = std::make_unique<CorePipelined>(
                &registers, &predictor, &memory, &memory, &control_state, Xlen::_32,
                config_isa_word_default, MachineConfig::HU_STALL_FORWARD);
        } else {
            core = std::make_unique<CoreSingle>(
                &registers, &predictor, &memory, &memory, &control_state, Xlen::_32,
                config_isa_word_default);
        }
    }

    RunResult run(uint64_t work_units) {
        const auto start = std::chrono::steady_clock::now();
        while (core->get_instruction_count() < work_units) {
            core->step(true);
            if (core->get_cycle_count() > work_units * 10) {
                throw std::runtime_error("Core workload did not make expected forward progress.");
            }
        }
        const auto end = std::chrono::steady_clock::now();

        validate(work_units);
        return {
            static_cast<uint64_t>(
                std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()),
            core->get_cycle_count(),
            core->get_instruction_count(),
            checksum(),
        };
    }

private:
    void load_program() {
        const QString loop_target = QString::number(registers.read_pc().get_raw());
        const QString instructions[] = {
            "addi x5, x5, 1",
            "addi x6, x6, 3",
            "xor x7, x5, x6",
            "add x8, x7, x5",
            QString("jal x0, %1").arg(loop_target),
        };

        Address pc = registers.read_pc();
        for (const QString &instruction : instructions) {
            uint32_t code[2] = {};
            const size_t size = Instruction::code_from_string(code, sizeof(code), instruction, pc);
            if (size != sizeof(uint32_t)) {
                throw std::runtime_error(
                    QString("Failed to encode workload instruction '%1'.")
                        .arg(instruction)
                        .toStdString());
            }
            memory_write_u32(&memory_backend, pc.get_raw(), code[0]);
            pc += sizeof(uint32_t);
        }
    }

    void validate(uint64_t work_units) const {
        if (core->get_instruction_count() != work_units) {
            throw std::runtime_error("Core workload retired an unexpected instruction count.");
        }

        const uint32_t loops = static_cast<uint32_t>(work_units / LOOP_INSTRUCTION_COUNT);
        const uint32_t expected_x5 = loops;
        const uint32_t expected_x6 = loops * 3;
        const uint32_t expected_x7 = expected_x5 ^ expected_x6;
        const uint32_t expected_x8 = expected_x7 + expected_x5;
        if (registers.read_gp(5).as_u32() != expected_x5
            || registers.read_gp(6).as_u32() != expected_x6
            || registers.read_gp(7).as_u32() != expected_x7
            || registers.read_gp(8).as_u32() != expected_x8) {
            throw std::runtime_error("Core workload final register state is incorrect.");
        }
    }

    uint64_t checksum() const {
        uint64_t hash = UINT64_C(1469598103934665603);
        for (unsigned reg : { 5U, 6U, 7U, 8U }) {
            hash ^= registers.read_gp(reg).as_u32();
            hash *= UINT64_C(1099511628211);
        }
        return hash;
    }

    Memory memory_backend;
    TrivialBus memory;
    Registers registers;
    CSR::ControlState control_state;
    BranchPredictor predictor;
    std::unique_ptr<Core> core;
};

const Scenario *find_scenario(const QString &name) {
    for (const Scenario &scenario : SCENARIOS) {
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
    const Scenario &scenario,
    uint64_t work_units,
    unsigned warmups,
    unsigned samples) {
    for (unsigned i = 0; i < warmups; ++i) {
        CoreWorkload workload(scenario);
        workload.run(work_units);
    }

    std::vector<uint64_t> durations;
    durations.reserve(samples);
    uint64_t expected_cycles = 0;
    uint64_t expected_checksum = 0;
    for (unsigned i = 0; i < samples; ++i) {
        CoreWorkload workload(scenario);
        const RunResult result = workload.run(work_units);
        if (i == 0) {
            expected_cycles = result.cycles;
            expected_checksum = result.checksum;
        } else if (
            result.cycles != expected_cycles || result.instructions != work_units
            || result.checksum != expected_checksum) {
            throw std::runtime_error("Benchmark samples produced inconsistent work results.");
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
        { "workload", "core-alu-loop-v1" },
        { "config",
          QJsonObject {
              { "pipelined", scenario.pipelined },
              { "branch_predictor", scenario.predictor_enabled ? "btfnt" : "disabled" },
              { "hazard_unit", scenario.pipelined ? "forward" : "none" },
          } },
        { "work",
          QJsonObject {
              { "instructions", static_cast<double>(work_units) },
              { "cycles", static_cast<double>(expected_cycles) },
              { "checksum", QString("0x%1").arg(expected_checksum, 16, 16, QLatin1Char('0')) },
          } },
        { "samples_ns", sample_values },
        { "summary",
          QJsonObject {
              { "min_ns", static_cast<double>(sorted.front()) },
              { "median_ns", median },
              { "mean_ns", mean },
              { "max_ns", static_cast<double>(sorted.back()) },
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
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("qtrvsim_component_benchmark");

    QCommandLineParser parser;
    parser.setApplicationDescription("Deterministic QtRvSim component benchmark runner.");
    parser.addHelpOption();
    parser.addOption({ "list", "List available benchmark scenarios." });
    parser.addOption(
        { "scenario", "Run a benchmark scenario. May be repeated.", "NAME" });
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

    if (parser.isSet("list")) {
        for (const Scenario &scenario : SCENARIOS) {
            printf("%s\n", scenario.name);
        }
        return EXIT_SUCCESS;
    }

    try {
        const uint64_t work_units
            = parse_unsigned(parser, "work-units", DEFAULT_WORK_UNITS);
        const uint64_t samples_value = parse_unsigned(parser, "samples", DEFAULT_SAMPLES);
        const uint64_t warmups_value = parse_unsigned(parser, "warmups", DEFAULT_WARMUPS);
        if (work_units == 0 || work_units % LOOP_INSTRUCTION_COUNT != 0) {
            throw std::runtime_error("--work-units must be a positive multiple of 5.");
        }
        if (samples_value == 0 || samples_value > std::numeric_limits<unsigned>::max()
            || warmups_value > std::numeric_limits<unsigned>::max()) {
            throw std::runtime_error("Sample counts are outside the supported range.");
        }

        QStringList requested = parser.values("scenario");
        if (requested.empty()) {
            for (const Scenario &scenario : SCENARIOS) {
                requested.append(scenario.name);
            }
        }

        QJsonArray benchmarks;
        for (const QString &name : requested) {
            const Scenario *scenario = find_scenario(name);
            if (scenario == nullptr) {
                throw std::runtime_error(
                    QString("Unknown benchmark scenario '%1'.").arg(name).toStdString());
            }
            benchmarks.append(benchmark_scenario(
                *scenario, work_units, static_cast<unsigned>(warmups_value),
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
