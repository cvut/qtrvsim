#include "alu.h"
#include "qtmipsexception.h"

std::uint32_t alu_operate(enum AluOp operation, std::uint32_t s, std::uint32_t t, std::uint8_t sa, Registers *regs) {
    switch(operation) {
        case ALU_OP_SLL:
            return t << sa;
        case ALU_OP_SRL:
            return t >> sa;
        case ALU_OP_SRA:
            // TODO is this correct implementation? (Should we be masking top most bit?)
            return ((t & 0x7fffffff) >> sa) | (t & 0x80000000);
        case ALU_OP_SLLV:
            return t << s;
        case ALU_OP_SRLV:
            return t >> s;
        case ALU_OP_SRAV:
            // TODO is this correct implementation? (Should we be masking top most bit?)
            return ((t & 0x7fffffff) >> s) | (t & 0x80000000);
        case ALU_OP_MOVZ:
            // We do this just to implement valid alu operation but we have to evaluate comparison way before this to disable register write
            return t == 0 ? s : 0;
        case ALU_OP_MOVN:
            // Same note as for MOVZ applies here
            return t != 0 ? s : 0;
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
        case ALU_OP_ADD:
            if (s > (0xFFFFFFFF - t))
                throw QTMIPS_EXCEPTION(Overflow, "ADD operation overflow/underflow", QString::number(s) + QString(" + ") + QString::number(t));
            __attribute__((fallthrough));
        case ALU_OP_ADDU:
            return s + t;
        case ALU_OP_SUB:
            if (s < t)
                throw QTMIPS_EXCEPTION(Overflow, "SUB operation overflow/underflow", QString::number(s) + QString(" - ") + QString::number(t));
            __attribute__((fallthrough));
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
        default:
            throw QTMIPS_EXCEPTION(UnsupportedAluOperation, "Unknown ALU operation", QString::number(operation, 16));
    }
}

QString alu_str(enum AluOp operation, std::uint32_t s, std::uint32_t t, std::uint8_t sa) {
    // TODO
    return QString("");
}
