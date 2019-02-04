// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

#include "instruction.h"
#include "alu.h"
#include "memory.h"
#include "qtmipsexception.h"

using namespace machine;

#define FLAGS_ALU_I (IMF_SUPPORTED | IMF_ALUSRC | IMF_REGWRITE | IMF_ALU_REQ_RS)
#define FLAGS_ALU_I_ZE (FLAGS_ALU_I | IMF_ZERO_EXTEND)

#define FLAGS_ALU_I_LOAD (IMF_SUPPORTED | IMF_ALUSRC | IMF_REGWRITE | \
                          IMF_MEMREAD | IMF_MEM | IMF_ALU_REQ_RS)
#define FLAGS_ALU_I_STORE (IMF_SUPPORTED | IMF_ALUSRC | IMF_MEMWRITE | \
                          IMF_MEM | IMF_MEM_STORE | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT)

#define FLAGS_ALU_T_R (IMF_SUPPORTED | IMF_REGD | IMF_REGWRITE)
#define FLAGS_ALU_T_R_STD (IMF_SUPPORTED | IMF_REGD | IMF_REGWRITE \
                         | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT)
#define FLAGS_ALU_T_R_ST (IMF_SUPPORTED | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT)
#define FLAGS_ALU_T_R_S (IMF_SUPPORTED |  IMF_ALU_REQ_RS)
#define FLAGS_ALU_T_R_D (IMF_SUPPORTED | IMF_REGD | IMF_REGWRITE )
#define FLAGS_ALU_T_R_SD (IMF_SUPPORTED | IMF_REGD | IMF_REGWRITE | IMF_ALU_REQ_RS)

#define NOALU .alu = ALU_OP_SLL
#define NOMEM .mem_ctl = AC_NONE

#define IM_UNKNOWN {"UNKNOWN", Instruction::T_UNKNOWN, NOALU, NOMEM, nullptr, .flags = 0}

struct InstructionMap {
    const char *name;
    enum Instruction::Type type;
    enum AluOp alu;
    enum AccessControl mem_ctl;
    const  struct InstructionMap *subclass; // when subclass is used then flags has special meaning
    unsigned int flags;
};

#define IMF_SUB_ENCODE(bits, shift) (((bits) << 8) | (shift))
#define IMF_SUB_GET_BITS(subcode) (((subcode) >> 8) & 0xff)
#define IMF_SUB_GET_SHIFT(subcode) ((subcode) & 0xff)

#define IT_R Instruction::T_R
#define IT_I Instruction::T_I
#define IT_J Instruction::T_J

// This table is indexed by funct
static const struct InstructionMap  alu_instruction_map[] = {
    {"SLL",    IT_R, ALU_OP_SLL, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
    IM_UNKNOWN,
    {"SRL",    IT_R, ALU_OP_SRL, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_SD},
    {"SRA",    IT_R, ALU_OP_SRA, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_SD},
    {"SLLV",   IT_R, ALU_OP_SLLV, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
    IM_UNKNOWN,
    {"SRLV",   IT_R, ALU_OP_SRLV, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
    {"SRAV",   IT_R, ALU_OP_SRAV, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
    {"JR",     IT_R, ALU_OP_JR, NOMEM, nullptr,
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS},
    {"JALR",   IT_R, ALU_OP_JALR, NOMEM, nullptr,
     .flags = IMF_SUPPORTED | IMF_REGD | IMF_REGWRITE | IMF_BJR_REQ_RS | IMF_PC8_TO_RT},
    {"MOVZ",   IT_R, ALU_OP_MOVZ, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
    {"MOVN",   IT_R, ALU_OP_MOVN, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
    IM_UNKNOWN,
    {"BREAK",  IT_R, ALU_OP_BREAK, NOMEM, nullptr,
     .flags = IMF_SUPPORTED},
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"MFHI",   IT_R, ALU_OP_MFHI, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_D | IMF_READ_HILO},
    {"MTHI",   IT_R, ALU_OP_MTHI, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_S | IMF_WRITE_HILO},
    {"MFLO",   IT_R, ALU_OP_MFLO, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_D | IMF_READ_HILO},
    {"MTLO",   IT_R, ALU_OP_MTLO, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_S | IMF_WRITE_HILO},
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"MULT",   IT_R, ALU_OP_MULT, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_ST | IMF_WRITE_HILO},    // 24
    {"MULTU",  IT_R, ALU_OP_MULTU, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_ST | IMF_WRITE_HILO},   // 25
    {"DIV",    IT_R, ALU_OP_DIV, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_ST | IMF_WRITE_HILO},     // 26
    {"DIVU",   IT_R, ALU_OP_DIVU, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_ST | IMF_WRITE_HILO},    // 27
    IM_UNKNOWN, // 28
    IM_UNKNOWN, // 29
    IM_UNKNOWN, // 30
    IM_UNKNOWN, // 31
    {"ADD",    IT_R, ALU_OP_ADDU, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},     // 32
    {"ADDU",   IT_R, ALU_OP_ADDU, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
    {"SUB",    IT_R, ALU_OP_SUB, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
    {"SUBU",   IT_R, ALU_OP_SUBU, NOMEM, nullptr,
     .flags =  FLAGS_ALU_T_R_STD},
    {"AND",    IT_R, ALU_OP_AND, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
    {"OR",     IT_R, ALU_OP_OR, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
    {"XOR",    IT_R, ALU_OP_XOR, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
    {"NOR",    IT_R, ALU_OP_NOR, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"SLT",    IT_R, ALU_OP_SLT, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
    {"SLTU",   IT_R, ALU_OP_SLTU, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
};

const std::int32_t instruction_map_opcode_field = IMF_SUB_ENCODE(6, 26);

// This table is indexed by opcode
static const struct InstructionMap instruction_map[] = {
    {"ALU",    IT_R, NOALU, NOMEM, alu_instruction_map,          // Alu operations
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"REGIMM", IT_I, NOALU, NOMEM, nullptr,          // REGIMM (BLTZ, BGEZ)
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS},
    {"J",      IT_J, NOALU, NOMEM, nullptr,          // J
     .flags = IMF_SUPPORTED},
    {"JAL",    IT_J, ALU_OP_PASS_S, NOMEM, nullptr,  // JAL
     .flags = IMF_SUPPORTED | IMF_PC_TO_R31 | IMF_REGWRITE},
    {"BEQ",    IT_I, NOALU, NOMEM, nullptr,         // BEQ
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BJR_REQ_RT},
    {"BNE",    IT_I, NOALU, NOMEM, nullptr,          // BNE
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BJR_REQ_RT},
    {"BLEZ",   IT_I, NOALU, NOMEM, nullptr,          // BLEZ
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS},
    {"BGTZ",   IT_I, NOALU, NOMEM, nullptr,          // BGTZ
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS},
    {"ADDI",   IT_I, ALU_OP_ADD, NOMEM, nullptr,     // ADDI
     .flags = FLAGS_ALU_I},
    {"ADDIU",  IT_I, ALU_OP_ADDU, NOMEM, nullptr,    // ADDIU
     .flags = FLAGS_ALU_I},
    {"SLTI",   IT_I, ALU_OP_SLT, NOMEM, nullptr,     // SLTI
     .flags = FLAGS_ALU_I},
    {"SLTIU",  IT_I, ALU_OP_SLTU, NOMEM, nullptr,    // SLTIU
     .flags = FLAGS_ALU_I},
    {"ANDI",   IT_I, ALU_OP_AND, NOMEM, nullptr,     // ANDI
     .flags = FLAGS_ALU_I_ZE},
    {"ORI",    IT_I, ALU_OP_OR, NOMEM, nullptr,      // ORI
     .flags = FLAGS_ALU_I_ZE},
    {"XORI",   IT_I, ALU_OP_XOR, NOMEM, nullptr,     // XORI
     .flags = FLAGS_ALU_I_ZE},
    {"LUI",    IT_I, ALU_OP_LUI, NOMEM, nullptr,     // LUI
     .flags = FLAGS_ALU_I},
    IM_UNKNOWN,  // 16
    IM_UNKNOWN,  // 17
    IM_UNKNOWN,  // 18
    IM_UNKNOWN,  // 19
    IM_UNKNOWN,  // 20
    IM_UNKNOWN,  // 21
    IM_UNKNOWN,  // 22
    IM_UNKNOWN,  // 23
    IM_UNKNOWN,  // 24
    IM_UNKNOWN,  // 25
    IM_UNKNOWN,  // 26
    IM_UNKNOWN,  // 27
    IM_UNKNOWN,  // 28
    IM_UNKNOWN,  // 29
    IM_UNKNOWN,  // 30
    IM_UNKNOWN,  // 31
    {"LB",     IT_I, ALU_OP_ADDU, AC_BYTE, nullptr,  // LB
     .flags = FLAGS_ALU_I_LOAD},
    {"LH",     IT_I, ALU_OP_ADDU, AC_HALFWORD, nullptr,  // LH
     .flags = FLAGS_ALU_I_LOAD},
    {"LWL",    IT_I, ALU_OP_ADDU, NOMEM, nullptr,    // LWL - unsupported
     .flags = IMF_MEM},
    {"LW",     IT_I, ALU_OP_ADDU, AC_WORD, nullptr,  // LW
     .flags = FLAGS_ALU_I_LOAD},
    {"LBU",    IT_I, ALU_OP_ADDU, AC_BYTE_UNSIGNED, nullptr,  // LBU
     .flags = FLAGS_ALU_I_LOAD},
    {"LHU",    IT_I, ALU_OP_ADDU, AC_HALFWORD_UNSIGNED, nullptr,   // LHU
     .flags = FLAGS_ALU_I_LOAD},
    {"LWR",    IT_I, ALU_OP_ADDU, NOMEM, nullptr,    // LWR - unsupported
     .flags = IMF_MEM},
    IM_UNKNOWN,  // 39
    {"SB",     IT_I, ALU_OP_ADDU, AC_BYTE, nullptr,  // SB
     .flags = FLAGS_ALU_I_STORE},
    {"SH",     IT_I, ALU_OP_ADDU, AC_HALFWORD, nullptr,   // SH
     .flags = FLAGS_ALU_I_STORE},
    {"SWL",    IT_I, ALU_OP_ADDU, NOMEM, nullptr,    // SWL
     .flags = IMF_MEM | IMF_MEM_STORE},
    {"SW",     IT_I, ALU_OP_ADDU, AC_WORD, nullptr,  // SW
     .flags = FLAGS_ALU_I_STORE},
    IM_UNKNOWN,  // 44
    IM_UNKNOWN,  // 45
    {"SWR",    IT_I, ALU_OP_ADDU, NOMEM, nullptr,    // SWR
     .flags = IMF_MEM | IMF_MEM_STORE},
    {"CACHE",  IT_I, ALU_OP_ADDU, AC_CACHE_OP, nullptr, // CACHE
     .flags = IMF_SUPPORTED | IMF_ALUSRC | IMF_MEM| IMF_MEM_STORE},
    IM_UNKNOWN,  // 48
    IM_UNKNOWN,  // 49
    IM_UNKNOWN,  // 50
    IM_UNKNOWN,  // 51
    IM_UNKNOWN,  // 52
    IM_UNKNOWN,  // 53
    IM_UNKNOWN,  // 54
    IM_UNKNOWN,  // 55
    IM_UNKNOWN,  // 56
    IM_UNKNOWN,  // 57
    IM_UNKNOWN,  // 58
    IM_UNKNOWN,  // 59
    IM_UNKNOWN,  // 60
    IM_UNKNOWN,  // 61
    IM_UNKNOWN,  // 62
    IM_UNKNOWN,  // 63
};

#undef IM_UNKNOWN

static inline const struct InstructionMap &InstructionMapFind(std::uint32_t code) {
    const struct InstructionMap *im = instruction_map;
    std::uint32_t flags = instruction_map_opcode_field;
    do {
        unsigned int bits = IMF_SUB_GET_BITS(flags);
        unsigned int shift = IMF_SUB_GET_SHIFT(flags);
        im = im + ((code >> shift) & ((1 << bits) - 1));
        if (im->subclass == nullptr)
            return *im;
        flags = im->flags;
        im = im->subclass;
    } while(1);
}

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

enum Instruction::Type Instruction::type() const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    return im.type;
}

enum InstructionFlags Instruction::flags() const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    return (enum InstructionFlags)im.flags;
}
enum AluOp Instruction::alu_op() const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    return im.alu;
}

enum AccessControl Instruction::mem_ctl() const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    return im.mem_ctl;
}

void Instruction::flags_alu_op_mem_ctl(enum InstructionFlags &flags,
                  enum AluOp &alu_op, enum AccessControl &mem_ctl) const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    flags = (enum InstructionFlags)im.flags;
    alu_op = im.alu;
    mem_ctl = im.mem_ctl;
}

bool Instruction::is_break() const {
    return opcode() == 0 && funct() == ALU_OP_BREAK;
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

QString Instruction::to_str() const {
    // TODO there are exception where some fields are zero and such so we should not print them in such case
    if  (opcode() >= (sizeof(instruction_map) / sizeof(struct InstructionMap)))
        return QString("UNKNOWN");
    if (dt == 0)
        return QString("NOP");
    const struct InstructionMap &im = instruction_map[opcode()];
    QString res;
    switch (im.type) {
    case T_I:
        res += im.name;
        if (im.flags & IMF_MEM) {
            res += " $" + QString::number(rt());
            res += ", 0x" + QString::number(immediate(), 16).toUpper() + "(" + QString::number(rs()) + ")";
        } else {
            res += " $" + QString::number(rt());
            if (!(im.flags & IMF_NO_RS))
                res += ", $" + QString::number(rs());
            res += ", 0x" + QString::number(immediate(), 16).toUpper();
        }
        break;
    case T_J:
        res += im.name;
        // TODO we need to know instruction address to expand address section by it
        res += " 0x" + QString::number(address(), 16).toUpper();
        break;
    case T_R:
        {
        // Note that all R instructions we support has opcode == 0 and so they are processed by alu table
        if (funct() >= (sizeof(alu_instruction_map) / sizeof(*alu_instruction_map)))
            return QString("UNKNOWN");
        const struct InstructionMap &am = alu_instruction_map[funct()];
        res += am.name;
        res += " $" + QString::number(rd()) + ", $" + QString::number(rs()) + ", $" + QString::number(rt());
        break;
        }
	case T_UNKNOWN:
		return QString("UNKNOWN");
    }
    return res;
}
