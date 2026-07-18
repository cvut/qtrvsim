#include "reporter.h"

#include "utilandtext.h"

#include <cinttypes>

using namespace machine;
using namespace std;

namespace {

QJsonValue json_number(uint64_t value) {
    return static_cast<double>(value);
}

} // namespace

Reporter::Reporter(QCoreApplication *app, Machine *machine)
    : QObject()
    , app(app)
    , machine(machine) {
    connect(machine, &Machine::program_exit, this, &Reporter::machine_exit);
    connect(
        machine, &Machine::execution_limit_reached, this, &Reporter::execution_limit_reached);
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

void Reporter::machine_exception_reached() {
    ExceptionCause excause = machine->get_exception_cause();
    printf("Machine stopped on %s exception.\n", get_exception_name(excause));
    report();
    exit(0);
}

void Reporter::execution_limit_reached(enum Machine::StopReason reason) {
    if (reason == Machine::SR_CYCLE_LIMIT) {
        printf("Specified cycle limit reached\n");
    } else if (reason == Machine::SR_INSTRUCTION_LIMIT) {
        printf("Specified instruction limit reached\n");
    }
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
    if (dump_format & DumpFormat::JSON) {
        dump_data_json["schema"] = "qtrvsim.cli-report";
        dump_data_json["version"] = 1;
        QJsonObject termination = {
            { "reason", Machine::stop_reason_name(machine->stop_reason()) },
            { "exit_code", machine->exit_code() },
        };
        if (machine->stop_reason() == Machine::SR_EXCEPTION) {
            termination["exception"] = get_exception_name(machine->get_exception_cause());
        }
        dump_data_json["termination"] = termination;
    }

    if (dump_format & DumpFormat::CONSOLE) {
        if (e_regs || e_cache_stats || e_cycles || e_symtab || e_predictor || e_fail) {
            printf("Machine state report:\n");
        }
    }

    if (e_regs) { report_regs(); }
    if (e_cache_stats) { report_caches(); }
    if (e_cycles) {
        const uint64_t cycle_count = machine->core()->get_cycle_count();
        const uint64_t instruction_count = machine->core()->get_instruction_count();
        const uint64_t stall_count = machine->core()->get_stall_count();
        if (dump_format & DumpFormat::JSON) {
            QJsonObject temp = {};
            temp["cycles"] = json_number(cycle_count);
            temp["instructions"] = json_number(instruction_count);
            temp["stalls"] = json_number(stall_count);
            dump_data_json["cycles"] = temp;
        }
        if (dump_format & DumpFormat::CONSOLE) {
            printf("cycles: %" PRIu64 "\n", cycle_count);
            printf("instructions: %" PRIu64 "\n", instruction_count);
            printf("stalls: %" PRIu64 "\n", stall_count);
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
            if (file.write(bytes) != bytes.size()) {
                fprintf(
                    stderr, "Failed to write JSON report to %s\n",
                    qPrintable(dump_file_json));
                report_failed = true;
            }
            file.close();
            if (file.error() != QFile::NoError) {
                fprintf(
                    stderr, "Failed to close JSON report %s: %s\n", qPrintable(dump_file_json),
                    qPrintable(file.errorString()));
                report_failed = true;
            }
            if ((dump_format & DumpFormat::CONSOLE) && !report_failed) {
                printf("JSON object written to file\n");
            }
        } else {
            fprintf(
                stderr, "Could not open JSON report %s for writing: %s\n",
                qPrintable(dump_file_json), qPrintable(file.errorString()));
            report_failed = true;
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
    if (dump_format & DumpFormat::CONSOLE) { printf("Cache statistics report:\n"); }
    report_cache("i-cache", *machine->cache_program());
    report_cache("d-cache", *machine->cache_data());
    if (machine->config().cache_level2().enabled()) {
        report_cache("l2-cache", *machine->cache_level2());
    }
    if (machine->config().get_vm_enabled()) { report_tlbs(); }
}

void Reporter::report_cache(const char *cache_name, const Cache &cache) {
    if (dump_format & DumpFormat::JSON) {
        QJsonObject caches = dump_data_json["caches"].toObject();
        QJsonObject temp = {};
        temp["reads"] = static_cast<double>(cache.get_read_count());
        temp["hit"] = static_cast<double>(cache.get_hit_count());
        temp["miss"] = static_cast<double>(cache.get_miss_count());
        temp["hit_rate"] = cache.get_hit_rate();
        temp["stalled_cycles"] = static_cast<double>(cache.get_stall_count());
        temp["improved_speed"] = cache.get_speed_improvement();
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

void Reporter::report_tlbs() {
    if (dump_format & DumpFormat::JSON) { dump_data_json["tlbs"] = {}; }
    if (dump_format & DumpFormat::CONSOLE) { printf("TLB statistics report:\n"); }
    report_tlb("i-tlb", *machine->get_tlb_program());
    report_tlb("d-tlb", *machine->get_tlb_data());
}

void Reporter::report_tlb(const char *tlb_name, const TLB &tlb) {
    if (dump_format & DumpFormat::JSON) {
        QJsonObject tlbs = dump_data_json["tlbs"].toObject();
        QJsonObject temp = {};
        temp["reads"] = static_cast<double>(tlb.get_read_count());
        temp["writes"] = static_cast<double>(tlb.get_write_count());
        temp["hit"] = static_cast<double>(tlb.get_hit_count());
        temp["miss"] = static_cast<double>(tlb.get_miss_count());
        temp["hit_rate"] = tlb.get_hit_rate();
        temp["stalled_cycles"] = static_cast<double>(tlb.get_stall_count());
        temp["improved_speed"] = tlb.get_speed_improvement();
        tlbs[tlb_name] = temp;
        dump_data_json["tlbs"] = tlbs;
    }
    if (dump_format & DumpFormat::CONSOLE) {
        printf("%s:reads: %" PRIu32 "\n", tlb_name, tlb.get_read_count());
        printf("%s:writes: %" PRIu32 "\n", tlb_name, tlb.get_write_count());
        printf("%s:hit: %u\n", tlb_name, tlb.get_hit_count());
        printf("%s:miss: %u\n", tlb_name, tlb.get_miss_count());
        printf("%s:hit-rate: %.3lf\n", tlb_name, tlb.get_hit_rate());
        printf("%s:stalled-cycles: %" PRIu32 "\n", tlb_name, tlb.get_stall_count());
        printf("%s:improved-speed: %.3lf\n", tlb_name, tlb.get_speed_improvement());
    }
}

void Reporter::report_predictor() {
    const BranchPredictor *predictor = machine->branch_predictor();
    if (predictor == nullptr) { return; }
    const PredictionStatistics *stats = predictor->get_stats();
    if (stats == nullptr) { return; }

    if (dump_format & DumpFormat::JSON) {
        QJsonObject predictorObj = dump_data_json["predictor"].toObject();
        QJsonObject temp = {};
        temp["total_predictions"] = static_cast<double>(stats->total);
        temp["hits"] = static_cast<double>(stats->correct);
        temp["misses"] = static_cast<double>(stats->wrong);
        temp["accuracy"] = stats->accuracy;
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
        report_failed = true;
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
        report_failed = true;
    }
}

void Reporter::exit(int retcode) {
    // Exit might not happen immediately, so we need to stop the machine explicitly.
    machine->pause();
    QCoreApplication::exit(report_failed ? EXIT_FAILURE : retcode);
}
