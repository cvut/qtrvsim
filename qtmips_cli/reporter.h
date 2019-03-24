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
    void machine_exception_reached();

private:
    QCoreApplication *app;
    machine::QtMipsMachine *machine;

    bool e_regs;
    enum FailReason e_fail;

    void report();
};

#endif // REPORTER_H
