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

#ifndef MACHINEDEFS_H
#define MACHINEDEFS_H

#include <stdint.h>

namespace machine {

enum AccessControl {
    AC_NONE,
    AC_BYTE,
    AC_HALFWORD,
    AC_WORD,
    AC_BYTE_UNSIGNED,
    AC_HALFWORD_UNSIGNED,
    AC_LOAD_LINKED,
    AC_STORE_CONDITIONAL,
    AC_WORD_RIGHT,
    AC_WORD_LEFT,
    AC_CACHE_OP,

    AC_FIRST_REGULAR = AC_BYTE,
    AC_LAST_REGULAR = AC_HALFWORD_UNSIGNED,
};

enum ExceptionCause {
    EXCAUSE_NONE     =  0,  // Use zero as default value when no exception is pending
    EXCAUSE_INT      =  1,  // Int is 0 on real CPU and in Cause regsiter
    EXCAUSE_ADDRL    =  4,
    EXCAUSE_ADDRS    =  5,
    EXCAUSE_IBUS     =  6,
    EXCAUSE_DBUS     =  7,
    EXCAUSE_SYSCALL  =  8,
    EXCAUSE_BREAK    =  9,
    EXCAUSE_OVERFLOW = 12,
    EXCAUSE_TRAP     = 13,
    EXCAUSE_HWBREAK  = 14,
    EXCAUSE_COUNT    = 15,
};

enum AluOp : std::uint8_t {
    ALU_OP_NOP,
    ALU_OP_SLL,
    ALU_OP_SRL,
    ALU_OP_ROTR,
    ALU_OP_SRA,
    ALU_OP_SLLV,
    ALU_OP_SRLV,
    ALU_OP_ROTRV,
    ALU_OP_SRAV,
    ALU_OP_MOVZ,
    ALU_OP_MOVN,
    ALU_OP_MFHI,
    ALU_OP_MTHI,
    ALU_OP_MFLO,
    ALU_OP_MTLO,
    ALU_OP_MULT,
    ALU_OP_MULTU,
    ALU_OP_DIV,
    ALU_OP_DIVU,
    ALU_OP_ADD,
    ALU_OP_ADDU,
    ALU_OP_SUB,
    ALU_OP_SUBU,
    ALU_OP_AND,
    ALU_OP_OR,
    ALU_OP_XOR,
    ALU_OP_NOR,
    ALU_OP_SLT,
    ALU_OP_SLTU,
    ALU_OP_MUL,
    ALU_OP_MADD,
    ALU_OP_MADDU,
    ALU_OP_MSUB,
    ALU_OP_MSUBU,
    ALU_OP_TGE,
    ALU_OP_TGEU,
    ALU_OP_TLT,
    ALU_OP_TLTU,
    ALU_OP_TEQ,
    ALU_OP_TNE,
    ALU_OP_LUI,
    ALU_OP_WSBH,
    ALU_OP_SEB,
    ALU_OP_SEH,
    ALU_OP_EXT,
    ALU_OP_INS,
    ALU_OP_CLZ,
    ALU_OP_CLO,
    ALU_OP_PASS_T, // Pass t argument without change for JAL
    ALU_OP_BREAK,
    ALU_OP_SYSCALL,
    ALU_OP_RDHWR,
    ALU_OP_MTC0,
    ALU_OP_MFC0,
    ALU_OP_MFMC0,
    ALU_OP_ERET,
    ALU_OP_UNKNOWN,
    ALU_OP_LAST // First impossible operation (just to be sure that we don't overflow)
};

enum LocationStatus {
    LOCSTAT_NONE      = 0,
    LOCSTAT_CACHED    = 1 << 0,
    LOCSTAT_DIRTY     = 1 << 1,
    LOCSTAT_READ_ONLY = 1 << 2,
    LOCSTAT_ILLEGAL   = 1 << 3,
};

}

#endif // MACHINEDEFS_H
