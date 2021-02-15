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

#if defined(__GNUC__) && __GNUC__ >= 4

static inline uint32_t alu_op_clz(uint32_t n) {
    int intbits = sizeof(int) * CHAR_BIT;
    if (n == 0) {
        return 32;
    }
    return __builtin_clz(n) - (intbits - 32);
}

#else /* Fallback for generic compiler */

// see https://en.wikipedia.org/wiki/Find_first_set#CLZ
static const uint8_t sig_table_4bit[16]
    = { 0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4 };

static inline uint32_t alu_op_clz(uint32_t n) {
    int len = 32;

    if (n & 0xFFFF0000) {
        len -= 16;
        n >>= 16;
    }
    if (n & 0xFF00) {
        len -= 8;
        n >>= 8;
    }
    if (n & 0xF0) {
        len -= 4;
        n >>= 4;
    }
    len -= sig_table_4bit[n];
    return len;
}

#endif /* end of mips_clz */

static inline uint64_t alu_read_hi_lo_64bit(Registers *regs) {
    uint64_t val;
    val = regs->read_hi_lo(false).as_u64();
    val |= regs->read_hi_lo(true).as_u64() << 32U;
    return val;
}

static inline void alu_write_hi_lo_64bit(Registers *regs, uint64_t val) {
    regs->write_hi_lo(false, (uint32_t)(val & 0xffffffff));
    regs->write_hi_lo(true, (uint32_t)(val >> 32U));
}

RegisterValue machine::alu_operate(
    enum AluOp operation,
    RegisterValue s,
    RegisterValue t,
    uint8_t sa,
    uint8_t sz,
    Registers *regs,
    bool &discard,
    ExceptionCause &excause) {
    int64_t s64_val;
    uint64_t u64_val;
    uint32_t u32_val;
    discard = false;

    switch (operation) {
    case ALU_OP_NOP: return 0;
    case ALU_OP_SLL: return t.as_u32() << sa;
    case ALU_OP_SRL: return t.as_u32() >> sa;
    case ALU_OP_ROTR:
        if (!sa) {
            return t.as_u32();
        }
        return (t.as_u32() >> sa) | (t.as_u32() << (32U - sa));
    case ALU_OP_SRA:
        // Note: This might be broken with some compilers but works with gcc
        return t.as_i32() >> sa;
    case ALU_OP_SLLV: return t.as_u32() << (s.as_u32() & 0x1fU);
    case ALU_OP_SRLV: return t.as_u32() >> (s.as_u32() & 0x1fU);
    case ALU_OP_ROTRV:
        u32_val = s.as_u32() & 0x1fU;
        if (!u32_val) {
            return t.as_u32();
        }
        return (t.as_u32() >> u32_val) | (t.as_u32() << (32 - u32_val));
    case ALU_OP_SRAV:
        // Note: same note as in case of SRA
        return t.as_i32() >> (s.as_u32() & 0x1fU);
    case ALU_OP_MOVZ:
        // Signal discard of result when condition is not true
        discard = (t.as_u32() != 0);
        return discard ? 0 : s;
    case ALU_OP_MOVN:
        // Same note as MOVZ
        discard = (t.as_u32() == 0);
        return discard ? 0 : s;
    case ALU_OP_MFHI: return regs->read_hi_lo(true);
    case ALU_OP_MTHI: regs->write_hi_lo(true, s); return 0x0;
    case ALU_OP_MFLO: return regs->read_hi_lo(false);
    case ALU_OP_MTLO: regs->write_hi_lo(false, s); return 0x0;
    case ALU_OP_MULT:
        s64_val = (int64_t)s.as_i32() * t.as_i32();
        alu_write_hi_lo_64bit(regs, (uint64_t)s64_val);
        return 0x0;
    case ALU_OP_MULTU:
        u64_val = (uint64_t)s.as_u32() * t.as_u32();
        alu_write_hi_lo_64bit(regs, u64_val);
        return 0x0;
    case ALU_OP_DIV:
        if (t.as_u32() == 0) {
            regs->write_hi_lo(false, 0);
            regs->write_hi_lo(true, 0);
            return 0;
        }
        regs->write_hi_lo(false, (uint32_t)(s.as_i32() / t.as_i32()));
        regs->write_hi_lo(true, (uint32_t)(s.as_i32() % t.as_i32()));
        return 0x0;
    case ALU_OP_DIVU:
        if (t.as_u32() == 0) {
            regs->write_hi_lo(false, 0);
            regs->write_hi_lo(true, 0);
            return 0;
        }
        regs->write_hi_lo(false, s.as_u32() / t.as_u32());
        regs->write_hi_lo(true, s.as_u32() % t.as_u32());
        return 0x0;
    case ALU_OP_ADD:
        /* s(31) ^ ~t(31) ... same signs on input  */
        /* (s + t)(31) ^ s(31)  ... different sign on output */
        if (((s.as_u32() ^ ~t.as_u32())
             & ((s.as_u32() + t.as_u32()) ^ s.as_u32()))
            & 0x80000000) {
            excause = EXCAUSE_OVERFLOW;
        }
        FALLTROUGH
    case ALU_OP_ADDU: return s.as_u32() + t.as_u32();
    case ALU_OP_SUB:
        /* s(31) ^ t(31) ... differnt signd on input */
        /* (s - t)(31) ^ ~s(31)  <> 0 ... otput sign differs from s  */
        if (((s.as_u32() ^ t.as_u32())
             & ((s.as_u32() - t.as_u32()) ^ s.as_u32()))
            & 0x80000000) {
            excause = EXCAUSE_OVERFLOW;
        }
        FALLTROUGH
    case ALU_OP_SUBU: return s.as_u32() - t.as_u32();
    case ALU_OP_AND: return s.as_u32() & t.as_u32();
    case ALU_OP_OR: return s.as_u32() | t.as_u32();
    case ALU_OP_XOR: return s.as_u32() ^ t.as_u32();
    case ALU_OP_NOR: return ~(s.as_u32() | t.as_u32());
    case ALU_OP_SLT:
        // Note: this is in two's complement so there is difference in unsigned
        // and signed compare
        return (s.as_i32() < t.as_i32()) ? 1 : 0;
    case ALU_OP_SLTU: return (s.as_u32() < t.as_u32()) ? 1 : 0;
    case ALU_OP_MUL: return (uint32_t)(s.as_i32() * t.as_i32());
    case ALU_OP_MADD:
        s64_val = (int64_t)alu_read_hi_lo_64bit(regs);
        s64_val += (int64_t)s.as_i32() * t.as_i32();
        alu_write_hi_lo_64bit(regs, (uint64_t)s64_val);
        return 0x0;
    case ALU_OP_MADDU:
        u64_val = alu_read_hi_lo_64bit(regs);
        u64_val += (uint64_t)s.as_u32() * t.as_u32();
        alu_write_hi_lo_64bit(regs, u64_val);
        return 0x0;
    case ALU_OP_MSUB:
        s64_val = (int64_t)alu_read_hi_lo_64bit(regs);
        s64_val -= (int64_t)s.as_i32() * t.as_i32();
        alu_write_hi_lo_64bit(regs, (uint64_t)s64_val);
        return 0x0;
    case ALU_OP_MSUBU:
        u64_val = alu_read_hi_lo_64bit(regs);
        u64_val -= (uint64_t)s.as_u32() * t.as_u32();
        alu_write_hi_lo_64bit(regs, u64_val);
        return 0x0;
    case ALU_OP_TGE:
        if (s.as_i32() >= t.as_i32()) {
            excause = EXCAUSE_TRAP;
        }
        return 0;
    case ALU_OP_TGEU:
        if (s.as_u32() >= t.as_u32()) {
            excause = EXCAUSE_TRAP;
        }
        return 0;
    case ALU_OP_TLT:
        if (s.as_i32() < t.as_i32()) {
            excause = EXCAUSE_TRAP;
        }
        return 0;
    case ALU_OP_TLTU:
        if (s.as_u32() < t.as_u32()) {
            excause = EXCAUSE_TRAP;
        }
        return 0;
    case ALU_OP_TEQ:
        if (s.as_u32() == t.as_u32()) {
            excause = EXCAUSE_TRAP;
        }
        return 0;
    case ALU_OP_TNE:
        if (s.as_u32() != t.as_u32()) {
            excause = EXCAUSE_TRAP;
        }
        return 0;
    case ALU_OP_LUI: return t.as_u32() << 16U;
    case ALU_OP_WSBH:
        return ((t.as_u32() << 8U) & 0xff00ff00)
               | ((t.as_u32() >> 8U) & 0x00ff00ffU);
    case ALU_OP_SEB: return (uint32_t)(int32_t)(int8_t)t.as_u32();
    case ALU_OP_SEH: return (uint32_t)(int32_t)(int16_t)t.as_u32();
    case ALU_OP_EXT: return (s.as_u32() >> sa) & ((1 << (sz + 1)) - 1);
    case ALU_OP_INS:
        u32_val = (1 << (sz + 1)) - 1;
        return ((s.as_u32() & u32_val) << sa) | (t.as_u32() & ~(u32_val << sa));
    case ALU_OP_CLZ: return alu_op_clz(s.as_u32());
    case ALU_OP_CLO: return alu_op_clz(~s.as_u32());
    case ALU_OP_PASS_T: // Pass s argument without change for JAL
        return t;
    case ALU_OP_BREAK:
    case ALU_OP_SYSCALL:
    case ALU_OP_RDHWR:
    case ALU_OP_MTC0:
    case ALU_OP_MFC0:
    case ALU_OP_MFMC0:
    case ALU_OP_ERET: return 0;
    default:
        throw QTMIPS_EXCEPTION(
            UnsupportedAluOperation, "Unknown ALU operation",
            QString::number(operation, 16));
    }
}
