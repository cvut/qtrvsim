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

#ifndef ALU_H
#define ALU_H

#include <cstdint>
#include <QString>
#include <QObject>
#include <registers.h>

#include "machinedefs.h"

namespace machine {

// Do ALU operation.
// excause: Reported exception by given operation
// operation: This is function field from instruction or shifted opcode for immediate instructions
// s: Loaded from rs. Also calles as source.
// t: Loaded from rt or immediate field from instruction it self. Also called as target.
// sa: This is value directly from instruction it self (sa section) used for shift operations
// sz: This is value directly from instruction it self used in filed extract instructions
// regs: Registers used. We need direct access to lo and hi registers (those are not accessed from core it self but from alu directly
// Returned value is commonly saved to rt/rd or any other way passed trough core
std::uint32_t alu_operate(enum AluOp operation, std::uint32_t s,
                          std::uint32_t t, std::uint8_t sa, std::uint8_t sz,
                          Registers *regs, bool &discard, ExceptionCause &excause);

}

Q_DECLARE_METATYPE(machine::AluOp)
Q_DECLARE_METATYPE(machine::ExceptionCause)

#endif // ALU_H
