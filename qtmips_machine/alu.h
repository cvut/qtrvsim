#ifndef ALU_H
#define ALU_H

#include <cstdint>
#include <QString>
#include <QObject>
#include <registers.h>

namespace machine {

// TODO Any other operations? We seems to be missing a lot of them.
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
    ALU_OP_MFHI = 16,
    ALU_OP_MTHI,
    ALU_OP_MFLO,
    ALU_OP_MTLO,
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
    ALU_OP_LAST = 64 // First impossible operation (just to be sure that we don't overflow)
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
