#ifndef CLIMAIN_H
#define CLIMAIN_H

#include "machine/machineconfig.h"
#include "msgreport.h"
#include "reporter.h"
#include "tracer.h"

#include <QObject>
#include <QCommandLineParser>

class CLIMain final : public QObject {
    Q_OBJECT

    using MachineConfig = machine::MachineConfig;
    using CacheConfig = machine::CacheConfig;

public:
    CLIMain(QCoreApplication& app);
    ~CLIMain();

public slots:
    void start();

private:
    QCoreApplication& app;
    QCommandLineParser parser;
    machine::MachineConfig config;
    machine::Machine* machine = nullptr;
    Tracer* tracer = nullptr;
    Reporter* reporter = nullptr;

    void create_parser();
    void configure_cache(CacheConfig &cacheconf, const QStringList &cachearg, const QString &which);
    void parse_u32_option(const QString &option_name, void (machine::MachineConfig::*setter)(uint32_t value));
    void configure_machine();
    void configure_tracer(Tracer &tr);
    void configure_reporter(Reporter &r, const machine::SymbolTable *symtab);
    void configure_serial_port(machine::SerialPort *ser_port);
    void configure_osemu(machine::Machine *machine);
    void load_ranges(machine::Machine &machine, const QStringList &ranges);
    bool assemble(machine::Machine &machine, MsgReport &msgrep, const QString &filename);

public:

};

#endif // CLIMAIN_H
