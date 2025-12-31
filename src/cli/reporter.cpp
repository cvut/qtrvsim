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
        exit(1);
    } else {
        exit(0);
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
    exit(0);
}

void Reporter::cycle_limit_reached() {
    printf("Specified cycle limit reached\n");
    report();
    exit(0);
}

void Reporter::machine_trap(SimulatorException &e) {
    report();

    bool expected = false;
    if (typeid(e) == typeid(SimulatorExceptionUnsupportedInstruction)) {
        expected = e_fail & FR_UNSUPPORTED_INSTR;
    }

    printf("Machine trapped: %s\n", qPrintable(e.msg(false)));
    exit(expected ? 0 : 1);
}

void Reporter::report() {
    if (dump_format & DumpFormat::CONSOLE) {
        if (e_regs | e_cycles | e_cycles | e_fail) { printf("Machine state report:\n"); }
    }

    if (e_regs) { report_regs(); }
    if (e_cache_stats) { report_caches(); }
    if (e_cycles) {
        QString cycle_count = QString::asprintf("%" PRIu32, machine->core()->get_cycle_count());
        QString stall_count = QString::asprintf("%" PRIu32, machine->core()->get_stall_count());
        if (dump_format & DumpFormat::JSON) {
            QJsonObject temp = {};
            temp["cycles"] = cycle_count;
            temp["stalls"] = stall_count;
            dump_data_json["cycles"] = temp;
        }
        if (dump_format & DumpFormat::CONSOLE) {
            printf("cycles: %s\n", qPrintable(cycle_count));
            printf("stalls: %s\n", qPrintable(stall_count));
        }
    }
    for (const DumpRange &range : dump_ranges) {
        report_range(range);
    }

    if (e_symtab) {
        QJsonObject symtab_json = {};

        for (const auto &name : machine->symbol_table()->names()) {
            SymbolValue sym_val;
            machine->symbol_table()->name_to_value(sym_val, name);
            QString value = QString::asprintf(
                machine->core()->get_xlen() == Xlen::_32 ? "0x%08" PRIx64 : "0x%016" PRIx64,
                sym_val);
            if (dump_format & DumpFormat::JSON) { symtab_json[name] = value; }
            if (dump_format & DumpFormat::CONSOLE) {
                printf("SYM[%s]: %s\n", qPrintable(name), qPrintable(value));
            }
        }

        if (dump_format & DumpFormat::JSON) { dump_data_json["symbols"] = symtab_json; }
    }

    if (e_predictor) { report_predictor(); }

    if (dump_format & DumpFormat::JSON) {
        QFile file(dump_file_json);
        QByteArray bytes = QJsonDocument(dump_data_json).toJson(QJsonDocument::Indented);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(bytes);
            file.close();
            printf("JSON object written to file\n");
        } else {
            printf("Could not open file for writing\n");
        }
    }
}

void Reporter::report_regs() {
    if (dump_format & DumpFormat::JSON) { dump_data_json["regs"] = {}; }
    report_pc();
    for (unsigned i = 0; i < REGISTER_COUNT; i++) {
        report_gp_reg(i, (i == REGISTER_COUNT - 1));
    }
    for (size_t i = 0; i < CSR::REGISTERS.size(); i++) {
        report_csr_reg(i, (i == CSR::REGISTERS.size() - 1));
    }
}

void Reporter::report_pc() {
    QString value = QString::asprintf("0x%08" PRIx64, machine->registers()->read_pc().get_raw());
    if (dump_format & DumpFormat::JSON) {
        QJsonObject regs = dump_data_json["regs"].toObject();
        regs["PC"] = value;
        dump_data_json["regs"] = regs;
    }
    if (dump_format & DumpFormat::CONSOLE) { printf("PC:%s\n", qPrintable(value)); }
}

void Reporter::report_gp_reg(unsigned int i, bool last) {
    QString key = QString::asprintf("R%u", i);
    QString value = QString::asprintf("0x%08" PRIx64, machine->registers()->read_gp(i).as_u64());
    if (dump_format & DumpFormat::JSON) {
        QJsonObject regs = dump_data_json["regs"].toObject();
        regs[key] = value;
        dump_data_json["regs"] = regs;
    }
    if (dump_format & DumpFormat::CONSOLE) {
        printf("%s:%s%s", qPrintable(key), qPrintable(value), (last) ? "\n" : " ");
    }
}

void Reporter::report_csr_reg(size_t internal_id, bool last) {
    QString key = QString::asprintf("%s", CSR::REGISTERS[internal_id].name);
    QString value = QString::asprintf(
        "0x%08" PRIx64, machine->control_state()->read_internal(internal_id).as_u64());
    if (dump_format & DumpFormat::JSON) {
        QJsonObject regs = dump_data_json["regs"].toObject();
        regs[key] = value;
        dump_data_json["regs"] = regs;
    }
    if (dump_format & DumpFormat::CONSOLE) {
        printf("%s: %s%s", qPrintable(key), qPrintable(value), (last) ? "\n" : " ");
    }
}

void Reporter::report_caches() {
    if (dump_format & DumpFormat::JSON) { dump_data_json["caches"] = {}; }
    printf("Cache statistics report:\n");
    report_cache("i-cache", *machine->cache_program());
    report_cache("d-cache", *machine->cache_data());
    if (machine->config().cache_level2().enabled()) {
        report_cache("l2-cache", *machine->cache_level2());
    }
}

void Reporter::report_cache(const char *cache_name, const Cache &cache) {
    if (dump_format & DumpFormat::JSON) {
        QJsonObject caches = dump_data_json["caches"].toObject();
        QJsonObject temp = {};
        temp["reads"] = QString::asprintf("%" PRIu32, cache.get_read_count());
        temp["hit"] = QString::asprintf("%" PRIu32, cache.get_hit_count());
        temp["miss"] = QString::asprintf("%" PRIu32, cache.get_miss_count());
        temp["hit_rate"] = QString::asprintf("%.3lf", cache.get_hit_rate());
        temp["stalled_cycles"] = QString::asprintf("%" PRIu32, cache.get_stall_count());
        temp["improved_speed"] = QString::asprintf("%.3lf", cache.get_speed_improvement());
        caches[cache_name] = temp;
        dump_data_json["caches"] = caches;
    }
    if (dump_format & DumpFormat::CONSOLE) {
        printf("%s:reads: %" PRIu32 "\n", cache_name, cache.get_read_count());
        printf("%s:hit: %" PRIu32 "\n", cache_name, cache.get_hit_count());
        printf("%s:miss: %" PRIu32 "\n", cache_name, cache.get_miss_count());
        printf("%s:hit-rate: %.3lf\n", cache_name, cache.get_hit_rate());
        printf("%s:stalled-cycles: %" PRIu32 "\n", cache_name, cache.get_stall_count());
        printf("%s:improved-speed: %.3lf\n", cache_name, cache.get_speed_improvement());
    }
}

void Reporter::report_predictor() {
    const BranchPredictor *predictor = machine->branch_predictor();
    const PredictionStatistics *stats = predictor->get_stats();

    if (dump_format & DumpFormat::JSON) { 
        QJsonObject predictorObj = dump_data_json["predictor"].toObject(); 
        QJsonObject temp = {};
        temp["total_predictions"] = QString::asprintf("%" PRIu32, stats->total);
        temp["hits"] = QString::asprintf("%" PRIu32,  stats->correct);
        temp["misses"] = QString::asprintf("%" PRIu32, stats->wrong);
        temp["accuracy"] = QString::asprintf("%.3lf", stats->accuracy);
        predictorObj = temp;
        dump_data_json["predictor"] = predictorObj;
    }
    if (dump_format & DumpFormat::CONSOLE) {
        printf("branch predictor:total predictions: %" PRIu32 "\n", stats->total);
        printf("branch predictor:hits: %" PRIu32 "\n", stats->correct);
        printf("branch predictor:misses: %" PRIu32 "\n", stats->wrong);
        printf("branch predictor:accuracy: %.3lf\n", stats->accuracy);
    }
}

void Reporter::report_range(const Reporter::DumpRange &range) {
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

void Reporter::exit(int retcode) {
    // Exit might not happen immediately, so we need to stop the machine explicitly.
    machine->pause();
    QCoreApplication::exit(retcode);
}
