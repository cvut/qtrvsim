#include "assembler/simpleasm.h"
#include "chariohandler.h"
#include "common/logging.h"
#include "common/logging_format_colors.h"
#include "machine/machineconfig.h"
#include "msgreport.h"
#include "reporter.h"
#include "tracer.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <cctype>
#include <fstream>
#include <iostream>
#include <utility>

using namespace machine;
using namespace std;

using ae = machine::AccessEffects; // For enum values, type is obvious from
                                   // context.

void create_parser(QCommandLineParser &p) {
    p.setApplicationDescription("QtMips CLI machine simulator");
    p.addHelpOption();
    p.addVersionOption();

    p.addPositionalArgument(
        "FILE", "Input ELF executable file or assembler source");

    // p.addOptions({}); available only from Qt 5.4+
    p.addOption({ "asm", "Treat provided file argument as assembler source." });
    p.addOption({ "pipelined", "Configure CPU to use five stage pipeline." });
    p.addOption({ "no-delay-slot", "Disable jump delay slot." });
    p.addOption(
        { "hazard-unit", "Specify hazard unit implementation [none|stall|forward].", "HUKIND" });
    p.addOption(
        { { "trace-fetch", "tr-fetch" },
          "Trace fetched instruction (for both pipelined and not core)." });
    p.addOption(
        { { "trace-decode", "tr-decode" },
          "Trace instruction in decode stage. (only for pipelined core)" });
    p.addOption(
        { { "trace-execute", "tr-execute" },
          "Trace instruction in execute stage. (only for pipelined core)" });
    p.addOption(
        { { "trace-memory", "tr-memory" },
          "Trace instruction in memory stage. (only for pipelined core)" });
    p.addOption(
        { { "trace-writeback", "tr-writeback" },
          "Trace instruction in write back stage. (only for pipelined core)" });
    p.addOption(
        { { "trace-pc", "tr-pc" }, "Print program counter register changes." });
    p.addOption({ { "trace-gp", "tr-gp" },
                  "Print general purpose register changes. You can use * for "
                  "all registers.",
                  "REG" });
    p.addOption({ { "trace-lo", "tr-lo" }, "Print LO register changes." });
    p.addOption({ { "trace-hi", "tr-hi" }, "Print HI register changes." });
    p.addOption({ { "dump-registers", "d-regs" },
                  "Dump registers state at program exit." });
    p.addOption(
        { "dump-cache-stats", "Dump cache statistics at program exit." });
    p.addOption(
        { "dump-cycles", "Dump number of CPU cycles till program end." });
    p.addOption({ "dump-range", "Dump memory range.", "START,LENGTH,FNAME" });
    p.addOption({ "load-range", "Load memory range.", "START,FNAME" });
    p.addOption(
        { "expect-fail",
          "Expect that program causes CPU trap and fail if it doesn't." });
    p.addOption(
        { "fail-match",
          "Program should exit with exactly this CPU TRAP. Possible values are "
          "I(unsupported Instruction), A(Unsupported ALU operation), "
          "O(Overflow/underflow) and J(Unaligned Jump). You can freely combine "
          "them. Using this implies expect-fail option.",
          "TRAP" });
    p.addOption(
        { "d-cache",
          "Data cache. Format policy,sets,words_in_blocks,associativity where "
          "policy is random/lru/lfu",
          "DCACHE" });
    p.addOption(
        { "i-cache",
          "Instruction cache. Format policy,sets,words_in_blocks,associativity "
          "where policy is random/lru/lfu",
          "ICACHE" });
    p.addOption({ "read-time", "Memory read access time (cycles).", "RTIME" });
    p.addOption({ "write-time", "Memory read access time (cycles).", "WTIME" });
    p.addOption({ "burst-time", "Memory read access time (cycles).", "BTIME" });
    p.addOption({ { "serial-in", "serin" },
                  "File connected to the serial port input.",
                  "FNAME" });
    p.addOption({ { "serial-out", "serout" },
                  "File connected to the serial port output.",
                  "FNAME" });
}

void configure_cache(
    CacheConfig &cacheconf,
    const QStringList &cachearg,
    const QString &which) {
    if (cachearg.empty()) {
        return;
    }
    cacheconf.set_enabled(true);
    QStringList pieces = cachearg.at(cachearg.size() - 1).split(",");
    if (pieces.size() < 3) {
        std::cerr << "Parameters for " << which.toLocal8Bit().data()
                  << " cache incorrect (correct lru,4,2,2,wb)." << std::endl;
        exit(1);
    }
    if (pieces.at(0).size() < 1) {
        std::cerr << "Policy for " << which.toLocal8Bit().data()
                  << " cache is incorrect." << std::endl;
        exit(1);
    }
    if (!pieces.at(0).at(0).isDigit()) {
        if (pieces.at(0).toLower() == "random") {
            cacheconf.set_replacement_policy(CacheConfig::RP_RAND);
        } else if (pieces.at(0).toLower() == "lru") {
            cacheconf.set_replacement_policy(CacheConfig::RP_LRU);
        } else if (pieces.at(0).toLower() == "lfu") {
            cacheconf.set_replacement_policy(CacheConfig::RP_LFU);
        } else {
            std::cerr << "Policy for " << which.toLocal8Bit().data()
                      << " cache is incorrect." << std::endl;
            exit(1);
        }
        pieces.removeFirst();
    }
    if (pieces.size() < 3) {
        std::cerr << "Parameters for " << which.toLocal8Bit().data()
                  << " cache incorrect (correct lru,4,2,2,wb)." << std::endl;
        exit(1);
    }
    cacheconf.set_set_count(pieces.at(0).toLong());
    cacheconf.set_block_size(pieces.at(1).toLong());
    cacheconf.set_associativity(pieces.at(2).toLong());
    if (cacheconf.set_count() == 0 || cacheconf.block_size() == 0
        || cacheconf.associativity() == 0) {
        std::cerr << "Parameters for " << which.toLocal8Bit().data()
                  << " cache cannot have zero component." << std::endl;
        exit(1);
    }
    if (pieces.size() > 3) {
        if (pieces.at(3).toLower() == "wb") {
            cacheconf.set_write_policy(CacheConfig::WP_BACK);
        } else if (
            pieces.at(3).toLower() == "wt"
            || pieces.at(3).toLower() == "wtna") {
            cacheconf.set_write_policy(CacheConfig::WP_THROUGH_NOALLOC);
        } else if (pieces.at(3).toLower() == "wta") {
            cacheconf.set_write_policy(CacheConfig::WP_THROUGH_ALLOC);
        } else {
            std::cerr << "Write policy for " << which.toLocal8Bit().data()
                      << " cache is incorrect (correct wb/wt/wtna/wta)."
                      << std::endl;
            exit(1);
        }
    }
}

void configure_machine(QCommandLineParser &p, MachineConfig &cc) {
    QStringList pa = p.positionalArguments();
    int siz;
    if (pa.size() != 1) {
        std::cerr << "Single ELF file has to be specified" << std::endl;
        exit(1);
    }
    cc.set_elf(pa[0]);

    cc.set_delay_slot(!p.isSet("no-delay-slot"));
    cc.set_pipelined(p.isSet("pipelined"));

    siz = p.values("hazard-unit").size();
    if (siz >= 1) {
        QString hukind = p.values("hazard-unit").at(siz - 1).toLower();
        if (!cc.set_hazard_unit(hukind)) {
            std::cerr << "Unknown kind of hazard unit specified" << std::endl;
            exit(1);
        }
    }

    siz = p.values("read-time").size();
    if (siz >= 1) {
        cc.set_memory_access_time_read(
            p.values("read-time").at(siz - 1).toLong());
    }
    siz = p.values("write-time").size();
    if (siz >= 1) {
        cc.set_memory_access_time_write(
            p.values("write-time").at(siz - 1).toLong());
    }
    siz = p.values("burst-time").size();
    if (siz >= 1) {
        cc.set_memory_access_time_burst(
            p.values("burst-time").at(siz - 1).toLong());
    }

    configure_cache(*cc.access_cache_data(), p.values("d-cache"), "data");
    configure_cache(
        *cc.access_cache_program(), p.values("i-cache"), "instruction");
}

void configure_tracer(QCommandLineParser &p, Tracer &tr) {
    if (p.isSet("trace-fetch")) {
        tr.trace_fetch = true;
    }
    if (p.isSet("pipelined")) { // Following are added only if we have stages
        if (p.isSet("trace-decode")) {
            tr.trace_decode = true;
        }
        if (p.isSet("trace-execute")) {
            tr.trace_fetch = true;
        }
        if (p.isSet("trace-memory")) {
            tr.trace_memory = true;
        }
        if (p.isSet("trace-writeback")) {
            tr.trace_writeback = true;
        }
    }

    if (p.isSet("trace-pc")) {
        tr.trace_regs_gp = true;
    }

    QStringList gps = p.values("trace-gp");
    for (int i = 0; i < gps.size(); i++) {
        if (gps[i] == "*") {
            tr.regs_to_trace.fill(true);
        } else {
            bool res;
            size_t num = gps[i].toInt(&res);
            if (res && num <= machine::REGISTER_COUNT) {
                tr.regs_to_trace.at(num) = true;
            } else {
                cout << "Unknown register number given for trace-gp: "
                     << gps[i].toStdString() << endl;
                exit(1);
            }
        }
    }

    // TODO
}

void configure_reporter(
    QCommandLineParser &p,
    Reporter &r,
    const SymbolTable *symtab) {
    if (p.isSet("dump-registers")) {
        r.regs();
    }
    if (p.isSet("dump-cache-stats")) {
        r.cache_stats();
    }
    if (p.isSet("dump-cycles")) {
        r.cycles();
    }

    QStringList fail = p.values("fail-match");
    for (int i = 0; i < fail.size(); i++) {
        for (int y = 0; y < fail[i].length(); y++) {
            enum Reporter::FailReason reason;
            switch (tolower(fail[i].toStdString()[y])) {
            case 'i': reason = Reporter::FR_UNSUPPORTED_INSTR; break;
            default:
                cout << "Unknown fail condition: " << fail[i].toStdString()[y] << endl;
                exit(1);
            }
            r.expect_fail(reason);
        }
    }
    if (p.isSet("expect-fail") && !p.isSet("fail-match")) {
        r.expect_fail(Reporter::FailAny);
    }

    foreach (QString range_arg, p.values("dump-range")) {
        uint64_t len;
        bool ok1 = true;
        bool ok2 = true;
        QString str;
        int comma1 = range_arg.indexOf(",");
        if (comma1 < 0) {
            cout << "Range start missing" << endl;
            exit(1);
        }
        int comma2 = range_arg.indexOf(",", comma1 + 1);
        if (comma2 < 0) {
            cout << "Range lengt/name missing" << endl;
            exit(1);
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
            cout << "Range start/length specification error." << endl;
            exit(1);
        }
        r.add_dump_range(start, len, range_arg.mid(comma2 + 1));
    }

    // TODO
}

void configure_serial_port(QCommandLineParser &p, SerialPort *ser_port) {
    int siz;
    CharIOHandler *ser_in = nullptr;
    CharIOHandler *ser_out = nullptr;

    if (!ser_port) {
        return;
    }

    siz = p.values("serial-in").size();
    if (siz >= 1) {
        QIODevice::OpenMode mode = QFile::ReadOnly;
        auto *qf = new QFile(p.values("serial-in").at(siz - 1));
        ser_in = new CharIOHandler(qf, ser_port);
        siz = p.values("serial-out").size();
        if (siz) {
            if (p.values("serial-in").at(siz - 1)
                == p.values("serial-out").at(siz - 1)) {
                mode = QFile::ReadWrite;
                ser_out = ser_in;
            }
        }
        if (!ser_in->open(mode)) {
            cout << "Serial port input file cannot be open for read." << endl;
            exit(1);
        }
    }

    if (!ser_out) {
        siz = p.values("serial-out").size();
        if (siz >= 1) {
            auto *qf = new QFile(p.values("serial-out").at(siz - 1));
            ser_out = new CharIOHandler(qf, ser_port);
            if (!ser_out->open(QFile::WriteOnly)) {
                cout << "Serial port output file cannot be open for write."
                     << endl;
                exit(1);
            }
        }
    }

    if (ser_in) {
        QObject::connect(
            ser_in, &QIODevice::readyRead, ser_port,
            &SerialPort::rx_queue_check);
        QObject::connect(
            ser_port, &SerialPort::rx_byte_pool, ser_in,
            &CharIOHandler::readBytePoll);
        if (ser_in->bytesAvailable()) {
            ser_port->rx_queue_check();
        }
    }

    if (ser_out) {
        QObject::connect(
            ser_port, &SerialPort::tx_byte, ser_out,
            QOverload<unsigned>::of(&CharIOHandler::writeByte));
    }
}

void load_ranges(Machine &machine, const QStringList &ranges) {
    foreach (QString range_arg, ranges) {
        bool ok = true;
        QString str;
        int comma1 = range_arg.indexOf(",");
        if (comma1 < 0) {
            cout << "Range start missing" << endl;
            exit(1);
        }
        str = range_arg.mid(0, comma1);
        Address start;
        if (str.size() >= 1 && !str.at(0).isDigit()
            && machine.symbol_table() != nullptr) {
            SymbolValue _start;
            ok = machine.symbol_table()->name_to_value(_start, str);
            start = Address(_start);
        } else {
            start = Address(str.toULong(&ok, 0));
        }
        if (!ok) {
            cout << "Range start/length specification error." << endl;
            exit(1);
        }
        ifstream in;
        in.open(range_arg.mid(comma1 + 1).toLocal8Bit().data(), ios::in);
        Address addr = start;
        for (std::string line; getline(in, line);) {
            size_t endpos = line.find_last_not_of(" \t\n");
            size_t startpos = line.find_first_not_of(" \t\n");
            size_t idx;
            if (std::string::npos == endpos) {
                continue;
            }
            line = line.substr(0, endpos + 1);
            line = line.substr(startpos);
            uint32_t val = stoul(line, &idx, 0);
            if (idx != line.size()) {
                cout << "cannot parse load range data." << endl;
                exit(1);
            }
            machine.memory_data_bus_rw()->write_u32(addr, val, ae::INTERNAL);
            addr += 4;
        }
        in.close();
    }
}

bool assemble(Machine &machine, MsgReport &msgrep, QString filename) {
    SymbolTableDb symtab(machine.symbol_table_rw(true));
    machine::FrontendMemory *mem = machine.memory_data_bus_rw();
    if (mem == nullptr) {
        return false;
    }
    machine.cache_sync();
    SimpleAsm sasm;

    SimpleAsm::connect(
        &sasm, &SimpleAsm::report_message, &msgrep, &MsgReport::report_message);

    sasm.setup(mem, &symtab, 0x80020000_addr);

    if (!sasm.process_file(filename)) {
        return false;
    }

    return sasm.finish();
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setApplicationVersion(APP_VERSION);
    set_default_log_pattern();

    QCommandLineParser p;
    create_parser(p);
    p.process(app);

    bool asm_source = p.isSet("asm");

    MachineConfig cc;
    configure_machine(p, cc);
    Machine machine(cc, !asm_source, !asm_source);

    Tracer tr(&machine);
    configure_tracer(p, tr);

    Reporter r(&app, &machine);
    configure_reporter(p, r, machine.symbol_table());

    configure_serial_port(p, machine.serial_port());

    if (asm_source) {
        MsgReport msgrep(&app);
        if (!assemble(machine, msgrep, p.positionalArguments()[0])) {
            exit(1);
        }
    }

    load_ranges(machine, p.values("load-range"));

    machine.play();
    return QCoreApplication::exec();
}
