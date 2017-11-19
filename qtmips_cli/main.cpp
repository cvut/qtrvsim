#include <QCoreApplication>
#include <QCommandLineParser>
#include <iostream>
#include "tracer.h"

void create_parser(QCommandLineParser &p) {
    p.setApplicationDescription("QtMips CLI machine simulator");
    p.addHelpOption();
    p.addVersionOption();

    p.addPositionalArgument("FILE", "Input ELF executable file");
}

void configure_machine(QCommandLineParser &p, MachineConfig &cc) {
    QStringList pa = p.positionalArguments();
    if (pa.size() != 1) {
        std::cerr << "Single ELF file has to be specified" << std::endl;
        exit(1);
    }
    cc.set_elf(pa[0]);

    // TODO
}

void configure_tracer(QCommandLineParser &p, Tracer &tr) {
    // TODO
    tr.reg_pc();
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    app.setApplicationName("qtmips_cli");
    app.setApplicationVersion("0.1");

    QCommandLineParser p;
    create_parser(p);
    p.process(app);

    MachineConfig cc;
    configure_machine(p, cc);
    QtMipsMachine machine(cc);

    app.connect(&machine, SIGNAL(program_exit()), &app, SLOT(quit()));

    Tracer tr(&machine);
    configure_tracer(p, tr);

    machine.play(); // Run machine
    return app.exec();
}
