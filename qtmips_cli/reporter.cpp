// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

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

    cout << "Machine trapped: " << e.msg(false).toStdString() << endl;
    app->exit(expected ? 0 : 1);
}

void Reporter::report() {
    if (e_regs) {
        // TODO
    }
}
