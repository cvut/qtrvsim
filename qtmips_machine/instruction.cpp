#include "instruction.h"
#include "qtmipsexception.h"

struct InstructionMap {
    const char *name;
};

#define IM_UNKNOWN {"UNKNOWN"}
const struct InstructionMap instruction_map[] = {
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"ADDI"},
    {"ADDIU"},
    {"SLTI"},
    {"SLTIU"},
    {"ANDI"},
    {"ORI"},
    {"XORI"},
    {"LUI"},
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN
};

Instruction::Instruction() {
    this->dt = 0;
}

Instruction::Instruction(std::uint32_t inst) {
    this->dt = inst;
}

Instruction::Instruction(std::uint8_t opcode, std::uint8_t rs, std::uint8_t rt, std::uint8_t rd, std::uint8_t shamt, std::uint8_t funct) {
    this->dt = 0;
    this->dt |= opcode << 26;
    this->dt |= rs << 21;
    this->dt |= rt << 16;
    this->dt |= rd << 11;
    this->dt |= shamt << 6;
    this->dt |= funct;
}

Instruction::Instruction(std::uint8_t opcode, std::uint8_t rs, std::uint8_t rt, std::uint16_t immediate) {
    this->dt = 0;
    this->dt |= opcode << 26;
    this->dt |= rs << 21;
    this->dt |= rt << 16;
    this->dt |= immediate;
}

Instruction::Instruction(std::uint8_t opcode, std::uint32_t address) {
    this->dt = 0;
    this->dt |= opcode << 26;
    this->dt |= address;
}

Instruction::Instruction(const Instruction &i) : Instruction(i.data()) { }

QString Instruction::to_str() {
    if  (this->opcode() >= sizeof(instruction_map))
        return QString("UNKNOWN");
    return QString(instruction_map[this->opcode()].name);
}

#define MASK(LEN,OFF) ((this->dt >> (OFF)) & ((1 << (LEN)) - 1))

std::uint8_t Instruction::opcode() const {
    return (std::uint8_t) MASK(6, 26);
}

std::uint8_t Instruction::rs() const {
    return (std::uint8_t) MASK(5, 21);
}

std::uint8_t Instruction::rt() const {
    return (std::uint8_t) MASK(5, 16);
}

std::uint8_t Instruction::rd() const {
    return (std::uint8_t) MASK(5, 11);
}

std::uint8_t Instruction::shamt() const {
    return (std::uint8_t) MASK(5, 6);

}

std::uint8_t Instruction::funct() const {
    return (std::uint8_t) MASK(6, 0);
}

std::uint16_t Instruction::immediate() const {
    return (std::uint16_t) MASK(16, 0);
}

std::uint32_t Instruction::address() const {
    return (std::uint32_t) MASK(26, 0);
}

std::uint32_t Instruction::data() const {
    return this->dt;
}

bool Instruction::operator==(const Instruction &c) const {
    return (this->data() == c.data());
}

bool Instruction::operator!=(const Instruction &c) const {
    return ! this->operator==(c);
}

Instruction &Instruction::operator=(const Instruction &c) {
    if (this != &c)
        this->dt = c.data();
    return *this;
}
