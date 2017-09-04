#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include "instruction.h"

enum InstructionArithmeticT {
    IAT_ADD, // Add
    IAT_ADDU, // Add unsigned
    IAT_SUB, // Subtract
    IAT_SUBU, // Subtract unsigned
    IAT_AND,
    IAT_OR,
    IAT_XOR,
    IAT_NOR,
    IAT_SLT, // set on less than
    IAT_SLTU, // set on less than unsigned
};

class InstructionArithmetic : public InstructionR  {
public:
    InstructionArithmetic(enum InstructionArithmeticT type, std::uint8_t rs, std::uint8_t rd, std::uint8_t rt);

    void decode(Registers *regs);
    void execute();
    void memory(Memory *mem);
    void write_back(Registers *regs);

    QVector<QString> to_strs();
private:
    enum InstructionArithmeticT type;
    std::uint32_t rs_d, rd_d, rt_d;
};

enum InstructionArithmeticImmediateT {
    IAT_ADDI,
    IAT_ADDIU,
    IAT_ANDI,
    IAT_ORI,
    IAT_XORI,
    IAT_SLTI,
    IAT_SLTIU,
    IAT_LUI
};

class InstructionArithmeticImmediate : public InstructionI {
public:
    InstructionArithmeticImmediate(enum InstructionArithmeticImmediateT type, std::uint8_t rs, std::uint8_t rt, std::uint16_t value);

    void decode(Registers *regs);
    void execute();
    void memory(Memory *mem);
    void write_back(Registers *regs);

    QVector<QString> to_strs();
private:
    enum InstructionArithmeticImmediateT type;
    std::uint32_t rs_d, rt_d;
};

#endif // ARITHMETIC_H
