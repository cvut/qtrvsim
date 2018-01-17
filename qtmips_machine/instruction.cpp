#include "instruction.h"
#include "qtmipsexception.h"

using namespace machine;
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

Instruction::Instruction(const Instruction &i) {
    this->dt = i.data();
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


enum InstructionType {
    IT_R,
    IT_I,
    IT_J
};

struct InstructionMap {
    const char *name;
    enum InstructionType type;
};

#define IM_UNKNOWN {"UNKNOWN", IT_R}
// This table is indexed by opcode
static const struct InstructionMap instruction_map[] = {
    {"ALU", IT_R}, // Alu operations
    {"REGIMM", IT_I}, // REGIMM (BLTZ, BGEZ)
    {"J", IT_J},
    {"JAL", IT_J},
    {"BEQ", IT_I},
    {"BNE", IT_I},
    {"BLEZ", IT_I},
    {"BGTZ", IT_I},
    {"ADDI", IT_I},
    {"ADDIU", IT_I},
    {"SLTI", IT_I},
    {"SLTIU", IT_I},
    {"ANDI", IT_I},
    {"ORI", IT_I},
    {"XORI", IT_I},
    {"LUI", IT_I},
    IM_UNKNOWN, // 16
    IM_UNKNOWN, // 17
    IM_UNKNOWN, // 18
    IM_UNKNOWN, // 19
    IM_UNKNOWN, // 20
    IM_UNKNOWN, // 21
    IM_UNKNOWN, // 22
    IM_UNKNOWN, // 23
    IM_UNKNOWN, // 24
    IM_UNKNOWN, // 25
    IM_UNKNOWN, // 26
    IM_UNKNOWN, // 27
    IM_UNKNOWN, // 28
    IM_UNKNOWN, // 29
    IM_UNKNOWN, // 30
    IM_UNKNOWN, // 31
    {"LB", IT_I},
    {"LH", IT_I},
    {"LWL", IT_I},
    {"LW", IT_I},
    {"LBU", IT_I},
    {"LHU", IT_I},
    {"LWR", IT_I},
    IM_UNKNOWN, // 39
    {"SB", IT_I},
    {"SH", IT_I},
    {"SWL", IT_I},
    {"SW", IT_I},
    IM_UNKNOWN, // 44
    IM_UNKNOWN, // 45
    {"SWR", IT_I},
    IM_UNKNOWN, // 47
    IM_UNKNOWN, // 48
    IM_UNKNOWN, // 49
    IM_UNKNOWN, // 50
    IM_UNKNOWN, // 51
    IM_UNKNOWN, // 52
    IM_UNKNOWN, // 53
    IM_UNKNOWN, // 54
    IM_UNKNOWN, // 55
    IM_UNKNOWN, // 56
    IM_UNKNOWN, // 57
    IM_UNKNOWN, // 58
    IM_UNKNOWN, // 59
    IM_UNKNOWN, // 60
    IM_UNKNOWN, // 61
    IM_UNKNOWN, // 61
    IM_UNKNOWN, // 62
    IM_UNKNOWN // 63
};
#undef IM_UNKNOWN

struct AluInstructionMap {
    const char *name;
};

#define AIM_UNKNOWN {"UNKNOWN"}
// This table is indexed by funct
static const struct AluInstructionMap alu_instruction_map[] = {
    {"SLL"},
    AIM_UNKNOWN,
    {"SRL"},
    {"SRA"},
    {"SLLV"},
    AIM_UNKNOWN,
    {"SRLV"},
    {"SRAV"},
    {"JR"},
    {"JALR"},
    {"MOVZ"},
    {"MOVN"},
    AIM_UNKNOWN,
    AIM_UNKNOWN,
    AIM_UNKNOWN,
    AIM_UNKNOWN,
    {"MFHU"},
    {"MTHI"},
    {"MFLO"},
    {"MTLO"},
    AIM_UNKNOWN,
    AIM_UNKNOWN,
    AIM_UNKNOWN,
    AIM_UNKNOWN,
    AIM_UNKNOWN,
    AIM_UNKNOWN,
    AIM_UNKNOWN,
    AIM_UNKNOWN,
    AIM_UNKNOWN,
    AIM_UNKNOWN,
    AIM_UNKNOWN,
    AIM_UNKNOWN,
    {"ADD"},
    {"ADDU"},
    {"SUB"},
    {"SUBU"},
    {"AND"},
    {"OR"},
    {"XOR"},
    {"NOR"},
    AIM_UNKNOWN,
    AIM_UNKNOWN,
    {"SLT"},
    {"SLTU"}
};
#undef AIM_UNKNOWN

QString Instruction::to_str() const {
    // TODO there are exception where some fields are zero and such so we should not print them in such case
    if  (opcode() >= (sizeof(instruction_map) / sizeof(struct InstructionMap)))
        return QString("UNKNOWN");
    if (dt == 0)
        return QString("NOP");
    const struct InstructionMap &im = instruction_map[opcode()];
    QString res;
    switch (im.type) {
    case IT_I:
        res += im.name;
        res += " $" + QString::number(rs()) + ", $" + QString::number(rt()) + ", 0x" + QString::number(immediate(), 16);
        break;
    case IT_J:
        res += im.name;
        // TODO we need to know instruction address to expand address section by it
        res += " " + QString::number(address(), 16);
        break;
    case IT_R:
        {
        // Note that all R instructions we support has opcode == 0 and so they are processed by alu table
        if (funct() >= (sizeof(alu_instruction_map) / sizeof(struct AluInstructionMap)))
            return QString("UNKNOWN");
        const struct AluInstructionMap &am = alu_instruction_map[funct()];
        res += am.name;
        res += " $" + QString::number(rs()) + ", $" + QString::number(rt()) + ", $" + QString::number(rd());
        break;
        }
    }
    return res;
}
