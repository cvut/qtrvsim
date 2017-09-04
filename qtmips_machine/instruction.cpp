#include "instruction.h"
#include "qtmipsexception.h"

Instruction::Instruction() {
    this->st = IS_FETCH;
}

void Instruction::decode(Registers *regs) {
    if (this->st != IS_FETCH)
        // TODO other exception
        throw std::exception();
    this->st = IS_DECODE;
}

void Instruction::execute() {
    if (this->st != IS_DECODE)
        // TODO other exception
        throw std::exception();
    this->st = IS_EXECUTE;
}

void Instruction::memory(Memory *mem) {
    if (this->st != IS_EXECUTE)
        // TODO other exception
        throw std::exception();
    this->st = IS_MEMORY;
}

void Instruction::write_back(Registers *regs) {
    if (this->st != IS_MEMORY)
        // TODO other exception
        throw std::exception();
    this->st = IS_WRITE_BACK;
}

enum InstructionState Instruction::state() {
    return this->st;
}

bool Instruction::running() {
    return this->st > IS_FETCH && this->st < IS_WRITE_BACK;
}

bool Instruction::done() {
    return this->st >= IS_WRITE_BACK;
}

InstructionR::InstructionR(std::uint8_t rs, std::uint8_t rd, std::uint8_t rt, std::uint8_t sa) : Instruction() {
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
    str << QString::number((unsigned)this->rs, 10);
    // Target register
    str << QString::number((unsigned)this->rt, 10);
    // Destination register
    str << QString::number((unsigned)this->rd, 10);
    // Shift amount
    str << QString::number((unsigned)this->sa, 10);
    return str;
}

InstructionI::InstructionI(std::uint8_t rs, std::uint8_t rt, std::uint16_t immediate) : Instruction() {
    this->rs = rs;
    this->rt = rt;
    this->immediate = immediate;
}

QVector<QString> InstructionI::to_strs() {
    QVector<QString> str;
    // Instruction name
    str << "unknown"; // unknown instruction, should be replaced by child
    // Source register
    str << QString::number((unsigned)this->rs, 10);
    // Target register
    str << QString::number((unsigned)this->rt, 10);
    // Immediate value
    str << QString::number((unsigned)this->immediate, 16);
    return str;
}

InstructionJ::InstructionJ(std::uint32_t address) : Instruction() {
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
