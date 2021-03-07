#ifndef REPORTER_H
#define REPORTER_H

#include "machine/machine.h"

#include <QCoreApplication>
#include <QObject>
#include <QString>
#include <QVector>

using machine::Address;

class Reporter : public QObject {
    Q_OBJECT

public:
    Reporter(QCoreApplication *app, machine::Machine *machine);

    void regs(); // Report status of registers
    void cache_stats();
    void cycles();

    enum FailReason {
        FR_I = (1 << 0), // Unsupported Instruction
        FR_A = (1 << 1), // Unsupported ALU operation
        FR_O = (1 << 2), // Overflow/underflow of numerical operation
        FR_J = (1 << 3), // Unaligned jump
    };
    static const enum FailReason FailAny
        = (enum FailReason)(FR_I | FR_A | FR_O | FR_J);
    void expect_fail(enum FailReason reason);

    struct DumpRange {
        Address start;
        size_t len;
        /** Path to file, where this range will be dumped. */
        QString path_to_write;
    };
    void add_dump_range(Address start, size_t len, const QString &path_to_write);

private slots:
    void machine_exit();
    void machine_trap(machine::SimulatorException &e);
    void machine_exception_reached();

private:
    QCoreApplication *app;
    machine::Machine *machine;
    QVector<DumpRange> dump_ranges;

    bool e_regs;
    bool e_cache_stats;
    bool e_cycles;
    enum FailReason e_fail;

    void report();
};

#endif // REPORTER_H

