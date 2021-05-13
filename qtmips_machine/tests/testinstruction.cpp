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

#include "qtmips_machine/instruction.h"
#include "tst_machine.h"

using namespace machine;

// Test that we are correctly encoding instructions in constructor
void MachineTests::instruction() {
    QCOMPARE(Instruction(0x0), Instruction());
    QCOMPARE(Instruction(0x4432146), Instruction(1, 2, 3, 4, 5, 6));
    QCOMPARE(Instruction(0x4430004), Instruction(1, 2, 3, 4));
    QCOMPARE(Instruction(0x4000002), Instruction(1, 2_addr));
}

// Test that we are correctly decoding instruction fields
void MachineTests::instruction_access() {
    Instruction i(0xffffffff);

    QCOMPARE(i.data(), (uint32_t)0xffffffff);
    QCOMPARE(i.opcode(), (uint8_t)0x3f);
    QCOMPARE(i.rs(), (uint8_t)0x1f);
    QCOMPARE(i.rt(), (uint8_t)0x1f);
    QCOMPARE(i.rd(), (uint8_t)0x1f);
    QCOMPARE(i.shamt(), (uint8_t)0x1f);
    QCOMPARE(i.funct(), (uint8_t)0x3f);
    QCOMPARE(i.immediate(), (uint16_t)0xffff);
    QCOMPARE(i.address().get_raw(), (uint64_t)0x3ffffff);
}

// TODO test to_str
