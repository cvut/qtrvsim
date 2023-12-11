#include "reporter.h"

#include <cinttypes>

using namespace machine;
using namespace std;

Reporter::Reporter(QCoreApplication *app, Machine *machine)
    : QObject()
    , app(app)
    , machine(machine) {
    connect(machine, &Machine::program_exit, this, &Reporter::machine_exit);
    connect(machine, &Machine::program_trap, this, &Reporter::machine_trap);
    connect(
        machine->core(), &Core::stop_on_exception_reached, this,
        &Reporter::machine_exception_reached);
}

void Reporter::add_dump_range(Address start, size_t len, const QString &path_to_write) {
    dump_ranges.append({ start, len, path_to_write });
}

void Reporter::machine_exit() {
    report();
    if (e_fail != 0) {
        printf("Machine was expected to fail but it didn't.\n");
        QCoreApplication::exit(1);
    } else {
        QCoreApplication::exit();
    }
}

/* TODO: Decide whether this should be moved to machine to avoid duplication or kept aside as
         a visualization concern */
constexpr const char *get_exception_name(ExceptionCause exception_cause) {
    switch (exception_cause) {
    case EXCAUSE_NONE: return "NONE";
    case EXCAUSE_INSN_FAULT: return "INSN_FAULT";
    case EXCAUSE_INSN_ILLEGAL: return "INSN_ILLEGAL";
    case EXCAUSE_BREAK: return "BREAK";
    case EXCAUSE_LOAD_MISALIGNED: return "LOAD_MISALIGNED";
    case EXCAUSE_LOAD_FAULT: return "LOAD_FAULT";
    case EXCAUSE_STORE_MISALIGNED: return "STORE_MISALIGNED";
    case EXCAUSE_STORE_FAULT: return "STORE_FAULT";
    case EXCAUSE_ECALL_U: return "ECALL_U";
    case EXCAUSE_ECALL_S: return "ECALL_S";
    case EXCAUSE_RESERVED_10: return "RESERVED_10";
    case EXCAUSE_ECALL_M: return "ECALL_M";
    case EXCAUSE_INSN_PAGE_FAULT: return "INSN_PAGE_FAULT";
    case EXCAUSE_LOAD_PAGE_FAULT: return "LOAD_PAGE_FAULT";
    case EXCAUSE_RESERVED_14: return "RESERVED_14";
    case EXCAUSE_STORE_PAGE_FAULT: return "STORE_PAGE_FAULT";
    // Simulator specific exception cause codes, alliases
    case EXCAUSE_HWBREAK: return "HWBREAK";
    case EXCAUSE_ECALL_ANY: return "ECALL_ANY";
    case EXCAUSE_INT: return "INT";
    default: UNREACHABLE
    }
}

void Reporter::machine_exception_reached() {
    ExceptionCause excause = machine->get_exception_cause();
    printf("Machine stopped on %s exception.\n", get_exception_name(excause));
    report();
    QCoreApplication::exit();
}

void Reporter::cycle_limit_reached() {
    printf("Specified cycle limit reached\n");
    report();
    QCoreApplication::exit();
}

void Reporter::machine_trap(SimulatorException &e) {
    report();

    bool expected = false;
    if (typeid(e) == typeid(SimulatorExceptionUnsupportedInstruction)) {
        expected = e_fail & FR_UNSUPPORTED_INSTR;
    }

    printf("Machine trapped: %s\n", qPrintable(e.msg(false)));
    QCoreApplication::exit(expected ? 0 : 1);
}

void Reporter::report() {
    if (e_regs | e_cycles | e_cycles | e_fail) { printf("Machine state report:\n"); }

    if (e_regs) { report_regs(); }
    if (e_cache_stats) { report_caches(); }
    if (e_cycles) {
        printf("cycles: %" PRIu32 "\n", machine->core()->get_cycle_count());
        printf("stalls: %" PRIu32 "\n", machine->core()->get_stall_count());
    }
    for (const DumpRange &range : dump_ranges) {
        report_range(range);
    }
}

void Reporter::report_regs() const {
    printf("PC:0x%08" PRIx64 "\n", machine->registers()->read_pc().get_raw());
    for (unsigned i = 0; i < REGISTER_COUNT; i++) {
        report_gp_reg(i, (i == REGISTER_COUNT - 1));
    }
    for (size_t i = 0; i < CSR::REGISTERS.size(); i++) {
        report_csr_reg(i, (i == CSR::REGISTERS.size() - 1));
    }
}

void Reporter::report_gp_reg(unsigned int i, bool last) const {
    printf(
        "R%u:0x%08" PRIx64 "%s", i, machine->registers()->read_gp(i).as_u64(), (last) ? "\n" : " ");
}

void Reporter::report_csr_reg(size_t internal_id, bool last) const {
    printf(
        "%s: 0x%08" PRIx64 "%s", CSR::REGISTERS[internal_id].name,
        machine->control_state()->read_internal(internal_id).as_u64(), (last) ? "\n" : " ");
}

void Reporter::report_caches() const {
    printf("Cache statistics report:\n");
    report_cache("i-cache", *machine->cache_program());
    report_cache("d-cache", *machine->cache_data());
}

void Reporter::report_cache(const char *cache_name, const Cache &cache) {
    printf("%s:reads: %" PRIu32 "\n", cache_name, cache.get_read_count());
    printf("%s:hit: %" PRIu32 "\n", cache_name, cache.get_hit_count());
    printf("%s:miss: %" PRIu32 "\n", cache_name, cache.get_miss_count());
    printf("%s:hit-rate: %.3lf\n", cache_name, cache.get_hit_rate());
    printf("%s:stalled-cycles: %" PRIu32 "\n", cache_name, cache.get_stall_count());
    printf("%s:improved-speed: %.3lf\n", cache_name, cache.get_speed_improvement());
}

void Reporter::report_range(const Reporter::DumpRange &range) const {
    FILE *out = fopen(range.path_to_write.toLocal8Bit().data(), "w");
    if (out == nullptr) {
        fprintf(
            stderr, "Failed to open %s for writing\n", range.path_to_write.toLocal8Bit().data());
        return;
    }
    Address start = range.start & ~3;
    Address end = range.start + range.len;
    if (end < start) { end = 0xffffffff_addr; }
    // TODO: report also cached memory?
    const MemoryDataBus *mem = machine->memory_data_bus();
    for (Address addr = start; addr < end; addr += 4) {
        fprintf(out, "0x%08" PRIx32 "\n", mem->read_u32(addr, ae::INTERNAL));
    }
    if (fclose(out)) {
        fprintf(stderr, "Failure closing %s\n", range.path_to_write.toLocal8Bit().data());
    }
}
