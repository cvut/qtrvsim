#include "shift.h"

InstructionShift::InstructionShift(enum InstructionShiftT type, std::uint8_t rt, std::uint8_t rd, std::uint8_t sa)
    : InstructionR(0, rt, rd, sa) {
    this->type = type;
}

QVector<QString> InstructionShift::to_strs() {
    QVector<QString> str = this->InstructionR::to_strs();
    str.erase(str.begin() + 1); // Drop rs field
    switch (this->type) {
    case IST_LL:
        str[0] = "sll";
        break;
    case IST_RL:
        str[0] = "srl";
        break;
    case IST_RA:
        str[0] = "sra";
        break;
    default:
        // TODO different exception
        throw std::exception();
    }
    return str;
}

InstructionShiftVariable::InstructionShiftVariable(enum InstructionShiftT type, std::uint8_t rs, std::uint8_t rt, std::uint8_t rd)
    : InstructionR(rs, rt, rd, 0) {
    this->type = type;
}

QVector<QString> InstructionShiftVariable::to_strs() {
    QVector<QString> str = this->InstructionR::to_strs();
    str.erase(str.begin() + 4); // Drop sa field
    switch (this->type) {
    case IST_LL:
        str[0] = "sllv";
        break;
    case IST_RL:
        str[0] = "srlv";
        break;
    case IST_RA:
        str[0] = "srav";
        break;
    default:
        // TODO different exception
        throw std::exception();
    }
    return str;
}
