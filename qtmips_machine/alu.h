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

namespace machine {

enum AluOp : std::uint8_t {
    ALU_OP_SLL = 0,
    ALU_OP_SRL = 2,
    ALU_OP_SRA,
    ALU_OP_SLLV,
    ALU_OP_SRLV = 6,
    ALU_OP_SRAV,
    ALU_OP_JR,
    ALU_OP_JALR,
    ALU_OP_MOVZ,
    ALU_OP_MOVN,
    ALU_OP_BREAK = 13,
    ALU_OP_MFHI = 16,
    ALU_OP_MTHI,
    ALU_OP_MFLO,
    ALU_OP_MTLO,
    ALU_OP_MULT = 24,
    ALU_OP_MULTU = 25,
    ALU_OP_DIV = 26,
    ALU_OP_DIVU = 27,
    ALU_OP_ADD = 32,
    ALU_OP_ADDU,
    ALU_OP_SUB,
    ALU_OP_SUBU,
    ALU_OP_AND,
    ALU_OP_OR,
    ALU_OP_XOR,
    ALU_OP_NOR,
    ALU_OP_SLT = 42,
    ALU_OP_SLTU,
    ALU_OP_LUI = 64, // We don't care about exact index for this one
    ALU_OP_PASS_S, // Pass s argument without change for JAL
    ALU_OP_LAST // First impossible operation (just to be sure that we don't overflow)
};

// Do ALU operation.
// operation: This is function field from instruction or shifted opcode for immediate instructions
// s: Loaded from rs. Also calles as source.
// t: Loaded from rt or immediate field from instruction it self. Also called as target.
// sa: This is value directly from instruction it self (sa section) used for shift operations
// regs: Registers used. We need direct access to lo and hi registers (those are not accessed from core it self but from alu directly
// Returned value is commonly saved to rt/rd or any other way passed trough core
std::uint32_t alu_operate(enum AluOp operation, std::uint32_t s, std::uint32_t t, std::uint8_t sa, Registers *regs);

}

Q_DECLARE_METATYPE(machine::AluOp)

#endif // ALU_H
