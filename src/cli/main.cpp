#include "assembler/simpleasm.h"
#include "chariohandler.h"
#include "common/logging.h"
#include "common/logging_format_colors.h"
#include "machine/machineconfig.h"
#include "os_emulation/ossyscall.h"
#include "msgreport.h"
#include "reporter.h"
#include "tracer.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <cctype>
#include <fstream>
#include <iostream>

using namespace machine;
using namespace std;

using ae = machine::AccessEffects; // For enum values, type is obvious from
                                   // context.

void create_parser(QCommandLineParser &p) {
    p.setApplicationDescription("QtRvSim CLI machine simulator");
    p.addHelpOption();
    p.addVersionOption();

    p.addPositionalArgument("FILE", "Input ELF executable file or assembler source");

    // p.addOptions({}); available only from Qt 5.4+
    p.addOption({ "asm", "Treat provided file argument as assembler source." });
    p.addOption({ "pipelined", "Configure CPU to use five stage pipeline." });
    p.addOption({ "no-delay-slot", "Disable jump delay slot." });
    p.addOption(
        { "hazard-unit", "Specify hazard unit implementation [none|stall|forward].", "HUKIND" });
    p.addOption({ { "trace-fetch", "tr-fetch" },
                  "Trace fetched instruction (for both pipelined and not core)." });
    p.addOption({ { "trace-decode", "tr-decode" },
                  "Trace instruction in decode stage. (only for pipelined core)" });
    p.addOption({ { "trace-execute", "tr-execute" },
                  "Trace instruction in execute stage. (only for pipelined core)" });
    p.addOption({ { "trace-memory", "tr-memory" },
                  "Trace instruction in memory stage. (only for pipelined core)" });
    p.addOption({ { "trace-writeback", "tr-writeback" },
                  "Trace instruction in write back stage. (only for pipelined core)" });
    p.addOption({ { "trace-pc", "tr-pc" }, "Print program counter register changes." });
    p.addOption({ { "trace-wrmem", "tr-wr" }, "Trace writes into memory." });
    p.addOption({ { "trace-rdmem", "tr-rd" }, "Trace reads from memory." });
    p.addOption({ { "trace-gp", "tr-gp" },
                  "Print general purpose register changes. You can use * for "
                  "all registers.",
                  "REG" });
    p.addOption({ "dump-to-json", "Configure reportor dump to json file.", "FNAME" });
    p.addOption({ { "dump-registers", "d-regs" }, "Dump registers state at program exit." });
    p.addOption({ "dump-cache-stats", "Dump cache statistics at program exit." });
    p.addOption({ "dump-cycles", "Dump number of CPU cycles till program end." });
    p.addOption({ "dump-range", "Dump memory range.", "START,LENGTH,FNAME" });
    p.addOption({ "load-range", "Load memory range.", "START,FNAME" });
    p.addOption({ "expect-fail", "Expect that program causes CPU trap and fail if it doesn't." });
    p.addOption({ "fail-match",
                  "Program should exit with exactly this CPU TRAP. Possible values are "
                  "I(unsupported Instruction), A(Unsupported ALU operation), "
                  "O(Overflow/underflow) and J(Unaligned Jump). You can freely combine "
                  "them. Using this implies expect-fail option.",
                  "TRAP" });
    p.addOption({ "d-cache",
                  "Data cache. Format policy,sets,words_in_blocks,associativity where "
                  "policy is random/lru/lfu",
                  "DCACHE" });
    p.addOption({ "i-cache",
                  "Instruction cache. Format policy,sets,words_in_blocks,associativity "
                  "where policy is random/lru/lfu",
                  "ICACHE" });
    p.addOption({ "l2-cache",
                  "L2 cache. Format policy,sets,words_in_blocks,associativity where "
                  "policy is random/lru/lfu",
                  "L2CACHE" });
    p.addOption({ "read-time", "Memory read access time (cycles).", "RTIME" });
    p.addOption({ "write-time", "Memory read access time (cycles).", "WTIME" });
    p.addOption({ "burst-time", "Memory read access time (cycles).", "BTIME" });
    p.addOption({ { "serial-in", "serin" }, "File connected to the serial port input.", "FNAME" });
    p.addOption(
        { { "serial-out", "serout" }, "File connected to the serial port output.", "FNAME" });
    p.addOption({ { "os-emulation", "osemu" }, "Operating system emulation." });
    p.addOption({ { "std-out", "stdout" }, "File connected to the syscall standard output.", "FNAME" });
    p.addOption({ { "os-fs-root", "osfsroot" }, "Emulated system root/prefix for opened files", "DIR" });
    p.addOption({ { "isa-variant", "isavariant" }, "Instruction set to emulate (default RV32IMA)", "STR" });
    p.addOption({ "cycle-limit", "Limit execution to specified maximum clock cycles", "NUMBER" });
}

void configure_cache(CacheConfig &cacheconf, const QStringList &cachearg, const QString &which) {
    if (cachearg.empty()) { return; }
    cacheconf.set_enabled(true);
    QStringList pieces = cachearg.at(cachearg.size() - 1).split(",");
    if (pieces.size() < 3) {
        fprintf(
            stderr, "Parameters %s cache incorrect (correct lru,4,2,2,wb).\n", qPrintable(which));
        exit(EXIT_FAILURE);
    }
    if (pieces.at(0).size() < 1) {
        fprintf(stderr, "Policy for %s cache is incorrect.\n", qPrintable(which));
        exit(EXIT_FAILURE);
    }
    if (!pieces.at(0).at(0).isDigit()) {
        if (pieces.at(0).toLower() == "random") {
            cacheconf.set_replacement_policy(CacheConfig::RP_RAND);
        } else if (pieces.at(0).toLower() == "lru") {
            cacheconf.set_replacement_policy(CacheConfig::RP_LRU);
        } else if (pieces.at(0).toLower() == "lfu") {
            cacheconf.set_replacement_policy(CacheConfig::RP_LFU);
        } else {
            fprintf(stderr, "Policy for %s cache is incorrect.\n", qPrintable(which));
            exit(EXIT_FAILURE);
        }
        pieces.removeFirst();
    }
    if (pieces.size() < 3) {
        fprintf(
            stderr, "Parameters for  %s  cache incorrect (correct lru,4,2,2,wb). \n",
            qPrintable(which));
        exit(EXIT_FAILURE);
    }
    cacheconf.set_set_count(pieces.at(0).toLong());
    cacheconf.set_block_size(pieces.at(1).toLong());
    cacheconf.set_associativity(pieces.at(2).toLong());
    if (cacheconf.set_count() == 0 || cacheconf.block_size() == 0
        || cacheconf.associativity() == 0) {
        fprintf(
            stderr, "Parameters for  %s  cache cannot have zero component. \n", qPrintable(which));
        exit(EXIT_FAILURE);
    }
    if (pieces.size() > 3) {
        if (pieces.at(3).toLower() == "wb") {
            cacheconf.set_write_policy(CacheConfig::WP_BACK);
        } else if (pieces.at(3).toLower() == "wt" || pieces.at(3).toLower() == "wtna") {
            cacheconf.set_write_policy(CacheConfig::WP_THROUGH_NOALLOC);
        } else if (pieces.at(3).toLower() == "wta") {
            cacheconf.set_write_policy(CacheConfig::WP_THROUGH_ALLOC);
        } else {
            fprintf(
                stderr, "Write policy for  %s  cache is incorrect (correct wb/wt/wtna/wta). \n",
                qPrintable(which));
            exit(EXIT_FAILURE);
        }
    }
}

void parse_u32_option(
    QCommandLineParser &parser,
    const QString &option_name,
    MachineConfig &config,
    void (MachineConfig::*setter)(uint32_t value)) {
    auto values = parser.values(option_name);
    if (!values.empty()) {
        bool ok = true;
        // Try to parse supplied value.
        uint32_t value = values.last().toUInt(&ok);
        if (ok) {
            // Set the value if successfully parsed.
            (config.*setter)(value);
        } else {
            fprintf(
                stderr, "Value of option %s is not a valid unsigned integer.",
                qPrintable(option_name));
            exit(EXIT_FAILURE);
        }
    }
}

void configure_machine(QCommandLineParser &parser, MachineConfig &config) {
    QStringList arguments = parser.positionalArguments();
    if (arguments.size() != 1) {
        fprintf(stderr, "Single ELF file has to be specified\n");
        parser.showHelp();
    }
    config.set_elf(arguments[0]);

    config.set_delay_slot(!parser.isSet("no-delay-slot"));
    config.set_pipelined(parser.isSet("pipelined"));

    auto hazard_unit_values = parser.values("hazard-unit");
    if (!hazard_unit_values.empty()) {
        if (!config.set_hazard_unit(hazard_unit_values.last().toLower())) {
            fprintf(stderr, "Unknown kind of hazard unit specified\n");
            exit(EXIT_FAILURE);
        }
    }

    parse_u32_option(parser, "read-time", config, &MachineConfig::set_memory_access_time_read);
    parse_u32_option(parser, "write-time", config, &MachineConfig::set_memory_access_time_write);
    parse_u32_option(parser, "burst-time", config, &MachineConfig::set_memory_access_time_burst);
    if (!parser.values("burst-time").empty()) config.set_memory_access_enable_burst(true);

    configure_cache(*config.access_cache_data(), parser.values("d-cache"), "data");
    configure_cache(*config.access_cache_program(), parser.values("i-cache"), "instruction");
    configure_cache(*config.access_cache_level2(), parser.values("l2-cache"), "level2");

    config.set_osemu_enable(parser.isSet("os-emulation"));
    config.set_osemu_known_syscall_stop(false);

    int siz = parser.values("os-fs-root").size();
    if (siz >= 1) {
        QString osemu_fs_root = parser.values("os-fs-root").at(siz - 1);
        if (osemu_fs_root.length() > 0)
            config.set_osemu_fs_root(osemu_fs_root);
    }
    siz = parser.values("isa-variant").size();
    for (int i = 0; i < siz; i++) {
        int pos = 0;
        bool first = true;
        bool subtract = false;
        QString isa_str = parser.values("isa-variant").at(i).toUpper();
        if (isa_str.startsWith ("RV32")) {
            config.set_simulated_xlen(machine::Xlen::_32);
            pos = 4;
        } else if (isa_str.startsWith ("RV64")) {
            config.set_simulated_xlen(machine::Xlen::_64);
            pos = 4;
        }
        for (; pos < isa_str.size(); pos++, first = false) {
            char ch = isa_str.at(pos).toLatin1();
            if (ch == '+') {
                subtract = false;
                continue;
            } else if (ch == '-') {
                subtract = true;
                continue;
            }
            auto flag = machine::ConfigIsaWord::byChar(ch);
            if (flag.isEmpty())
                continue;
            if (first)
                config.modify_isa_word(~machine::ConfigIsaWord::empty(), machine::ConfigIsaWord::empty());
            if (subtract)
                config.modify_isa_word(flag, machine::ConfigIsaWord::empty());
            else
                config.modify_isa_word(flag, flag);
        }
    }
}

void configure_tracer(QCommandLineParser &p, Tracer &tr) {
    if (p.isSet("trace-fetch")) { tr.trace_fetch = true; }
    if (p.isSet("pipelined")) { // Following are added only if we have stages
        if (p.isSet("trace-decode")) { tr.trace_decode = true; }
        if (p.isSet("trace-execute")) { tr.trace_execute = true; }
        if (p.isSet("trace-memory")) { tr.trace_memory = true; }
        if (p.isSet("trace-writeback")) { tr.trace_writeback = true; }
    }

    if (p.isSet("trace-pc")) { tr.trace_pc = true; }
    if (p.isSet("trace-gp")) { tr.trace_regs_gp = true; }

    QStringList gps = p.values("trace-gp");
    for (const auto & gp : gps) {
        if (gp == "*") {
            tr.regs_to_trace.fill(true);
        } else {
            bool res;
            size_t num = gp.toInt(&res);
            if (res && num <= machine::REGISTER_COUNT) {
                tr.regs_to_trace.at(num) = true;
            } else {
                fprintf(
                    stderr, "Unknown register number given for trace-gp: %s\n", qPrintable(gp));
                exit(EXIT_FAILURE);
            }
        }
    }

    if (p.isSet("trace-rdmem")) { tr.trace_rdmem = true; }
    if (p.isSet("trace-wrmem")) { tr.trace_wrmem = true; }

    QStringList clim = p.values("cycle-limit");
    if (!clim.empty()) {
        bool ok;
        tr.cycle_limit = clim.at(clim.size() - 1).toLong(&ok);
        if (!ok) {
            fprintf(
                stderr, "Cycle limit parse error\n");
            exit(EXIT_FAILURE);
        }
    }

    // TODO
}

void configure_reporter(QCommandLineParser &p, Reporter &r, const SymbolTable *symtab) {
    if (p.isSet("dump-to-json")) {
        r.dump_format = (DumpFormat)(r.dump_format | DumpFormat::JSON);
        r.dump_file_json = p.value("dump-to-json");
    }
    if (p.isSet("dump-registers")) { r.enable_regs_reporting(); }
    if (p.isSet("dump-cache-stats")) { r.enable_cache_stats(); }
    if (p.isSet("dump-cycles")) { r.enable_cycles_reporting(); }

    QStringList fail = p.values("fail-match");
    for (const auto & i : fail) {
        for (int y = 0; y < i.length(); y++) {
            enum Reporter::FailReason reason;
            switch (tolower(i.toStdString()[y])) {
            case 'i': reason = Reporter::FR_UNSUPPORTED_INSTR; break;
            default:
                fprintf(stderr, "Unknown fail condition: %c\n", qPrintable(i)[y]);
                exit(EXIT_FAILURE);
            }
            r.expect_fail(reason);
        }
    }
    if (p.isSet("expect-fail") && !p.isSet("fail-match")) { r.expect_fail(Reporter::FailAny); }

    foreach (QString range_arg, p.values("dump-range")) {
        uint64_t len;
        bool ok1 = true;
        bool ok2 = true;
        QString str;
        int comma1 = range_arg.indexOf(",");
        if (comma1 < 0) {
            fprintf(stderr, "Range start missing\n");
            exit(EXIT_FAILURE);
        }
        int comma2 = range_arg.indexOf(",", comma1 + 1);
        if (comma2 < 0) {
            fprintf(stderr, "Range length/name missing\n");
            exit(EXIT_FAILURE);
        }
        str = range_arg.mid(0, comma1);
        Address start;
        if (str.size() >= 1 && !str.at(0).isDigit() && symtab != nullptr) {
            SymbolValue _start;
            ok1 = symtab->name_to_value(_start, str);
            start = Address(_start);
        } else {
            start = Address(str.toULong(&ok1, 0));
        }
        str = range_arg.mid(comma1 + 1, comma2 - comma1 - 1);
        if (str.size() >= 1 && !str.at(0).isDigit() && symtab != nullptr) {
            ok2 = symtab->name_to_value(len, str);
        } else {
            len = str.toULong(&ok2, 0);
        }
        if (!ok1 || !ok2) {
            fprintf(stderr, "Range start/length specification error.\n");
            exit(EXIT_FAILURE);
        }
        r.add_dump_range(start, len, range_arg.mid(comma2 + 1));
    }

    // TODO
}

void configure_serial_port(QCommandLineParser &p, SerialPort *ser_port) {
    CharIOHandler *ser_in = nullptr;
    CharIOHandler *ser_out = nullptr;
    int siz;

    if (!ser_port) { return; }

    siz = p.values("serial-in").size();
    if (siz >= 1) {
        QIODevice::OpenMode mode = QFile::ReadOnly;
        auto *qf = new QFile(p.values("serial-in").at(siz - 1));
        ser_in = new CharIOHandler(qf, ser_port);
        siz = p.values("serial-out").size();
        if (siz) {
            if (p.values("serial-in").at(siz - 1) == p.values("serial-out").at(siz - 1)) {
                mode = QFile::ReadWrite;
                ser_out = ser_in;
            }
        }
        if (!ser_in->open(mode)) {
            fprintf(stderr, "Serial port input file cannot be open for read.\n");
            exit(EXIT_FAILURE);
        }
    }

    if (!ser_out) {
        siz = p.values("serial-out").size();
        if (siz >= 1) {
            auto *qf = new QFile(p.values("serial-out").at(siz - 1));
            ser_out = new CharIOHandler(qf, ser_port);
            if (!ser_out->open(QFile::WriteOnly)) {
                fprintf(stderr, "Serial port output file cannot be open for write.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (ser_in) {
        QObject::connect(ser_in, &QIODevice::readyRead, ser_port, &SerialPort::rx_queue_check);
        QObject::connect(ser_port, &SerialPort::rx_byte_pool, ser_in, &CharIOHandler::readBytePoll);
        if (ser_in->bytesAvailable()) { ser_port->rx_queue_check(); }
    }

    if (ser_out) {
        QObject::connect(
            ser_port, &SerialPort::tx_byte, ser_out,
            QOverload<unsigned>::of(&CharIOHandler::writeByte));
    }
}

void configure_osemu(QCommandLineParser &p, MachineConfig &config, Machine *machine) {
    CharIOHandler *std_out = nullptr;
    int siz;

    siz = p.values("std-out").size();
    if (siz >= 1) {
        auto *qf = new QFile(p.values("std-out").at(siz - 1));
        std_out = new CharIOHandler(qf, machine);
        if (!std_out->open(QFile::WriteOnly)) {
            fprintf(stderr, "Emulated system standard output file cannot be open for write.\n");
            exit(EXIT_FAILURE);
        }
    }
    const static machine::ExceptionCause ecall_variats[] = {machine::EXCAUSE_ECALL_ANY,
        machine::EXCAUSE_ECALL_M, machine::EXCAUSE_ECALL_S, machine::EXCAUSE_ECALL_U};

    if (config.osemu_enable()) {
        auto *osemu_handler = new osemu::OsSyscallExceptionHandler(
            config.osemu_known_syscall_stop(), config.osemu_unknown_syscall_stop(),
            config.osemu_fs_root());
        if (std_out) {
            machine::Machine::connect(
                osemu_handler, &osemu::OsSyscallExceptionHandler::char_written,
                std_out, QOverload<int, unsigned>::of(&CharIOHandler::writeByte));
        }
        /*connect(
            osemu_handler, &osemu::OsSyscallExceptionHandler::rx_byte_pool, terminal,
            &TerminalDock::rx_byte_pool);*/
        for (auto ecall_variat : ecall_variats) {
            machine->register_exception_handler(ecall_variat, osemu_handler);
            machine->set_step_over_exception(ecall_variat, true);
            machine->set_stop_on_exception(ecall_variat, false);
        }
    } else {
        for (auto ecall_variat : ecall_variats) {
            machine->set_step_over_exception(ecall_variat, false);
            machine->set_stop_on_exception(ecall_variat, config.osemu_exception_stop());
        }
    }
}

void load_ranges(Machine &machine, const QStringList &ranges) {
    for (const QString &range_arg : ranges) {
        bool ok = true;
        QString str;
        int comma1 = range_arg.indexOf(",");
        if (comma1 < 0) {
            fprintf(stderr, "Range start missing\n");
            exit(EXIT_FAILURE);
        }
        str = range_arg.mid(0, comma1);
        Address start;
        if (str.size() >= 1 && !str.at(0).isDigit() && machine.symbol_table() != nullptr) {
            SymbolValue _start;
            ok = machine.symbol_table()->name_to_value(_start, str);
            start = Address(_start);
        } else {
            start = Address(str.toULong(&ok, 0));
        }
        if (!ok) {
            fprintf(stderr, "Range start/length specification error.\n");
            exit(EXIT_FAILURE);
        }
        ifstream in;
        in.open(range_arg.mid(comma1 + 1).toLocal8Bit().data(), ios::in);
        Address addr = start;
        for (std::string line; getline(in, line);) {
            size_t end_pos = line.find_last_not_of(" \t\n");
            if (std::string::npos == end_pos) {
                continue;
            }

            size_t start_pos = line.find_first_not_of(" \t\n");
            line = line.substr(0, end_pos + 1);
            line = line.substr(start_pos);

            size_t idx;
            uint32_t val = stoul(line, &idx, 0);
            if (idx != line.size()) {
                fprintf(stderr, "cannot parse load range data.\n");
                exit(EXIT_FAILURE);
            }
            machine.memory_data_bus_rw()->write_u32(addr, val, ae::INTERNAL);
            addr += 4;
        }
        in.close();
    }
}

bool assemble(Machine &machine, MsgReport &msgrep, const QString &filename) {
    SymbolTableDb symbol_table_db(machine.symbol_table_rw(true));
    machine::FrontendMemory *mem = machine.memory_data_bus_rw();
    if (mem == nullptr) { return false; }
    machine.cache_sync();
    SimpleAsm assembler;

    SimpleAsm::connect(&assembler, &SimpleAsm::report_message, &msgrep, &MsgReport::report_message);

    assembler.setup(mem, &symbol_table_db, 0x00000200_addr, machine.core()->get_xlen());

    if (!assembler.process_file(filename)) {
        return false;
    }

    return assembler.finish();
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setApplicationVersion(APP_VERSION);
    set_default_log_pattern();

    QCommandLineParser p;
    create_parser(p);
    p.process(app);

    MachineConfig config;
    configure_machine(p, config);

    bool asm_source = p.isSet("asm");
    Machine machine(config, !asm_source, !asm_source);

    Tracer tr(&machine);
    configure_tracer(p, tr);

    configure_serial_port(p, machine.serial_port());

    configure_osemu(p, config, &machine);

    if (asm_source) {
        MsgReport msg_report(&app);
        if (!assemble(machine, msg_report, p.positionalArguments()[0])) { exit(EXIT_FAILURE); }
    }

    Reporter r(&app, &machine);
    configure_reporter(p, r, machine.symbol_table());

    QObject::connect(&tr, &Tracer::cycle_limit_reached, &r, &Reporter::cycle_limit_reached);

    load_ranges(machine, p.values("load-range"));

    machine.play();
    return QCoreApplication::exec();
}
