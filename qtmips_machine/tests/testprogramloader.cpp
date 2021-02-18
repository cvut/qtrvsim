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

#include "memory/backend/memory.h"
#include "qtmips_machine/instruction.h"
#include "qtmips_machine/memory/memory_utils.h"
#include "qtmips_machine/programloader.h"
#include "tst_machine.h"

using namespace machine;

// This is common program start (initial value of program counter)
#define PC_INIT 0x80020000

void MachineTests::program_loader() {
    ProgramLoader pl("data");
    Memory m;
    pl.to_memory(&m);

    // 	addi $1, $0, 6
    QCOMPARE(
        Instruction(memory_read_u32(&m, PC_INIT)), Instruction(8, 0, 1, 6));
    // j (8)0020000 (only 28 bits are used and they are logically shifted left
    // by 2)
    QCOMPARE(
        Instruction(memory_read_u32(&m, PC_INIT + 4)),
        Instruction(2, Address(0x20000 >> 2)));
    // TODO add some more code to data and do more compares (for example more
    // sections)
}
