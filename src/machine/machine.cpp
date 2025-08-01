#include "machine.h"

#include "memory/tlb/tlb.h"
#include "memory/virtual/page_fault_handler.h"
#include "programloader.h"

#include <QTime>
#include <iostream>
#include <ostream>
#include <qelapsedtimer.h>
#include <utility>

LOG_CATEGORY("machine");

using namespace machine;

Machine::Machine(MachineConfig config, bool load_symtab, bool load_executable)
    : machine_config(std::move(config))
    , stat(ST_READY) {
    regs = new Registers();

    controlst
        = new CSR::ControlState(machine_config.get_simulated_xlen(), machine_config.get_isa_word());

    uint32_t sv32_root_ppn = 0;
    if (load_executable) {
        ProgramLoader program(machine_config.elf());
        this->machine_config.set_simulated_endian(program.get_endian());
        mem_program_only = new Memory(machine_config.get_simulated_endian());
        program.to_memory(mem_program_only);
        if (program.get_architecture_type() == ARCH64)
            this->machine_config.set_simulated_xlen(Xlen::_64);
        else
            this->machine_config.set_simulated_xlen(Xlen::_32);
        if (load_symtab) { symtab = program.get_symbol_table(); }
        program_end = program.end();
        if (machine_config.get_vm_enabled()
            && machine_config.get_vm_mode() == MachineConfig::VM_SV32) {
            uint32_t base_va = machine_config.get_va_base_addr();
            program_end = Address { program_end.get_raw() + base_va };
        }

        if (program.get_executable_entry() != 0x0_addr) {
            uint32_t base_va = 0;
            if (machine_config.get_vm_enabled()
                && machine_config.get_vm_mode() == MachineConfig::VM_SV32) {
                base_va = machine_config.get_va_base_addr();
            }
            regs->write_pc(Address { program.get_executable_entry().get_raw() + base_va });
        }
        mem = new Memory(*mem_program_only);

        if (machine_config.get_vm_enabled()
            && machine_config.get_vm_mode() == MachineConfig::VM_SV32) {
            sv32_root_ppn = allocate_page();
            uint32_t asid = machine_config.get_vm_asid() & 0x1FFu;
            uint32_t satp = (1u << 31) | (asid << 22) | sv32_root_ppn;
            controlst->write_internal(CSR::Id::SATP, satp);
            uint32_t base_va = machine_config.get_va_base_addr();
            map_elf_segments(program, mem, sv32_root_ppn, base_va);
        }
    } else {
        mem = new Memory(machine_config.get_simulated_endian());
    }
    data_bus = new MemoryDataBus(machine_config.get_simulated_endian());
    data_bus->insert_device_to_range(mem, 0x00000000_addr, 0xefffffff_addr, false);

    setup_serial_port();
    setup_perip_spi_led();
    setup_lcd_display();
    setup_aclint_mtime();
    setup_aclint_mswi();
    setup_aclint_sswi();

    unsigned access_time_read = machine_config.memory_access_time_read();
    unsigned access_time_write = machine_config.memory_access_time_write();
    unsigned access_time_burst = machine_config.memory_access_time_burst();
    bool access_enable_burst = machine_config.memory_access_enable_burst();

    cch_level2 = new Cache(
        data_bus, &machine_config.cache_level2(), access_time_read, access_time_write,
        access_time_burst, access_enable_burst);
    if (machine_config.cache_level2().enabled()) {
        access_time_read = machine_config.memory_access_time_level2();
        access_time_write = machine_config.memory_access_time_level2();
        access_time_burst = 0;
        access_enable_burst = true;
    }
    cch_program = new Cache(
        cch_level2, &machine_config.cache_program(), access_time_read, access_time_write,
        access_time_burst, access_enable_burst);
    cch_data = new Cache(
        cch_level2, &machine_config.cache_data(), access_time_read, access_time_write,
        access_time_burst, access_enable_burst);

    if (machine_config.get_vm_enabled() && machine_config.get_vm_mode() == MachineConfig::VM_SV32) {
        uint32_t asid = machine_config.get_vm_asid() & 0x1FFu;
        uint32_t satp = (1u << 31) | (asid << 22) | sv32_root_ppn;

        tlb_program.emplace(cch_program, PROGRAM, this);
        tlb_data.emplace(cch_data, DATA, this);

        controlst->write_internal(CSR::Id::SATP, satp);
        tlb_program->on_csr_write(CSR::Id::SATP, satp);
        tlb_data->on_csr_write(CSR::Id::SATP, satp);

        auto *pfh_data = new PageFaultHandler(this, *tlb_data);
        tlb_data->set_page_fault_handler(pfh_data);
        auto *pfh_instr = new PageFaultHandler(this, *tlb_program);
        tlb_program->set_page_fault_handler(pfh_instr);

        connect(&*tlb_data, &TLB::firstWrite, this, [this](VirtualAddress va) {
            regs->write_pc(Address { va.get_raw() });
        });

    } else if (
        machine_config.get_vm_enabled() && machine_config.get_vm_mode() == MachineConfig::VM_BARE) {
        tlb_program.emplace(cch_program, PROGRAM, this);
        tlb_data.emplace(cch_data, DATA, this);
        controlst->write_internal(CSR::Id::SATP, 0);
        tlb_program->on_csr_write(CSR::Id::SATP, 0);
        tlb_data->on_csr_write(CSR::Id::SATP, 0);
        LOG("Machine: running in bare mode (identity mapping via TLB/PTW).");
    }

    instr_if_ = tlb_program ? static_cast<FrontendMemory *>(&*tlb_program)
                            : static_cast<FrontendMemory *>(cch_program);
    data_if_ = tlb_data ? static_cast<FrontendMemory *>(&*tlb_data)
                        : static_cast<FrontendMemory *>(cch_data);

    predictor = new BranchPredictor(
        machine_config.get_bp_enabled(), machine_config.get_bp_type(),
        machine_config.get_bp_init_state(), machine_config.get_bp_btb_bits(),
        machine_config.get_bp_bhr_bits(), machine_config.get_bp_bht_addr_bits());

    if (machine_config.pipelined()) {
        cr = new CorePipelined(
            regs, predictor, instr_if_, data_if_, controlst, machine_config.get_simulated_xlen(),
            machine_config.get_isa_word(), machine_config.hazard_unit());

    } else {
        cr = new CoreSingle(
            regs, predictor, instr_if_, data_if_, controlst, machine_config.get_simulated_xlen(),
            machine_config.get_isa_word());
    }
    connect(
        this, &Machine::set_interrupt_signal, controlst, &CSR::ControlState::set_interrupt_signal);

    run_t = new QTimer(this);
    set_speed(0); // In default run as fast as possible
    connect(run_t, &QTimer::timeout, this, &Machine::step_timer);

    for (int i = 0; i < EXCAUSE_COUNT; i++) {
        if (i != EXCAUSE_INT && i != EXCAUSE_BREAK && i != EXCAUSE_HWBREAK) {
            set_stop_on_exception((enum ExceptionCause)i, machine_config.osemu_exception_stop());
            set_step_over_exception((enum ExceptionCause)i, machine_config.osemu_exception_stop());
        }
    }

    set_stop_on_exception(EXCAUSE_INT, machine_config.osemu_interrupt_stop());
    set_step_over_exception(EXCAUSE_INT, false);
}
void Machine::setup_lcd_display() {
    perip_lcd_display = new LcdDisplay(machine_config.get_simulated_endian());
    memory_bus_insert_range(perip_lcd_display, 0xffe00000_addr, 0xffe4afff_addr, true);
    if (machine_config.get_simulated_xlen() == Xlen::_64)
        memory_bus_insert_range(
            perip_lcd_display, 0xffffffffffe00000_addr, 0xffffffffffe4afff_addr, false);
}
void Machine::setup_perip_spi_led() {
    perip_spi_led = new PeripSpiLed(machine_config.get_simulated_endian());
    memory_bus_insert_range(perip_spi_led, 0xffffc100_addr, 0xffffc1ff_addr, true);
    if (machine_config.get_simulated_xlen() == Xlen::_64)
        memory_bus_insert_range(
            perip_spi_led, 0xffffffffffffc100_addr, 0xffffffffffffc1ff_addr, false);
}
void Machine::setup_serial_port() {
    ser_port = new SerialPort(machine_config.get_simulated_endian());
    memory_bus_insert_range(ser_port, 0xffffc000_addr, 0xffffc03f_addr, true);
    memory_bus_insert_range(ser_port, 0xffff0000_addr, 0xffff003f_addr, false);
    if (machine_config.get_simulated_xlen() == Xlen::_64)
        memory_bus_insert_range(ser_port, 0xffffffffffffc000_addr, 0xffffffffffffc03f_addr, false);
    connect(ser_port, &SerialPort::signal_interrupt, this, &Machine::set_interrupt_signal);
}

void Machine::setup_aclint_mtime() {
    aclint_mtimer = new aclint::AclintMtimer(machine_config.get_simulated_endian());
    memory_bus_insert_range(
        aclint_mtimer, 0xfffd0000_addr + aclint::CLINT_MTIMER_OFFSET,
        0xfffd0000_addr + aclint::CLINT_MTIMER_OFFSET + aclint::CLINT_MTIMER_SIZE - 1, true);
    if (machine_config.get_simulated_xlen() == Xlen::_64)
        memory_bus_insert_range(
            aclint_mtimer, 0xfffffffffffd0000_addr + aclint::CLINT_MTIMER_OFFSET,
            0xfffffffffffd0000_addr + aclint::CLINT_MTIMER_OFFSET + aclint::CLINT_MTIMER_SIZE - 1,
            false);
    connect(
        aclint_mtimer, &aclint::AclintMtimer::signal_interrupt, this,
        &Machine::set_interrupt_signal);
}

void Machine::setup_aclint_mswi() {
    aclint_mswi = new aclint::AclintMswi(machine_config.get_simulated_endian());
    memory_bus_insert_range(
        aclint_mswi, 0xfffd0000_addr + aclint::CLINT_MSWI_OFFSET,
        0xfffd0000_addr + aclint::CLINT_MSWI_OFFSET + aclint::CLINT_MSWI_SIZE - 1, true);
    if (machine_config.get_simulated_xlen() == Xlen::_64)
        memory_bus_insert_range(
            aclint_mswi, 0xfffffffffffd0000_addr + aclint::CLINT_MSWI_OFFSET,
            0xfffffffffffd0000_addr + aclint::CLINT_MSWI_OFFSET + aclint::CLINT_MSWI_SIZE - 1,
            false);
    connect(
        aclint_mswi, &aclint::AclintMswi::signal_interrupt, this, &Machine::set_interrupt_signal);
}

void Machine::setup_aclint_sswi() {
    aclint_sswi = new aclint::AclintSswi(machine_config.get_simulated_endian());
    memory_bus_insert_range(
        aclint_sswi, 0xfffd0000_addr + aclint::CLINT_SSWI_OFFSET,
        0xfffd0000_addr + aclint::CLINT_SSWI_OFFSET + aclint::CLINT_SSWI_SIZE - 1, true);
    if (machine_config.get_simulated_xlen() == Xlen::_64)
        memory_bus_insert_range(
            aclint_sswi, 0xfffffffffffd0000_addr + aclint::CLINT_SSWI_OFFSET,
            0xfffffffffffd0000_addr + aclint::CLINT_SSWI_OFFSET + aclint::CLINT_SSWI_SIZE - 1,
            false);
    connect(
        aclint_sswi, &aclint::AclintSswi::signal_interrupt, this, &Machine::set_interrupt_signal);
}

Machine::~Machine() {
    delete run_t;
    run_t = nullptr;
    delete cr;
    cr = nullptr;
    delete controlst;
    controlst = nullptr;
    delete regs;
    regs = nullptr;
    delete mem;
    mem = nullptr;
    tlb_program.reset();
    tlb_data.reset();
    delete cch_program;
    cch_program = nullptr;
    delete cch_data;
    cch_data = nullptr;
    delete cch_level2;
    cch_level2 = nullptr;
    delete data_bus;
    data_bus = nullptr;
    delete mem_program_only;
    mem_program_only = nullptr;
    delete symtab;
    symtab = nullptr;
    delete predictor;
    predictor = nullptr;
}

const MachineConfig &Machine::config() {
    return machine_config;
}

void Machine::set_speed(unsigned int ips, unsigned int time_chunk) {
    this->time_chunk = time_chunk;
    run_t->setInterval(ips);
    if (run_t->isActive()) {
        // Clock settings changed.
        start_core_clock();
    }
}

const Registers *Machine::registers() {
    return regs;
}

const CSR::ControlState *Machine::control_state() {
    return controlst;
}

const Memory *Machine::memory() {
    return mem;
}

Memory *Machine::memory_rw() {
    return mem;
}

const Cache *Machine::cache_program() {
    return cch_program;
}

const Cache *Machine::cache_data() {
    return cch_data;
}

const Cache *Machine::cache_level2() {
    return cch_level2;
}

Cache *Machine::cache_data_rw() {
    return cch_data;
}

void Machine::cache_sync() {
    if (cch_program != nullptr) { cch_program->sync(); }
    if (cch_data != nullptr) { cch_data->sync(); }
    if (cch_level2 != nullptr) { cch_level2->sync(); }
}

const MemoryDataBus *Machine::memory_data_bus() {
    return data_bus;
}

MemoryDataBus *Machine::memory_data_bus_rw() {
    return data_bus;
}

SerialPort *Machine::serial_port() {
    return ser_port;
}

PeripSpiLed *Machine::peripheral_spi_led() {
    return perip_spi_led;
}

LcdDisplay *Machine::peripheral_lcd_display() {
    return perip_lcd_display;
}

SymbolTable *Machine::symbol_table_rw(bool create) {
    if (create && (symtab == nullptr)) { symtab = new SymbolTable; }
    return symtab;
}

const SymbolTable *Machine::symbol_table(bool create) {
    return symbol_table_rw(create);
}

void Machine::set_symbol(
    const QString &name,
    uint32_t value,
    uint32_t size,
    unsigned char info,
    unsigned char other) {
    if (symtab == nullptr) { symtab = new SymbolTable; }
    symtab->set_symbol(name, value, size, info, other);
}

const Core *Machine::core() {
    return cr;
}

const CoreSingle *Machine::core_singe() {
    return machine_config.pipelined() ? nullptr : (const CoreSingle *)cr;
}

const CorePipelined *Machine::core_pipelined() {
    return machine_config.pipelined() ? (const CorePipelined *)cr : nullptr;
}

bool Machine::executable_loaded() const {
    return (mem_program_only != nullptr);
}

enum Machine::Status Machine::status() {
    return stat;
}

bool Machine::exited() {
    return stat == ST_EXIT || stat == ST_TRAPPED;
}

// We don't allow to call control methods when machine exited or if it's busy
// We rather silently fail.
#define CTL_GUARD                                                                                  \
    do {                                                                                           \
        if (exited() || stat == ST_BUSY) return;                                                   \
    } while (false)

void Machine::play() {
    CTL_GUARD;
    set_status(ST_RUNNING);
    start_core_clock();
    step_internal(true);
    emit play_initiated();
}

void Machine::pause() {
    if (stat != ST_BUSY) { CTL_GUARD; }
    set_status(ST_READY);
    stop_core_clock();
    emit play_paused();
}

void Machine::step_internal(bool skip_break) {
    CTL_GUARD;
    enum Status stat_prev = stat;
    set_status(ST_BUSY);
    emit tick();
    try {
        QElapsedTimer timer;
        timer.start();
        do {
            cr->step(skip_break);
        } while (time_chunk != 0 && stat == ST_BUSY && !skip_break
                 && timer.elapsed() < (int)time_chunk);
    } catch (SimulatorException &e) {
        stop_core_clock();
        set_status(ST_TRAPPED);
        emit program_trap(e);
        return;
    }
    if (regs->read_pc() >= program_end) {
        stop_core_clock();
        set_status(ST_EXIT);
        emit program_exit();
    } else {
        if (stat == ST_BUSY) { set_status(stat_prev); }
    }
    emit post_tick();
}

void Machine::start_core_clock() {
    // Handle frequency measurement.
    last_cycle_count = cr->get_cycle_count();
    if (run_t->interval() == 0) {
        // The clock is not fixed, we need to measure it.
        frequency_timer.start();
    } else {
        // Clocked is fixed.
        emit report_core_frequency(1e3 / (double)run_t->interval());
    }

    run_t->start();
}

void Machine::stop_core_clock() {
    run_t->stop();
    frequency_timer.invalidate();
    emit report_core_frequency(0.0);
}

void Machine::step() {
    step_internal(true);
}

void Machine::step_timer() {
    if (run_t->interval() == 0 && time_chunk == 0) {
        // We need to amortize QTimer event loop overhead when running in max speed mode.
        for (size_t i = 0; i < 32 && stat == ST_RUNNING; i++) {
            step_internal();
        }
    } else {
        step_internal();
    }
    // Compute core frequency each 0x100 cycles
    auto total_cycle_count = cr->get_cycle_count();
    auto cycle_count = total_cycle_count - last_cycle_count;
    if (cycle_count >= 0x2000) {
        double period_ns = (double)(frequency_timer.nsecsElapsed()) / cycle_count;
        if (period_ns < 0.01) { return; }
        last_cycle_count = cr->get_cycle_count();
        emit frequency_timer.start();
        report_core_frequency(1e9 / period_ns);
    }
}

void Machine::restart() {
    pause();
    regs->reset();
    if (mem_program_only != nullptr) { mem->reset(*mem_program_only); }
    cch_program->reset();
    cch_data->reset();
    cch_level2->reset();
    cr->reset();
    set_status(ST_READY);
}

void Machine::set_status(enum Status st) {
    bool change = st != stat;
    stat = st;
    if (change) { emit status_change(st); }
}

void Machine::register_exception_handler(ExceptionCause excause, ExceptionHandler *exhandler) {
    if (cr != nullptr) { cr->register_exception_handler(excause, exhandler); }
}

bool Machine::memory_bus_insert_range(
    BackendMemory *mem_acces,
    Address start_addr,
    Address last_addr,
    bool move_ownership) {
    if (data_bus == nullptr) { return false; }
    return data_bus->insert_device_to_range(mem_acces, start_addr, last_addr, move_ownership);
}

void Machine::insert_hwbreak(Address address) {
    if (cr != nullptr) { cr->insert_hwbreak(address); }
}

void Machine::remove_hwbreak(Address address) {
    if (cr != nullptr) { cr->remove_hwbreak(address); }
}

bool Machine::is_hwbreak(Address address) {
    if (cr != nullptr) { return cr->is_hwbreak(address); }
    return false;
}

void Machine::set_stop_on_exception(enum ExceptionCause excause, bool value) {
    if (cr != nullptr) { cr->set_stop_on_exception(excause, value); }
}

bool Machine::get_stop_on_exception(enum ExceptionCause excause) const {
    if (cr != nullptr) { return cr->get_stop_on_exception(excause); }
    return false;
}

void Machine::set_step_over_exception(enum ExceptionCause excause, bool value) {
    if (cr != nullptr) { cr->set_step_over_exception(excause, value); }
}

bool Machine::get_step_over_exception(enum ExceptionCause excause) const {
    if (cr != nullptr) { return cr->get_step_over_exception(excause); }
    return false;
}

enum ExceptionCause Machine::get_exception_cause() const {
    uint32_t val;
    if (controlst == nullptr) { return EXCAUSE_NONE; }
    val = (controlst->read_internal(CSR::Id::MCAUSE).as_u64());
    if (val & 0xffffffff80000000) {
        return EXCAUSE_INT;
    } else {
        return (ExceptionCause)val;
    }
}

void Machine::map_elf_segments(
    ProgramLoader &program,
    Memory *mem,
    uint32_t root_ppn,
    uint32_t base_va) {
    constexpr unsigned PAGE_SHIFT = 12;
    constexpr unsigned PAGE_SIZE = 1u << PAGE_SHIFT;
    constexpr unsigned VPN1_SHIFT = PAGE_SHIFT + 10;
    constexpr unsigned VPN0_SHIFT = PAGE_SHIFT;
    constexpr unsigned VPN_MASK = (1u << 10) - 1;

    auto rd32 = [&](uint64_t a) {
        uint32_t v;
        mem->read(&v, Offset { a }, sizeof(v), { .type = ae::INTERNAL });
        return v;
    };
    auto wr32 = [&](uint64_t a, uint32_t v) {
        mem->write(Offset { a }, &v, sizeof(v), { .type = ae::INTERNAL });
    };

    for (auto seg : program.get_load_segments()) {
        uint32_t vbase = seg.vaddr + base_va;
        uint32_t fsize = seg.size;
        uint32_t perms = seg.flags;
        uint32_t npages = (fsize + PAGE_SIZE - 1) / PAGE_SIZE;

        for (uint32_t i = 0; i < npages; i++) {
            uint32_t va_page = vbase + i * PAGE_SIZE;
            uint32_t vpn1 = (va_page >> VPN1_SHIFT) & VPN_MASK;
            uint32_t vpn0 = (va_page >> VPN0_SHIFT) & VPN_MASK;
            uint32_t current_ppn = root_ppn;
            uint64_t pte_addr = 0;

            for (int lvl = 1; lvl >= 0; --lvl) {
                uint32_t idx = (lvl == 1 ? vpn1 : vpn0);
                pte_addr = (uint64_t(current_ppn) << PAGE_SHIFT) + idx * 4;
                Sv32Pte pte = Sv32Pte::from_uint(rd32(pte_addr));

                if (!pte.is_valid()) {
                    uint32_t new_ppn = allocate_page();
                    Sv32Pte npte {};
                    npte.v = 1;
                    npte.ppn = new_ppn;
                    wr32(pte_addr, npte.to_uint());
                    current_ppn = new_ppn;
                } else if (pte.is_leaf()) {
                    throw SIMULATOR_EXCEPTION(
                        PageFault, "map_elf_segments: unexpected leaf PTE",
                        QString::number(pte_addr, 16));
                } else {
                    current_ppn = pte.ppn;
                }
            }

            uint32_t phys_ppn = seg.vaddr >> PAGE_SHIFT;
            Sv32Pte leaf {};
            leaf.v = 1;
            leaf.r = (perms & PF_R) ? 1 : 0;
            leaf.w = (perms & PF_W) ? 1 : 0;
            leaf.x = (perms & PF_X) ? 1 : 0;
            leaf.a = 1;
            leaf.d = 1;
            leaf.ppn = phys_ppn;
            wr32(pte_addr, leaf.to_uint());
        }
    }
}
