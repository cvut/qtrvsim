#ifndef REPORTER_H
#define REPORTER_H

#include <QObject>
#include <QCoreApplication>
#include "qtmipsmachine.h"

class Reporter : QObject {
    Q_OBJECT
public:
    Reporter(QCoreApplication *app, machine::QtMipsMachine *machine);

    void regs(); // Report status of registers
    // TODO

    enum FailReason {
        FR_I = (1<<0), // Unsupported Instruction
        FR_A = (1<<1), // Unsupported ALU operation
        FR_O = (1<<2), // Overflow/underflow of numerical operation
        FR_J = (1<<3), // Unaligned jump
    };
    static const enum FailReason FailAny = (enum FailReason)(FR_I | FR_A | FR_O | FR_J);

    void expect_fail(enum FailReason reason);

private slots:
    void machine_exit();
    void machine_trap(machine::QtMipsException &e);

private:
    QCoreApplication *app;
    machine::QtMipsMachine *machine;

    bool e_regs;
    enum FailReason e_fail;

    void report();
};

#endif // REPORTER_H
