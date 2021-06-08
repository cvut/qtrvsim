#include "reporter.h"

#include <cinttypes>
#include <utility>

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

void Reporter::machine_exception_reached() {
    ExceptionCause excause = machine->get_exception_cause();
    switch (excause) {
    case EXCAUSE_NONE: printf("Machine stopped on NONE exception.\n"); break;
    case EXCAUSE_INT: printf("Machine stopped on INT exception.\n"); break;
    case EXCAUSE_ADDRL: printf("Machine stopped on ADDRL exception.\n"); break;
    case EXCAUSE_ADDRS: printf("Machine stopped on ADDRS exception.\n"); break;
    case EXCAUSE_IBUS: printf("Machine stopped on IBUS exception.\n"); break;
    case EXCAUSE_DBUS: printf("Machine stopped on DBUS exception.\n"); break;
    case EXCAUSE_SYSCALL: printf("Machine stopped on SYSCALL exception.\n"); break;
    case EXCAUSE_OVERFLOW: printf("Machine stopped on OVERFLOW exception.\n"); break;
    case EXCAUSE_TRAP: printf("Machine stopped on TRAP exception.\n"); break;
    case EXCAUSE_HWBREAK: printf("Machine stopped on HWBREAK exception.\n"); break;
    default: break;
    }
    report();
    QCoreApplication::exit();
}

void Reporter::machine_trap(SimulatorException &e) {
    report();

    bool expected = false;
    auto &etype = typeid(e);
    if (etype == typeid(SimulatorExceptionUnsupportedInstruction)) {
        expected = e_fail & FR_UNSUPPORTED_INSTR;
    }

    printf("Machine trapped: %s\n", qPrintable(e.msg(false)));
    QCoreApplication::exit(expected ? 0 : 1);
}

void Reporter::report() {
    if (e_regs) {
        printf("Machine state report:\n");
        printf("PC:0x%08" PRIx64 "\n", machine->registers()->read_pc().get_raw());
        for (unsigned i = 0; i < 32; i++) {
            printf(
                "R%u:0x%08" PRIx64 "%s", i, machine->registers()->read_gp(i).as_u64(),
                (i == 31) ? "\n" : " ");
        }
    }
    for (int i = 1; i < Cop0State::COP0REGS_CNT; i++) {
        printf(
            "%s: 0x%08" PRIx32 "%s",
            Cop0State::cop0reg_name((Cop0State::Cop0Registers)i).toLocal8Bit().data(),
            machine->cop0state()->read_cop0reg((Cop0State::Cop0Registers)i),
            (i == Cop0State::COP0REGS_CNT - 1) ? "\n" : " ");
    }
    if (e_cache_stats) {
        printf("Cache statistics report:\n");
        printf("i-cache:reads: %" PRIu32 "\n", machine->cache_program()->get_read_count());
        printf("i-cache:hit: %" PRIu32 "\n", machine->cache_program()->get_hit_count());
        printf("i-cache:miss: %" PRIu32 "\n", machine->cache_program()->get_miss_count());
        printf("i-cache:hit-rate: %.3lf\n", machine->cache_program()->get_hit_rate());
        printf(
            "i-cache:stalled-cycles: %" PRIu32 "\n", machine->cache_program()->get_stall_count());
        printf(
            "i-cache:improved-speed: %.3lf\n", machine->cache_program()->get_speed_improvement());
        printf("d-cache:reads: %" PRIu32 "\n", machine->cache_data()->get_read_count());
        printf("d-cache:writes: %" PRIu32 "\n", machine->cache_data()->get_write_count());
        printf("d-cache:hit: %" PRIu32 "\n", machine->cache_data()->get_hit_count());
        printf("d-cache:miss: %" PRIu32 "\n", machine->cache_data()->get_miss_count());
        printf("d-cache:hit-rate: %.3lf\n", machine->cache_data()->get_hit_rate());
        printf("d-cache:stalled-cycles: %" PRIu32 "\n", machine->cache_data()->get_stall_count());
        printf("d-cache:improved-speed: %.3lf\n", machine->cache_data()->get_speed_improvement());
    }
    if (e_cycles) {
        printf("cycles: %" PRIu32 "\n", machine->core()->get_cycle_count());
        printf("stalls: %" PRIu32 "\n", machine->core()->get_stall_count());
    }
    for (const DumpRange &range : dump_ranges) {
        FILE *out = fopen(range.path_to_write.toLocal8Bit().data(), "w");
        if (out == nullptr) {
            fprintf(
                stderr, "Failed to open %s for writing\n",
                range.path_to_write.toLocal8Bit().data());
            return;
        }
        Address start = range.start & ~3;
        Address end = range.start + range.len;
        if (end < start) { end = 0xffffffff_addr; }
        const MemoryDataBus *mem = machine->memory_data_bus();
        for (Address addr = start; addr < end; addr += 4) {
            fprintf(out, "0x%08" PRIu32 "\n", mem->read_u32(addr, ae::INTERNAL));
        }
        if (!fclose(out)) {
            fprintf(stderr, "Failure closing %s\n", range.path_to_write.toLocal8Bit().data());
        }
    }
}
