#include "alu.h"
#include "qtmipsexception.h"

std::uint32_t alu_operate(enum AluOp operation, std::uint32_t s, std::uint32_t t, std::uint8_t sa) {
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
        case ALU_OP_ADD:
            if (s > (0xFFFFFFFF - t))
                throw QTMIPS_EXCEPTION(Overflow, "ADD operation overflow/underflow", QString::number(s) + QString(" + ") + QString::number(t));
            // Intentional falltrough
        case ALU_OP_ADDU:
            return s + t;
        case ALU_OP_SUB:
            if (s < t)
                throw QTMIPS_EXCEPTION(Overflow, "SUB operation overflow/underflow", QString::number(s) + QString(" - ") + QString::number(t));
            // Intentional falltrough
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
        case ALU_OP_SLTU:
            // TODO is this correct implementation? (this is two's complement signed representation so do we care?)
            // Intentional falltrough
        case ALU_OP_SLT:
            return (s < t) ? 1 : 0;
        default:
            throw QTMIPS_EXCEPTION(UnsupportedAluOperation, "Unknown ALU operation", QString::number(operation, 16));
    }
}

QString alu_str(enum AluOp operation, std::uint32_t s, std::uint32_t t, std::uint8_t sa) {
    // TODO
    return QString("");
}
