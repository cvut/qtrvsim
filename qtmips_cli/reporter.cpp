#include "reporter.h"
#include <iostream>
#include <typeinfo>
#include <qtmipsexception.h>

using namespace machine;
using namespace std;

Reporter::Reporter(QCoreApplication *app, QtMipsMachine *machine) : QObject() {
    this->app = app;
    this->machine = machine;

    connect(machine, SIGNAL(program_exit()), this, SLOT(machine_exit()));
    connect(machine, SIGNAL(program_trap(machine::QtMipsException&)), this, SLOT(machine_trap(machine::QtMipsException&)));

    e_regs = false;
    e_fail = (enum FailReason)0;
}

void Reporter::regs() {
    e_regs = true;
}

void Reporter::expect_fail(enum FailReason reason) {
    e_fail = (enum FailReason)(e_fail | reason);
}

void Reporter::machine_exit() {
    report();
    if (e_fail != 0) {
        cout << "Machine was expected to fail but it didn't." << endl;
        app->exit(1);
    } else
        app->exit();
}

void Reporter::machine_trap(QtMipsException &e) {
    report();

    bool expected = false;
    auto& etype = typeid(e);
    if (etype == typeid(QtMipsExceptionUnsupportedInstruction))
        expected = e_fail & FR_I;
    else if (etype == typeid(QtMipsExceptionUnsupportedAluOperation))
        expected = e_fail & FR_A;
    else if (etype == typeid(QtMipsExceptionOverflow))
        expected = e_fail & FR_O;
    else if (etype == typeid(QtMipsExceptionUnalignedJump))
        expected = e_fail & FR_J;

    cout << "Machine trapped: " << e.what() << endl;
    app->exit(expected ? 0 : 1);
}

void Reporter::report() {
    if (e_regs) {
        // TODO
    }
}
