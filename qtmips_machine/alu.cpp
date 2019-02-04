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

#include "alu.h"
#include "qtmipsexception.h"
#include "utils.h"

using namespace machine;

std::uint32_t machine::alu_operate(enum AluOp operation, std::uint32_t s, std::uint32_t t, std::uint8_t sa, Registers *regs) {
    std::int64_t s64_val;
    std::uint64_t u64_val;

    switch(operation) {
        case ALU_OP_SLL:
            return t << sa;
        case ALU_OP_SRL:
            return t >> sa;
        case ALU_OP_SRA:
            // Note: This might be broken with some compilers but works with gcc
            return (std::int32_t)t >> sa;
        case ALU_OP_SLLV:
            return t << s;
        case ALU_OP_SRLV:
            return t >> s;
        case ALU_OP_SRAV:
            // Note: same note as in case of SRA
            return (std::int32_t)t >> s;
        case ALU_OP_JR:
            // Do nothing as we solve this when we are handling program counter in instruction decode (handle_pc)
            return 0;
        case ALU_OP_JALR:
            // Pass return value in rt to save PC after isntruction, program counter is handled in handle_pc
            return t;
        case ALU_OP_MOVZ:
            // We do this just to implement valid alu operation but we have to evaluate comparison outside of this function to disable register write
            return t == 0 ? s : 0;
        case ALU_OP_MOVN:
            // Same note as for MOVZ applies here
            return t != 0 ? s : 0;
        case ALU_OP_BREAK:
             return 0;
        case ALU_OP_MFHI:
            return regs->read_hi_lo(true);
        case ALU_OP_MTHI:
            regs->write_hi_lo(true, s);
            return 0x0;
        case ALU_OP_MFLO:
            return regs->read_hi_lo(false);
        case ALU_OP_MTLO:
            regs->write_hi_lo(false, s);
            return 0x0;
        case ALU_OP_MULT:
            s64_val = (std::int64_t)(std::int32_t)s * (std::int32_t)t;
            regs->write_hi_lo(false, (std::uint32_t)(s64_val & 0xffffffff));
            regs->write_hi_lo(true,  (std::uint32_t)(s64_val >> 32));
            return 0x0;
        case ALU_OP_MULTU:
            u64_val = (std::uint64_t)s * t;
            regs->write_hi_lo(false, (std::uint32_t)(u64_val & 0xffffffff));
            regs->write_hi_lo(true,  (std::uint32_t)(u64_val >> 32));
            return 0x0;
        case ALU_OP_DIV:
            regs->write_hi_lo(false, (std::uint32_t)((std::int32_t)s / (std::int32_t)t));
            regs->write_hi_lo(true,  (std::uint32_t)((std::int32_t)s % (std::int32_t)t));
            return 0x0;
        case ALU_OP_DIVU:
            regs->write_hi_lo(false, s / t);
            regs->write_hi_lo(true, s % t);
            return 0x0;
        case ALU_OP_ADD:
            /* s(31) ^ ~t(31) ... same signs on input  */
            /* (s + t)(31) ^ s(31)  ... different sign on output */
            if (((s ^ ~t) & ((s + t) ^ s)) & 0x80000000)
                throw QTMIPS_EXCEPTION(Overflow, "ADD operation overflow/underflow", QString::number(s) + QString(" + ") + QString::number(t));
            FALLTROUGH
        case ALU_OP_ADDU:
            return s + t;
        case ALU_OP_SUB:
            /* s(31) ^ t(31) ... differnt signd on input */
            /* (s - t)(31) ^ ~s(31)  <> 0 ... otput sign differs from s  */
            if (((s ^ t) & ((s - t) ^ s)) & 0x80000000)
                throw QTMIPS_EXCEPTION(Overflow, "SUB operation overflow/underflow", QString::number(s) + QString(" - ") + QString::number(t));
            FALLTROUGH
        case ALU_OP_SUBU:
            return s - t;
        case ALU_OP_AND:
            return s & t;
        case ALU_OP_OR:
            return s | t;
        case ALU_OP_XOR:
            return s ^ t;
        case ALU_OP_NOR:
            return ~(s | t);
        case ALU_OP_SLT:
            // Note: this is in two's complement so there is difference in unsigned and signed compare
            return ((std::int32_t)s < (std::int32_t)t) ? 1 : 0;
        case ALU_OP_SLTU:
            return (s < t) ? 1 : 0;
        case ALU_OP_LUI:
            return t << 16;
        case  ALU_OP_PASS_S: // Pass s argument without change for JAL
            return s;
        default:
            throw QTMIPS_EXCEPTION(UnsupportedAluOperation, "Unknown ALU operation", QString::number(operation, 16));
    }
}
