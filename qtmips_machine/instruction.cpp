#include "instruction.h"

InstructionR::InstructionR(std::uint8_t rs, std::uint8_t rd, std::uint8_t rt, std::uint8_t sa) {
    this->rs = rs;
    this->rd = rd;
    this->rt = rt;
    this->sa = sa;
}

// TODO for registers output as register ($0)!

QVector<QString> InstructionR::to_strs() {
    QVector<QString> str;
    // Instruction name
    str << "unknown"; // unknown instruction, should be replaced by child
    // Source register
    str << QString::number((unsigned)this->rs, 16);
    // Target register
    str << QString::number((unsigned)this->rt, 16);
    // Destination register
    str << QString::number((unsigned)this->rd, 16);
    // Shift amount
    str << QString::number((unsigned)this->sa, 16);
    return str;
}

InstructionI::InstructionI(std::uint8_t rs, std::uint8_t rt, std::uint16_t immediate) {
    this->rs = rs;
    this->rt = rt;
    this->immediate = immediate;
}

QVector<QString> InstructionI::to_strs() {
    QVector<QString> str;
    // Instruction name
    str << "unknown"; // unknown instruction, should be replaced by child
    // Source register
    str << QString::number((unsigned)this->rs, 16);
    // Target register
    str << QString::number((unsigned)this->rt, 16);
    // Immediate value
    str << QString::number((unsigned)this->immediate, 16);
    return str;
}

InstructionJ::InstructionJ(std::uint32_t address) {
    this->address = address;
}

QVector<QString> InstructionJ::to_strs() {
    QVector<QString> str;
    // Instruction name
    str << "unknown"; // unknown instruction, should be replaced by child
    // Source register
    str << QString::number((unsigned)this->address, 16);
    return str;
}
