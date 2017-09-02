#include "instructions/arithmetic.h"

InstructionArithmetic::InstructionArithmetic(enum InstructionArithmeticT type, std::uint8_t rs, std::uint8_t rd, std::uint8_t rt)
    : InstructionR(rs, rd, rt, 0) {
    this->type = type;
}

QVector<QString> InstructionArithmetic::to_strs() {
    QVector<QString> str = this->InstructionR::to_strs();
    str.erase(str.begin() + 4); // Drop sa field
    switch (this->type) {
    case IAT_ADD:
        str[0] = "add";
        break;
    case IAT_ADDU:
        str[0] = "addu";
        break;
    case IAT_SUB:
        str[0] = "sub";
        break;
    case IAT_SUBU:
        str[0] = "subu";
        break;
    case IAT_AND:
        str[0] = "and";
        break;
    case IAT_OR:
        str[0] = "or";
        break;
    case IAT_XOR:
        str[0] = "xor";
        break;
    case IAT_NOR:
        str[0] = "nor";
        break;
    case IAT_SLT:
        str[0] = "slt";
        break;
    case IAT_SLTU:
        str[0] = "sltu";
        break;
    default:
        // TODO different exception
        throw std::exception();
    }
    return str;
}

InstructionArithmeticImmediate::InstructionArithmeticImmediate(enum InstructionArithmeticImmediateT type, std::uint8_t rs, std::uint8_t rt, std::uint16_t value)
    : InstructionI(rs, rt, value) {
    this->type = type;
}

QVector<QString> InstructionArithmeticImmediate::to_strs() {
    QVector<QString> str = this->InstructionI::to_strs();
    switch (this->type) {
    case IAT_ADDI:
        str[0] = "addi";
        break;
    case IAT_ADDIU:
        str[0] = "addiu";
        break;
    case IAT_ANDI:
        str[0] = "andi";
        break;
    case IAT_ORI:
        str[0] = "ori";
        break;
    case IAT_XORI:
        str[0] = "xori";
        break;
    case IAT_SLTI:
        str[0] = "slti";
        break;
    case IAT_SLTIU:
        str[0] = "sltiu";
        break;
    case IAT_LUI:
        str[0] = "lu";
        break;
    default:
        // TODO different exception
        throw std::exception();
    }
    return str;
}
