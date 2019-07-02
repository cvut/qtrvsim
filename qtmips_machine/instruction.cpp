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

#include <QMultiMap>
#include <QVector>
#include <QStringList>
#include <QChar>
#include <iostream>
#include <cctype>
#include <cstring>
#include "instruction.h"
#include "alu.h"
#include "memory.h"
#include "utils.h"
#include "qtmipsexception.h"

using namespace machine;

#define IMF_SUB_ENCODE(bits, shift) (((bits) << 8) | (shift))
#define IMF_SUB_GET_BITS(subcode) (((subcode) >> 8) & 0xff)
#define IMF_SUB_GET_SHIFT(subcode) ((subcode) & 0xff)

#define RS_SHIFT 21
#define RT_SHIFT 16
#define RD_SHIFT 11
#define SHAMT_SHIFT 6

#define FIELD_RS        IMF_SUB_ENCODE(5, RS_SHIFT)
#define FIELD_RT        IMF_SUB_ENCODE(5, RT_SHIFT)
#define FIELD_RD        IMF_SUB_ENCODE(5, RD_SHIFT)
#define FIELD_SHAMT     IMF_SUB_ENCODE(5, SHAMT_SHIFT)
#define FIELD_IMMEDIATE IMF_SUB_ENCODE(16, 0)
#define FIELD_DELTA     IMF_SUB_ENCODE(16, 0)
#define FIELD_TARGET    IMF_SUB_ENCODE(26, 0)
#define FIELD_COPZ      IMF_SUB_ENCODE(25, 0)
#define FIELD_CODE      IMF_SUB_ENCODE(10,16)
#define FIELD_PREFX     IMF_SUB_ENCODE(5, 11)
#define FIELD_CACHE     IMF_SUB_ENCODE(5, 16)
#define FIELD_CODE2     IMF_SUB_ENCODE(10, 6)
#define FIELD_CODE20    IMF_SUB_ENCODE(20, 6)
#define FIELD_CODE19    IMF_SUB_ENCODE(19, 6)
#define FIELD_SEL       IMF_SUB_ENCODE(3, 0)
#define FIELD_IGNORE    0

struct ArgumentDesc {
    inline ArgumentDesc(char name, char kind, std::uint32_t loc, std::int64_t min, std::int64_t max, unsigned shift) {
        this->name = name;
        this->kind = kind;
        this->loc = loc;
        this->min = min;
        this->max = max;
        this->shift = shift;
    }
    char name;
    char kind;
    std::uint32_t loc;
    std::int64_t  min;
    std::int64_t  max;
    unsigned      shift;
};

static const ArgumentDesc argdeslist[] = {
    ArgumentDesc('1', 'n', FIELD_SHAMT, 0, 31, 0),    // 5 bit sync type {OP_*_SHAMT}
    ArgumentDesc('<', 'n', FIELD_SHAMT, 0, 31, 0),    // 5 bit shift amount {OP_*_SHAMT}
    ArgumentDesc('>', 'n', FIELD_SHAMT, 32, 63, 0),   // shift amount between 32 and 63, stored after subtracting 32 {OP_*_SHAMT}
    ArgumentDesc('a', 'a', FIELD_TARGET, 0, 0x3ffffff, 2), // 26 bit target address {OP_*_TARGET}
    ArgumentDesc('b', 'g', FIELD_RS, 0, 31, 0),    // 5 bit base register {OP_*_RS}
    ArgumentDesc('c', 'g', FIELD_CODE, 0, 0x3ff, 0), // 10 bit breakpoint code {OP_*_CODE}
    ArgumentDesc('d', 'g', FIELD_RD, 0, 31, 0),    // 5 bit destination register specifier {OP_*_RD}
    ArgumentDesc('h', 'h', FIELD_PREFX, 0, 31, 0),    // 5 bit prefx hint {OP_*_PREFX}
    ArgumentDesc('H', 'n', FIELD_SEL, 0, 7, 0),    // 3 select field of MTC0, MFC0
    ArgumentDesc('i', 'n', FIELD_IMMEDIATE, 0, 0xffff, 0) , // 16 bit unsigned immediate {OP_*_IMMEDIATE}
    ArgumentDesc('j', 'n', FIELD_IMMEDIATE, -0x8000, 0x7fff, 0) , // 16 bit signed immediate {OP_*_DELTA}
    ArgumentDesc('k', 'n', FIELD_CACHE, 0, 31, 0),       // 5 bit cache opcode in target register position {OP_*_CACHE}
                             // Also used for immediate operands in vr5400 vector insns.
    ArgumentDesc('o', 'o', FIELD_DELTA, -0x8000, 0x7fff, 0) , // 16 bit signed offset {OP_*_DELTA}
    ArgumentDesc('p', 'p', FIELD_DELTA, -0x8000, 0x7fff, 2) , // 16 bit PC relative branch target address {OP_*_DELTA}
    ArgumentDesc('q', 'n', FIELD_CODE2, 0, 0x3ff, 0) , // 10 bit extra breakpoint code {OP_*_CODE2}
    ArgumentDesc('r', 'g', FIELD_RS, 0, 31, 0) , // 5 bit same register used as both source and target {OP_*_RS}
    ArgumentDesc('s', 'g', FIELD_RS, 0, 31, 0) , // 5 bit source register specifier {OP_*_RS}
    ArgumentDesc('t', 'g', FIELD_RT, 0, 31, 0) , // 5 bit target register {OP_*_RT}
    ArgumentDesc('u', 'n', FIELD_IMMEDIATE, 0, 0xffff, 0) , // 16 bit upper 16 bits of address {OP_*_IMMEDIATE}
    ArgumentDesc('v', 'g', FIELD_RS, 0, 31, 0) , // 5 bit same register used as both source and destination {OP_*_RS}
    ArgumentDesc('w', 'g', FIELD_RT, 0, 31, 0) , // 5 bit same register used as both target and destination {OP_*_RT}
    ArgumentDesc('U', 'g', FIELD_RD, 0, 31, 0) , // 5 bit same destination register in both OP_*_RD and OP_*_RT
    ArgumentDesc('G', 'g', FIELD_RD, 0, 31, 0) , // 5 destination register used in MFC0, MTC0
                             // {used by clo and clz}
    ArgumentDesc('C', 'n', FIELD_COPZ, 0, 0x1ffffff, 0) , // 25 bit coprocessor function code {OP_*_COPZ}
    ArgumentDesc('B', 'n', FIELD_CODE20, 0, 0xfffff, 0) , // 20 bit syscall/breakpoint function code {OP_*_CODE20}
    ArgumentDesc('J', 'n', FIELD_CODE19, 0, 0x7ffff, 0) , // 19 bit wait function code {OP_*_CODE19}
    ArgumentDesc('x', 'g', FIELD_IGNORE, 0, 31, 0) , // accept and ignore register name
    ArgumentDesc('z', 'n', FIELD_IGNORE, 0, 0, 0) , // must be zero register
};

static const ArgumentDesc *argdesbycode[(int)('z' + 1)];

static bool fill_argdesbycode() {
    uint i;
    for (i = 0; i < sizeof(argdeslist)/sizeof(*argdeslist); i++)
        argdesbycode[(uint)argdeslist[i].name] = &argdeslist[i];
    return true;
}

bool argdesbycode_filled = fill_argdesbycode();

#define FLAGS_ALU_I_NO_RS (IMF_SUPPORTED | IMF_ALUSRC | IMF_REGWRITE)
#define FLAGS_ALU_I (IMF_SUPPORTED | IMF_ALUSRC | IMF_REGWRITE | IMF_ALU_REQ_RS)
#define FLAGS_ALU_I_ZE (FLAGS_ALU_I | IMF_ZERO_EXTEND)

#define FLAGS_ALU_I_LOAD (IMF_SUPPORTED | IMF_ALUSRC | IMF_REGWRITE | \
                          IMF_MEMREAD | IMF_MEM | IMF_ALU_REQ_RS)
#define FLAGS_ALU_I_STORE (IMF_SUPPORTED | IMF_ALUSRC | IMF_MEMWRITE | \
                          IMF_MEM | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT)

#define FLAGS_ALU_T_R_D (IMF_SUPPORTED | IMF_REGD | IMF_REGWRITE)
#define FLAGS_ALU_T_R_STD (FLAGS_ALU_T_R_D | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT)
#define FLAGS_ALU_T_R_STD_SHV (FLAGS_ALU_T_R_STD | IMF_ALU_SHIFT)
#define FLAGS_ALU_T_R_TD (FLAGS_ALU_T_R_D | IMF_ALU_REQ_RT)
#define FLAGS_ALU_T_R_TD_SHAMT (FLAGS_ALU_T_R_TD | IMF_ALU_SHIFT)
#define FLAGS_ALU_T_R_S (IMF_SUPPORTED |  IMF_ALU_REQ_RS)
#define FLAGS_ALU_T_R_SD (FLAGS_ALU_T_R_D | IMF_ALU_REQ_RS)
#define FLAGS_ALU_T_R_ST (IMF_SUPPORTED | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT)

#define FLAGS_ALU_TRAP_ST (IMF_SUPPORTED | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT)
#define FLAGS_ALU_TRAP_SI (IMF_SUPPORTED | IMF_ALU_REQ_RS | IMF_ALUSRC)

#define FLAGS_J_B_PC_TO_R31 (IMF_SUPPORTED | IMF_PC_TO_R31 | IMF_REGWRITE)

#define NOALU .alu = ALU_OP_SLL
#define NOMEM .mem_ctl = AC_NONE

#define IM_UNKNOWN {"UNKNOWN", Instruction::T_UNKNOWN, NOALU, NOMEM, nullptr, {}, 0, 0, 0}

struct InstructionMap {
    const char *name;
    enum Instruction::Type type;
    enum AluOp alu;
    enum AccessControl mem_ctl;
    const struct InstructionMap *subclass; // when subclass is used then flags has special meaning
    const QStringList args;
    std::uint32_t code;
    std::uint32_t mask;
    unsigned int flags;
};

#define IT_R Instruction::T_R
#define IT_I Instruction::T_I
#define IT_J Instruction::T_J

static const struct InstructionMap  srl_rotr_instruction_map[] = {
    {"SRL",    IT_R, ALU_OP_SRL, NOMEM, nullptr, {"d", "w", "<"}, 0x00000002, 0xffe0003f,
     .flags = FLAGS_ALU_T_R_TD_SHAMT},
    {"ROTR",   IT_R, ALU_OP_ROTR, NOMEM, nullptr, {"d", "w", "<"}, 0x00200002, 0xffe0003f,
     .flags = FLAGS_ALU_T_R_TD_SHAMT},
};

static const struct InstructionMap  srlv_rotrv_instruction_map[] = {
    {"SRLV",   IT_R, ALU_OP_SRLV, NOMEM, nullptr, {"d", "t", "s"}, 0x00000006, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD_SHV},
    {"ROTRV",   IT_R, ALU_OP_ROTRV, NOMEM, nullptr, {"d", "t", "s"}, 0x00000046, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD_SHV},
};

// This table is indexed by funct
static const struct InstructionMap  alu_instruction_map[] = {
    {"SLL",    IT_R, ALU_OP_SLL, NOMEM, nullptr, {"d", "w", "<"}, 0x00000000, 0xffe0003f,
     .flags = FLAGS_ALU_T_R_TD_SHAMT},
    IM_UNKNOWN,
    {"SRL",    IT_R, ALU_OP_SRL, NOMEM, srl_rotr_instruction_map, {}, 0, 0,
     .flags = IMF_SUB_ENCODE(1, 21)},
    {"SRA",    IT_R, ALU_OP_SRA, NOMEM, nullptr, {"d", "w", "<"},	0x00000003, 0xffe0003f,
     .flags = FLAGS_ALU_T_R_TD_SHAMT},
    {"SLLV",   IT_R, ALU_OP_SLLV, NOMEM, nullptr, {"d", "t", "s"}, 0x00000004, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD_SHV},
    IM_UNKNOWN,
    {"SRLV",   IT_R, ALU_OP_SRLV, NOMEM, srlv_rotrv_instruction_map, {"d", "t", "s"}, 0x00000006, 0xfc0007ff,
     .flags = IMF_SUB_ENCODE(1, 6)},
    {"SRAV",   IT_R, ALU_OP_SRAV, NOMEM, nullptr, {"d", "t", "s"}, 0x00000007, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD_SHV},
    {"JR",     IT_R, ALU_OP_NOP, NOMEM, nullptr, {"s"}, 0x00000008, 0xfc1fffff,
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_JUMP},
    {"JALR",   IT_R, ALU_OP_PASS_T, NOMEM, nullptr, {"d", "s"}, 0x00000009, 0xfc1f07ff,
     .flags = IMF_SUPPORTED | IMF_REGD | IMF_REGWRITE | IMF_BJR_REQ_RS | IMF_PC8_TO_RT | IMF_JUMP},
    {"MOVZ",   IT_R, ALU_OP_MOVZ, NOMEM, nullptr, {"d", "v", "t"}, 0x0000000a, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD},
    {"MOVN",   IT_R, ALU_OP_MOVN, NOMEM, nullptr, {"d", "v", "t"}, 0x0000000b, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD},
    {"SYSCALL",IT_R, ALU_OP_SYSCALL, NOMEM, nullptr, {nullptr}, 0x0000000c, 0xfc00003f,
     .flags = IMF_SUPPORTED | IMF_EXCEPTION},
    {"BREAK",  IT_R, ALU_OP_BREAK, NOMEM, nullptr, {nullptr}, 0x0000000d, 0xfc00003f,
     .flags = IMF_SUPPORTED | IMF_EXCEPTION},
    IM_UNKNOWN,
    {"SYNC", IT_I, NOALU, NOMEM, nullptr, {nullptr}, 0x0000000f, 0xfffff83f,       // SYNC
     .flags = IMF_SUPPORTED},
    {"MFHI",   IT_R, ALU_OP_MFHI, NOMEM, nullptr, {"d"}, 0x00000010, 0xffff07ff,
     .flags = FLAGS_ALU_T_R_D | IMF_READ_HILO},
    {"MTHI",   IT_R, ALU_OP_MTHI, NOMEM, nullptr, {"s"}, 0x00000011, 0xfc1fffff,
     .flags = FLAGS_ALU_T_R_S | IMF_WRITE_HILO},
    {"MFLO",   IT_R, ALU_OP_MFLO, NOMEM, nullptr, {"d"}, 0x00000012, 0xffff07ff,
     .flags = FLAGS_ALU_T_R_D | IMF_READ_HILO},
    {"MTLO",   IT_R, ALU_OP_MTLO, NOMEM, nullptr, {"s"}, 0x00000013, 0xfc1fffff,
     .flags = FLAGS_ALU_T_R_S | IMF_WRITE_HILO},
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"MULT",   IT_R, ALU_OP_MULT, NOMEM, nullptr, {"s", "t"}, 0x00000018, 0xfc00ffff,
     .flags = FLAGS_ALU_T_R_ST | IMF_WRITE_HILO},    // 24
    {"MULTU",  IT_R, ALU_OP_MULTU, NOMEM, nullptr, {"s", "t"}, 0x00000019, 0xfc00ffff,
     .flags = FLAGS_ALU_T_R_ST | IMF_WRITE_HILO},   // 25
    {"DIV",    IT_R, ALU_OP_DIV, NOMEM, nullptr, {"s", "t"}, 0x0000001a, 0xfc00ffff,
     .flags = FLAGS_ALU_T_R_ST | IMF_WRITE_HILO},     // 26
    {"DIVU",   IT_R, ALU_OP_DIVU, NOMEM, nullptr, {"s", "t"}, 0x0000001b, 0xfc00ffff,
     .flags = FLAGS_ALU_T_R_ST | IMF_WRITE_HILO},    // 27
    IM_UNKNOWN, // 28
    IM_UNKNOWN, // 29
    IM_UNKNOWN, // 30
    IM_UNKNOWN, // 31
    {"ADD",    IT_R, ALU_OP_ADD, NOMEM, nullptr, {"d", "v", "t"}, 0x00000020, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD},     // 32
    {"ADDU",   IT_R, ALU_OP_ADDU, NOMEM, nullptr, {"d", "v", "t"}, 0x00000021, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD},
    {"SUB",    IT_R, ALU_OP_SUB, NOMEM, nullptr, {"d", "v", "t"}, 0x00000022, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD},
    {"SUBU",   IT_R, ALU_OP_SUBU, NOMEM, nullptr, {"d", "v", "t"}, 0x00000023, 0xfc0007ff,
     .flags =  FLAGS_ALU_T_R_STD},
    {"AND",    IT_R, ALU_OP_AND, NOMEM, nullptr, {"d", "v", "t"}, 0x00000024, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD},
    {"OR",     IT_R, ALU_OP_OR, NOMEM, nullptr, {"d", "v", "t"}, 0x00000025, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD},
    {"XOR",    IT_R, ALU_OP_XOR, NOMEM, nullptr, {"d", "v", "t"}, 0x00000026, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD},
    {"NOR",    IT_R, ALU_OP_NOR, NOMEM, nullptr, {"d", "v", "t"}, 0x00000027, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD},
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"SLT",    IT_R, ALU_OP_SLT, NOMEM, nullptr, {"d", "v", "t"}, 0x0000002a, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD},
    {"SLTU",   IT_R, ALU_OP_SLTU, NOMEM, nullptr, {"d", "v", "t"}, 0x0000002b, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD},
    IM_UNKNOWN, // 44
    IM_UNKNOWN, // 45
    IM_UNKNOWN, // 46
    IM_UNKNOWN, // 47
    {"TGE", IT_I, ALU_OP_TGE, NOMEM, nullptr, {"s", "t"}, 0x00000030, 0xfc00003f, // TGE 48
     .flags = FLAGS_ALU_TRAP_ST},
    {"TGEU", IT_I, ALU_OP_TGEU, NOMEM, nullptr, {"s", "t"}, 0x00000031, 0xfc00003f, // TGEU 49
     .flags = FLAGS_ALU_TRAP_ST},
    {"TLT", IT_I, ALU_OP_TLT, NOMEM, nullptr, {"s", "t"}, 0x00000032, 0xfc00003f, // TLT 50
     .flags = FLAGS_ALU_TRAP_ST},
    {"TLTU", IT_I, ALU_OP_TGEU, NOMEM, nullptr, {"s", "t"}, 0x00000033, 0xfc00003f, // TLTU 51
     .flags = FLAGS_ALU_TRAP_ST},
    {"TEQ", IT_I, ALU_OP_TEQ, NOMEM, nullptr, {"s", "t"}, 0x00000034, 0xfc00003f, // TEQ 52
     .flags = FLAGS_ALU_TRAP_ST},
    IM_UNKNOWN, // 53
    {"TNE", IT_I, ALU_OP_TNE, NOMEM, nullptr, {"s", "t"}, 0x00000036, 0xfc00003f, // TNE 54
     .flags = FLAGS_ALU_TRAP_ST},
    IM_UNKNOWN, // 55
    IM_UNKNOWN, // 56
    IM_UNKNOWN, // 57
    IM_UNKNOWN, // 58
    IM_UNKNOWN, // 59
    IM_UNKNOWN, // 60
    IM_UNKNOWN, // 61
    IM_UNKNOWN, // 62
    IM_UNKNOWN, // 63
};

static const struct InstructionMap  special2_instruction_map[] = {
    {"MADD",    IT_R, ALU_OP_MADD, NOMEM, nullptr, {"s", "t"}, 0x70000000, 0xfc00ffff,
     .flags = FLAGS_ALU_T_R_ST | IMF_READ_HILO | IMF_WRITE_HILO},
    {"MADDU",   IT_R, ALU_OP_MADDU, NOMEM, nullptr, {"s", "t"}, 0x70000001, 0xfc00ffff,
     .flags = FLAGS_ALU_T_R_ST | IMF_READ_HILO | IMF_WRITE_HILO},
    {"MUL",     IT_R, ALU_OP_MUL, NOMEM, nullptr, {"d", "v", "t"}, 0x70000002, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_STD},     // 32
    IM_UNKNOWN,	//	3
    {"MSUB",    IT_R, ALU_OP_MSUB, NOMEM, nullptr, {"s", "t"}, 0x70000004, 0xfc00ffff,
     .flags = FLAGS_ALU_T_R_ST | IMF_READ_HILO | IMF_WRITE_HILO},
    {"MSUBU",   IT_R, ALU_OP_MSUBU, NOMEM, nullptr, {"s", "t"}, 0x70000005, 0xfc00ffff,
     .flags = FLAGS_ALU_T_R_ST | IMF_READ_HILO | IMF_WRITE_HILO},
    IM_UNKNOWN,	//	6
    IM_UNKNOWN,	//	7
    IM_UNKNOWN,	//	8
    IM_UNKNOWN,	//	9
    IM_UNKNOWN,	//	10
    IM_UNKNOWN,	//	11
    IM_UNKNOWN,	//	12
    IM_UNKNOWN,	//	13
    IM_UNKNOWN,	//	14
    IM_UNKNOWN,	//	15
    IM_UNKNOWN,	//	16
    IM_UNKNOWN,	//	17
    IM_UNKNOWN,	//	18
    IM_UNKNOWN,	//	19
    IM_UNKNOWN,	//	20
    IM_UNKNOWN,	//	21
    IM_UNKNOWN,	//	22
    IM_UNKNOWN,	//	23
    IM_UNKNOWN,	//	24
    IM_UNKNOWN,	//	25
    IM_UNKNOWN,	//	26
    IM_UNKNOWN,	//	27
    IM_UNKNOWN,	//	28
    IM_UNKNOWN,	//	29
    IM_UNKNOWN,	//	30
    IM_UNKNOWN,	//	31
    {"CLZ",   IT_R, ALU_OP_CLZ, NOMEM, nullptr, {"U", "s"}, 0x70000020, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_SD},
    {"CLO",   IT_R, ALU_OP_CLO, NOMEM, nullptr, {"U", "s"}, 0x70000021, 0xfc0007ff,
     .flags = FLAGS_ALU_T_R_SD},
    IM_UNKNOWN,	//	34
    IM_UNKNOWN,	//	35
    IM_UNKNOWN,	//	36
    IM_UNKNOWN,	//	37
    IM_UNKNOWN,	//	38
    IM_UNKNOWN,	//	39
    IM_UNKNOWN,	//	40
    IM_UNKNOWN,	//	41
    IM_UNKNOWN,	//	42
    IM_UNKNOWN,	//	43
    IM_UNKNOWN,	//	44
    IM_UNKNOWN,	//	45
    IM_UNKNOWN,	//	46
    IM_UNKNOWN,	//	47
    IM_UNKNOWN,	//	48
    IM_UNKNOWN,	//	49
    IM_UNKNOWN,	//	50
    IM_UNKNOWN,	//	51
    IM_UNKNOWN,	//	52
    IM_UNKNOWN,	//	53
    IM_UNKNOWN,	//	54
    IM_UNKNOWN,	//	55
    IM_UNKNOWN,	//	56
    IM_UNKNOWN,	//	57
    IM_UNKNOWN,	//	58
    IM_UNKNOWN,	//	59
    IM_UNKNOWN,	//	60
    IM_UNKNOWN,	//	61
    IM_UNKNOWN,	//	62
    IM_UNKNOWN,	//	63
};

static const struct InstructionMap  bshfl_instruction_map[] = {
    IM_UNKNOWN,	//	0
    IM_UNKNOWN,	//	1
    {"WSBH", IT_R, ALU_OP_WSBH, NOMEM, nullptr, {"d", "w"}, 0x7c0000a0, 0xffe007ff,
     .flags = FLAGS_ALU_T_R_TD},
    IM_UNKNOWN,	//	3
    IM_UNKNOWN,	//	4
    IM_UNKNOWN,	//	5
    IM_UNKNOWN,	//	6
    IM_UNKNOWN,	//	7
    IM_UNKNOWN,	//	8
    IM_UNKNOWN,	//	9
    IM_UNKNOWN,	//	10
    IM_UNKNOWN,	//	11
    IM_UNKNOWN,	//	12
    IM_UNKNOWN,	//	13
    IM_UNKNOWN,	//	14
    IM_UNKNOWN,	//	15
    {"SEB", IT_R, ALU_OP_SEB, NOMEM, nullptr, {"d", "w"}, 0x7c000420, 0xffe007ff,
     .flags = FLAGS_ALU_T_R_TD},
    IM_UNKNOWN,	//	17
    IM_UNKNOWN,	//	18
    IM_UNKNOWN,	//	19
    IM_UNKNOWN,	//	20
    IM_UNKNOWN,	//	21
    IM_UNKNOWN,	//	22
    IM_UNKNOWN,	//	23
    {"SEH", IT_R, ALU_OP_SEH, NOMEM, nullptr, {"d", "w"}, 0x7c000620, 0xffe007ff,
     .flags = FLAGS_ALU_T_R_TD},
    IM_UNKNOWN,	//	25
    IM_UNKNOWN,	//	26
    IM_UNKNOWN,	//	27
    IM_UNKNOWN,	//	28
    IM_UNKNOWN,	//	29
    IM_UNKNOWN,	//	30
    IM_UNKNOWN,	//	31
};

static const struct InstructionMap  special3_instruction_map[] = {
    {"EXT", IT_I, ALU_OP_EXT, NOMEM, nullptr, {"t", "r", "+A", "+C"}, 0x7c000000, 0xfc00003f,
     .flags = IMF_SUPPORTED | IMF_REGWRITE | IMF_ALU_REQ_RS},
    IM_UNKNOWN,	//	1
    IM_UNKNOWN,	//	2
    IM_UNKNOWN,	//	3
    {"INS", IT_I, ALU_OP_EXT, NOMEM, nullptr, {"t", "r", "+A", "+B"}, 0x7c000004, 0xfc00003f,
     .flags = IMF_SUPPORTED | IMF_REGWRITE | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT},
    IM_UNKNOWN,	//	5
    IM_UNKNOWN,	//	6
    IM_UNKNOWN,	//	7
    IM_UNKNOWN,	//	8
    IM_UNKNOWN,	//	9
    IM_UNKNOWN,	//	10
    IM_UNKNOWN,	//	11
    IM_UNKNOWN,	//	12
    IM_UNKNOWN,	//	13
    IM_UNKNOWN,	//	14
    IM_UNKNOWN,	//	15
    IM_UNKNOWN,	//	16
    IM_UNKNOWN,	//	17
    IM_UNKNOWN,	//	18
    IM_UNKNOWN,	//	19
    IM_UNKNOWN,	//	20
    IM_UNKNOWN,	//	21
    IM_UNKNOWN,	//	22
    IM_UNKNOWN,	//	23
    IM_UNKNOWN,	//	24
    IM_UNKNOWN,	//	25
    IM_UNKNOWN,	//	26
    IM_UNKNOWN,	//	27
    IM_UNKNOWN,	//	28
    IM_UNKNOWN,	//	29
    IM_UNKNOWN,	//	30
    IM_UNKNOWN,	//	31
    {"BSHFL", IT_I, NOALU, NOMEM, bshfl_instruction_map, {}, 0, 0,
     .flags = IMF_SUB_ENCODE(5, 6)},
    IM_UNKNOWN,	//	33
    IM_UNKNOWN,	//	34
    IM_UNKNOWN,	//	35
    IM_UNKNOWN,	//	36
    IM_UNKNOWN,	//	37
    IM_UNKNOWN,	//	38
    IM_UNKNOWN,	//	39
    IM_UNKNOWN,	//	40
    IM_UNKNOWN,	//	41
    IM_UNKNOWN,	//	42
    IM_UNKNOWN,	//	43
    IM_UNKNOWN,	//	44
    IM_UNKNOWN,	//	45
    IM_UNKNOWN,	//	46
    IM_UNKNOWN,	//	47
    IM_UNKNOWN,	//	48
    IM_UNKNOWN,	//	49
    IM_UNKNOWN,	//	50
    IM_UNKNOWN,	//	51
    IM_UNKNOWN,	//	52
    IM_UNKNOWN,	//	53
    IM_UNKNOWN,	//	54
    IM_UNKNOWN,	//	55
    IM_UNKNOWN,	//	56
    IM_UNKNOWN,	//	57
    IM_UNKNOWN,	//	58
    {"RDHWR", IT_R, ALU_OP_RDHWR, NOMEM, nullptr, {"t", "K"}, 0x7c00003b, 0xffe007ff,
     .flags = IMF_SUPPORTED | IMF_REGWRITE},
    IM_UNKNOWN,	//	60
    IM_UNKNOWN,	//	61
    IM_UNKNOWN,	//	62
    IM_UNKNOWN,	//	63
};

static const struct InstructionMap  regimm_instruction_map[] = {
    {"BLTZ", IT_I, NOALU, NOMEM, nullptr, {"s", "p"}, 0x04000000, 0xfc1f0000,       // BLTZ
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH},
    {"BGEZ", IT_I, NOALU, NOMEM, nullptr, {"s", "p"}, 0x04010000, 0xfc1f0000,       // BGEZ
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_BJ_NOT},
    {"BLTZL", IT_I, NOALU, NOMEM, nullptr, {"s", "p"}, 0x04020000, 0xfc1f0000,       // BLTZL
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_NB_SKIP_DS},
    {"BGEZL", IT_I, NOALU, NOMEM, nullptr, {"s", "p"}, 0x04030000, 0xfc1f0000,       // BGEZL
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_NB_SKIP_DS | IMF_BJ_NOT},
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"TGEI", IT_I, ALU_OP_TGE, NOMEM, nullptr, {"s", "j"}, 0x04080000, 0xfc1f0000, // TGEI 16
     .flags = FLAGS_ALU_TRAP_SI},
    {"TGEIU", IT_I, ALU_OP_TGEU, NOMEM, nullptr, {"s", "j"}, 0x04090000, 0xfc1f0000, // TGEIU 17
     .flags = FLAGS_ALU_TRAP_SI},
    {"TLTI", IT_I, ALU_OP_TLT, NOMEM, nullptr, {"s", "j"}, 0x040a0000, 0xfc1f0000, // TLTI 18
     .flags = FLAGS_ALU_TRAP_SI},
    {"TLTIU", IT_I, ALU_OP_TGEU, NOMEM, nullptr, {"s", "j"}, 0x040b0000, 0xfc1f0000, // TLTIU 19
     .flags = FLAGS_ALU_TRAP_SI},
    {"TEQI", IT_I, ALU_OP_TEQ, NOMEM, nullptr, {"s", "j"}, 0x040c0000, 0xfc1f0000, // TEQI 20
     .flags = FLAGS_ALU_TRAP_SI},
    IM_UNKNOWN, // 21
    {"TNEI", IT_I, ALU_OP_TNE, NOMEM, nullptr, {"s", "j"}, 0x040e0000, 0xfc1f0000, // TNEI 22
     .flags = FLAGS_ALU_TRAP_SI},
    IM_UNKNOWN, // 23
    {"BLTZAL", IT_I, ALU_OP_PASS_T, NOMEM, nullptr, {"s", "p"}, 0x04100000, 0xfc1f0000, // BLTZAL
     .flags = FLAGS_J_B_PC_TO_R31 | IMF_BJR_REQ_RS | IMF_BRANCH},
    {"BGEZAL", IT_I, ALU_OP_PASS_T, NOMEM, nullptr, {"s", "p"}, 0x04110000, 0xfc1f0000, // BGEZAL
     .flags = FLAGS_J_B_PC_TO_R31 | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_BJ_NOT},
    {"BLTZALL", IT_I, ALU_OP_PASS_T, NOMEM, nullptr, {"s", "p"}, 0x04120000, 0xfc1f0000, // BLTZALL
     .flags = FLAGS_J_B_PC_TO_R31 | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_NB_SKIP_DS },
    {"BGEZALL", IT_I, ALU_OP_PASS_T, NOMEM, nullptr, {"s", "p"}, 0x04130000, 0xfc1f0000, // BGEZALL
     .flags = FLAGS_J_B_PC_TO_R31 | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_NB_SKIP_DS | IMF_BJ_NOT},
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"SYNCI", IT_I,  ALU_OP_ADDU, AC_CACHE_OP, nullptr, {"o(b)"}, 0x041f0000, 0xfc1f0000, // SYNCI
     .flags = IMF_SUPPORTED | IMF_STOP_IF | IMF_BJR_REQ_RS},
};

static const struct InstructionMap  cop0_func_instruction_map[] = {
    IM_UNKNOWN,	//	0
    IM_UNKNOWN,	//	1
    IM_UNKNOWN,	//	2
    IM_UNKNOWN,	//	3
    IM_UNKNOWN,	//	4
    IM_UNKNOWN,	//	5
    IM_UNKNOWN,	//	6
    IM_UNKNOWN,	//	7
    IM_UNKNOWN,	//	8
    IM_UNKNOWN,	//	9
    IM_UNKNOWN,	//	10
    IM_UNKNOWN,	//	11
    IM_UNKNOWN,	//	12
    IM_UNKNOWN,	//	13
    IM_UNKNOWN,	//	14
    IM_UNKNOWN,	//	15
    IM_UNKNOWN,	//	16
    IM_UNKNOWN,	//	17
    IM_UNKNOWN,	//	18
    IM_UNKNOWN,	//	19
    IM_UNKNOWN,	//	20
    IM_UNKNOWN,	//	21
    IM_UNKNOWN,	//	22
    IM_UNKNOWN,	//	23
    {"ERET", IT_I, ALU_OP_ERET, NOMEM, nullptr, {nullptr}, 0x42000018, 0xffffffff,
     .flags = IMF_SUPPORTED | IMF_STOP_IF},
    IM_UNKNOWN,	//	25
    IM_UNKNOWN,	//	26
    IM_UNKNOWN,	//	27
    IM_UNKNOWN,	//	28
    IM_UNKNOWN,	//	29
    IM_UNKNOWN,	//	30
    IM_UNKNOWN,	//	31
    IM_UNKNOWN,	//	32
    IM_UNKNOWN,	//	33
    IM_UNKNOWN,	//	34
    IM_UNKNOWN,	//	35
    IM_UNKNOWN,	//	36
    IM_UNKNOWN,	//	37
    IM_UNKNOWN,	//	38
    IM_UNKNOWN,	//	39
    IM_UNKNOWN,	//	40
    IM_UNKNOWN,	//	41
    IM_UNKNOWN,	//	42
    IM_UNKNOWN,	//	43
    IM_UNKNOWN,	//	44
    IM_UNKNOWN,	//	45
    IM_UNKNOWN,	//	46
    IM_UNKNOWN,	//	47
    IM_UNKNOWN,	//	48
    IM_UNKNOWN,	//	49
    IM_UNKNOWN,	//	50
    IM_UNKNOWN,	//	51
    IM_UNKNOWN,	//	52
    IM_UNKNOWN,	//	53
    IM_UNKNOWN,	//	54
    IM_UNKNOWN,	//	55
    IM_UNKNOWN,	//	56
    IM_UNKNOWN,	//	57
    IM_UNKNOWN,	//	58
    IM_UNKNOWN,	//	59
    IM_UNKNOWN,	//	60
    IM_UNKNOWN,	//	61
    IM_UNKNOWN,	//	62
    IM_UNKNOWN,	//	63
};

static const struct InstructionMap  cop0_instruction_map[] = {
    {"MFC0", IT_I, ALU_OP_MFC0, NOMEM, nullptr, {"t", "G", "H"}, 0x40000000, 0xffe007f8,
     .flags = IMF_SUPPORTED | IMF_REGWRITE},
    IM_UNKNOWN,	//	1
    IM_UNKNOWN,	//	2 MFH
    IM_UNKNOWN,	//	3
    {"MTC0", IT_I, ALU_OP_MTC0, NOMEM, nullptr, {"t", "G", "H"}, 0x40800000, 0xffe007f8,
     .flags = IMF_SUPPORTED | IMF_ALU_REQ_RT},
    IM_UNKNOWN,	//	5
    IM_UNKNOWN,	//	6 MTH
    IM_UNKNOWN,	//	7
    IM_UNKNOWN,	//	8
    IM_UNKNOWN,	//	9
    IM_UNKNOWN,	//  10 RDPGPR
    {"MFMC0", IT_I, ALU_OP_MFMC0, NOMEM, nullptr, {"t"}, 0x41600000, 0xffe0ffdf, // TODO
     .flags = IMF_SUPPORTED | IMF_REGWRITE},
    IM_UNKNOWN,	//	12
    IM_UNKNOWN,	//	13
    IM_UNKNOWN,	//	13 WRPGPR
    IM_UNKNOWN,	//	15
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"C0", IT_I, NOALU, NOMEM, cop0_func_instruction_map, {"C"}, 0x42000000, 0xfe000000,
     .flags = IMF_SUB_ENCODE(6, 0)},
};

const std::int32_t instruction_map_opcode_field = IMF_SUB_ENCODE(6, 26);

// This table is indexed by opcode
static const struct InstructionMap instruction_map[] = {
    {"ALU",    IT_R, NOALU, NOMEM, alu_instruction_map, {}, 0, 0,       // Alu operations
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"REGIMM", IT_I, NOALU, NOMEM, regimm_instruction_map, {}, 0, 0,    // REGIMM (BLTZ, nullptr, 0, 0 BGEZ)
     .flags = IMF_SUB_ENCODE(5, 16)},
    {"J",      IT_J, NOALU, NOMEM, nullptr, {"a"}, 0x08000000, 0xfc000000,         // J
     .flags = IMF_SUPPORTED | IMF_JUMP},
    {"JAL",    IT_J, ALU_OP_PASS_T, NOMEM, nullptr, {"a"}, 0x0c000000, 0xfc000000,  // JAL
     .flags = FLAGS_J_B_PC_TO_R31 | IMF_JUMP},
    {"BEQ",    IT_I, NOALU, NOMEM, nullptr, {"s", "t", "p"}, 0x10000000, 0xfc000000,         // BEQ
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BJR_REQ_RT | IMF_BRANCH},
    {"BNE",    IT_I, NOALU, NOMEM, nullptr, {"s", "t", "p"}, 0x14000000, 0xfc000000,          // BNE
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BJR_REQ_RT | IMF_BRANCH | IMF_BJ_NOT},
    {"BLEZ",   IT_I, NOALU, NOMEM, nullptr, {"s", "p"}, 0x18000000, 0xfc1f0000,          // BLEZ
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_BGTZ_BLEZ},
    {"BGTZ",   IT_I, NOALU, NOMEM, nullptr, {"s", "p"}, 0x1c000000, 0xfc1f0000,          // BGTZ
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_BGTZ_BLEZ | IMF_BJ_NOT},
    {"ADDI",   IT_I, ALU_OP_ADD, NOMEM, nullptr, {"t", "r", "j"}, 0x20000000, 0xfc000000,     // ADDI
     .flags = FLAGS_ALU_I},
    {"ADDIU",  IT_I, ALU_OP_ADDU, NOMEM, nullptr, {"t", "r", "j"}, 0x24000000, 0xfc000000,    // ADDIU
     .flags = FLAGS_ALU_I},
    {"SLTI",   IT_I, ALU_OP_SLT, NOMEM, nullptr, {"t", "r", "j"}, 0x28000000, 0xfc000000,     // SLTI
     .flags = FLAGS_ALU_I},
    {"SLTIU",  IT_I, ALU_OP_SLTU, NOMEM, nullptr, {"t", "r", "j"}, 0x2c000000, 0xfc000000,    // SLTIU
     .flags = FLAGS_ALU_I},
    {"ANDI",   IT_I, ALU_OP_AND, NOMEM, nullptr, {"t", "r", "i"}, 0x30000000, 0xfc000000,     // ANDI
     .flags = FLAGS_ALU_I_ZE},
    {"ORI",    IT_I, ALU_OP_OR, NOMEM, nullptr, {"t", "r", "i"}, 0x34000000, 0xfc000000,      // ORI
     .flags = FLAGS_ALU_I_ZE},
    {"XORI",   IT_I, ALU_OP_XOR, NOMEM, nullptr, {"t", "r", "i"}, 0x38000000, 0xfc000000,     // XORI
     .flags = FLAGS_ALU_I_ZE},
    {"LUI",    IT_I, ALU_OP_LUI, NOMEM, nullptr, {"t", "u"}, 0x3c000000, 0xffe00000,     // LUI
     .flags = FLAGS_ALU_I_NO_RS},
    {"COP0",   IT_I, NOALU, NOMEM, cop0_instruction_map, {"C"}, 0x00000000, 0x00000000, // COP0
     .flags = IMF_SUB_ENCODE(5, 21)},
    IM_UNKNOWN,  // 17
    IM_UNKNOWN,  // 18
    IM_UNKNOWN,  // 19
    {"BEQL",    IT_I, NOALU, NOMEM, nullptr, {"s", "t", "p"}, 0x50000000, 0xfc000000,         // BEQL
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BJR_REQ_RT | IMF_BRANCH | IMF_NB_SKIP_DS},
    {"BNEL",    IT_I, NOALU, NOMEM, nullptr, {"s", "t", "p"}, 0x54000000, 0xfc000000,         // BNEL
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BJR_REQ_RT | IMF_BRANCH | IMF_NB_SKIP_DS | IMF_BJ_NOT},
    {"BLEZL",   IT_I, NOALU, NOMEM, nullptr, {"s", "p"}, 0x58000000, 0xfc1f0000,         // BLEZL
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_NB_SKIP_DS | IMF_BGTZ_BLEZ},
    {"BGTZL",   IT_I, NOALU, NOMEM, nullptr, {"s", "p"}, 0x5c000000, 0xfc1f0000,         // BGTZL
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_NB_SKIP_DS | IMF_BGTZ_BLEZ | IMF_BJ_NOT},
    IM_UNKNOWN,  // 24
    IM_UNKNOWN,  // 25
    IM_UNKNOWN,  // 26
    IM_UNKNOWN,  // 27
    {"REGIMM", IT_I, NOALU, NOMEM, special2_instruction_map, {}, 0, 0,  // SPECIAL2
    .flags = IMF_SUB_ENCODE(6, 0)},
    IM_UNKNOWN,  // 29
    IM_UNKNOWN,  // 30
    {"SPECIAL3", IT_R, NOALU, NOMEM, special3_instruction_map, {}, 0, 0, //
    .flags = IMF_SUB_ENCODE(6, 0)},
    {"LB",     IT_I, ALU_OP_ADDU, AC_BYTE, nullptr, {"t", "o(b)"}, 0x80000000, 0xfc000000,  // LB
     .flags = FLAGS_ALU_I_LOAD},
    {"LH",     IT_I, ALU_OP_ADDU, AC_HALFWORD, nullptr, {"t", "o(b)"}, 0x84000000, 0xfc000000,  // LH
     .flags = FLAGS_ALU_I_LOAD},
    {"LWL",    IT_I, ALU_OP_ADDU, AC_WORD_LEFT, nullptr, {"t", "o(b)"}, 0x88000000, 0xfc000000,    // LWL - unsupported
     .flags = FLAGS_ALU_I_LOAD | IMF_ALU_REQ_RT},
    {"LW",     IT_I, ALU_OP_ADDU, AC_WORD, nullptr, {"t", "o(b)"}, 0x8c000000, 0xfc000000,  // LW
     .flags = FLAGS_ALU_I_LOAD},
    {"LBU",    IT_I, ALU_OP_ADDU, AC_BYTE_UNSIGNED, nullptr, {"t", "o(b)"}, 0x90000000, 0xfc000000,  // LBU
     .flags = FLAGS_ALU_I_LOAD},
    {"LHU",    IT_I, ALU_OP_ADDU, AC_HALFWORD_UNSIGNED, nullptr, {"t", "o(b)"}, 0x94000000, 0xfc000000,   // LHU
     .flags = FLAGS_ALU_I_LOAD},
    {"LWR",    IT_I, ALU_OP_ADDU, AC_WORD_RIGHT, nullptr, {"t", "o(b)"}, 0x98000000, 0xfc000000,    // LWR - unsupported
     .flags = FLAGS_ALU_I_LOAD | IMF_ALU_REQ_RT},
    IM_UNKNOWN,  // 39
    {"SB",     IT_I, ALU_OP_ADDU, AC_BYTE, nullptr, {"t", "o(b)"}, 0xa0000000, 0xfc000000,  // SB
     .flags = FLAGS_ALU_I_STORE},
    {"SH",     IT_I, ALU_OP_ADDU, AC_HALFWORD, nullptr, {"t", "o(b)"}, 0xa4000000, 0xfc000000,   // SH
     .flags = FLAGS_ALU_I_STORE},
    {"SWL",    IT_I, ALU_OP_ADDU, AC_WORD_LEFT, nullptr, {"t", "o(b)"}, 0xa8000000, 0xfc000000,    // SWL
     .flags = FLAGS_ALU_I_STORE},
    {"SW",     IT_I, ALU_OP_ADDU, AC_WORD, nullptr, {"t", "o(b)"}, 0xac000000, 0xfc000000,  // SW
     .flags = FLAGS_ALU_I_STORE},
    IM_UNKNOWN,  // 44
    IM_UNKNOWN,  // 45
    {"SWR",    IT_I, ALU_OP_ADDU, AC_WORD_RIGHT, nullptr, {"t", "o(b)"}, 0xb8000000, 0xfc000000,    // SWR
     .flags = FLAGS_ALU_I_STORE},
    {"CACHE",  IT_I, ALU_OP_ADDU, AC_CACHE_OP, nullptr, {"k", "o(b)"}, 0xbc000000, 0xfc000000, // CACHE
     .flags = IMF_SUPPORTED | IMF_ALUSRC | IMF_MEM},
    {"LL",     IT_I, ALU_OP_ADDU, AC_LOAD_LINKED, nullptr, {"t", "o(b)"}, 0xc0000000, 0xfc000000,  // LL
     .flags = FLAGS_ALU_I_LOAD},
    {"LWC1", IT_I, NOALU, NOMEM, nullptr, {"T", "o(b)"}, 0xc4000000, 0xfc000000,
     .flags = IMF_SUPPORTED},
    IM_UNKNOWN,  // 50
    {"PREF", IT_I, NOALU, NOMEM, nullptr, {"k", "o(b)"}, 0xcc000000, 0xfc000000,            // PREF
     .flags = IMF_SUPPORTED},
    IM_UNKNOWN,  // 52
    {"LWD1", IT_I, NOALU, NOMEM, nullptr, {"T", "o(b)"}, 0xd4000000, 0xfc000000,
     .flags = IMF_SUPPORTED},
    IM_UNKNOWN,  // 54
    IM_UNKNOWN,  // 55
    {"SC",     IT_I, ALU_OP_ADDU, AC_STORE_CONDITIONAL, nullptr, {"t", "o(b)"}, 0xe0000000, 0xfc000000,  // SW
     .flags = FLAGS_ALU_I_STORE | IMF_MEMREAD | IMF_REGWRITE},
    {"SWC1", IT_I, NOALU, NOMEM, nullptr, {"T", "o(b)"}, 0xe4000000, 0xfc000000,
     .flags = IMF_SUPPORTED},
    IM_UNKNOWN,  // 58
    IM_UNKNOWN,  // 59
    IM_UNKNOWN,  // 60
    {"SDC1", IT_I, NOALU, NOMEM, nullptr, {"T", "o(b)"}, 0xf4000000, 0xfc000000,
     .flags = IMF_SUPPORTED},
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
    return (std::uint8_t) MASK(5, RS_SHIFT);
}

std::uint8_t Instruction::rt() const {
    return (std::uint8_t) MASK(5, RT_SHIFT);
}

std::uint8_t Instruction::rd() const {
    return (std::uint8_t) MASK(5, RD_SHIFT);
}

std::uint8_t Instruction::shamt() const {
    return (std::uint8_t) MASK(5, SHAMT_SHIFT);

}

std::uint8_t Instruction::funct() const {
    return (std::uint8_t) MASK(6, 0);
}

std::uint8_t Instruction::cop0sel() const {
    return (std::uint8_t) MASK(3, 0);
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
   #if 1
    if ((dt ^ im.code) & (im.mask))
        flags = (enum InstructionFlags)(flags & ~IMF_SUPPORTED);
   #endif
}

enum ExceptionCause Instruction::encoded_exception() const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    if (!(im.flags & IMF_EXCEPTION))
        return EXCAUSE_NONE;
    switch (im.alu) {
        case ALU_OP_BREAK:
            return EXCAUSE_BREAK;
        case ALU_OP_SYSCALL:
            return EXCAUSE_SYSCALL;
        default:
            return EXCAUSE_NONE;
    }
}

bool Instruction::is_break() const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    return im.alu == ALU_OP_BREAK;
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

QString Instruction::to_str(std::int32_t inst_addr) const {
    const InstructionMap &im = InstructionMapFind(dt);
    // TODO there are exception where some fields are zero and such so we should not print them in such case
    if (dt == 0)
        return QString("NOP");
    SANITY_ASSERT(argdesbycode_filled, QString("argdesbycode_filled not initialized"));
    QString res;
    QString next_delim = " ";
    if (im.type == T_UNKNOWN)
        return QString("UNKNOWN");

    res += im.name;
    foreach (const QString &arg, im.args) {
        res += next_delim;
        next_delim = ", ";
        foreach (QChar ao, arg) {
            uint a = ao.toLatin1();
            if (!a)
                continue;
            const ArgumentDesc *adesc = argdesbycode[a];
            if (adesc == nullptr) {
               res += ao;
               continue;
            }
            uint bits = IMF_SUB_GET_BITS(adesc->loc);
            uint shift = IMF_SUB_GET_SHIFT(adesc->loc);
            std::uint32_t field = MASK(bits, shift);
            if ((adesc->min < 0) && (field & (1 << (bits - 1))))
                field -= 1 << bits;

            field <<= adesc->shift;
            switch (adesc->kind) {
            case 'g':
                res += "$" + QString::number(field);
                break;
            case 'o':
            case 'n':
                if (adesc->min < 0)
                    res += QString::number((std::int32_t)field, 10).toUpper();
                else
                    res += "0x" + QString::number(field, 16).toUpper();
                break;
            case 'p':
                field += inst_addr + 4;
                res += "0x" + QString::number((std::uint32_t)field, 16).toUpper();
                break;
            case 'a':
                std::uint32_t target = (inst_addr & 0xF0000000) | (address() << 2);
                res += " 0x" + QString::number(target, 16).toUpper();
                break;
            }
        }
    }
    return res;
}

QMultiMap<QString, std::uint32_t> str_to_instruction_code_map;

void instruction_from_string_build_base(const InstructionMap *im = nullptr,
                    unsigned int flags = 0, std::uint32_t base_code = 0) {
    std::uint32_t code;

    if (im == nullptr) {
        im = instruction_map;
        flags = instruction_map_opcode_field;
        base_code = 0;
    }
    unsigned int bits = IMF_SUB_GET_BITS(flags);
    unsigned int shift = IMF_SUB_GET_SHIFT(flags);

    for (unsigned int i = 0; i < 1U << bits; i++, im++) {
        code = base_code | (i << shift);
        if (im->subclass) {
            instruction_from_string_build_base(im->subclass, im->flags, code);
            continue;
        }
        if (!(im->flags & IMF_SUPPORTED))
            continue;
        str_to_instruction_code_map.insert(im->name, code);
       #if 0
        if (im->code != code) {
            printf("code mitchmatch %s computed 0x%08x found 0x%08x\n", im->name, code, im->code);
        }
       #endif
    }
#if 0
    for (auto i = str_to_instruction_code_map.begin();
         i != str_to_instruction_code_map.end(); i++)
        std::cout << i.key().toStdString() << ' ';
#endif
}

static int parse_reg_from_string(QString str, uint *chars_taken = nullptr)
{
    int res;
    int i;
    uint ctk;
    if (str.count() < 2 || str.at(0) != '$')
        return -1;
    char cstr[str.count() + 1];
    for (i = 0; i < str.count(); i++)
        cstr[i] = str.at(i).toLatin1();
    cstr[i] = 0;
    const char *p = cstr + 1;
    char *r;
    res = std::strtol(p, &r, 0);
    ctk = r - p + 1;
    if (p == r)
        return -1;
    if (res > 31)
        return -1;
    if (chars_taken != nullptr)
        *chars_taken = ctk;
    return res;
}

static void reloc_append(RelocExpressionList *reloc, QString fl, uint32_t inst_addr,
                    std::int64_t offset, const ArgumentDesc *adesc, uint *chars_taken = nullptr,
                    int line = 0, int options = 0) {
    uint bits = IMF_SUB_GET_BITS(adesc->loc);
    uint shift = IMF_SUB_GET_SHIFT(adesc->loc);
    QString expression = "";
    QString allowed_operators = "+-/*|&^~";
    int i = 0;
    for (; i < fl.size(); i++) {
        QChar ch = fl.at(i);
        if (ch.isSpace())
            continue;
        if (ch.isLetterOrNumber())
            expression.append(ch);
        else if (allowed_operators.indexOf(ch) >= 0)
            expression.append(ch);
        else
            break;
    }

    reloc->append(new RelocExpression(inst_addr, expression, offset,
                  adesc->min, adesc->max, shift, bits, adesc->shift, line, options));
    if (chars_taken != nullptr) {
        *chars_taken = i;
    }
}

#define CFS_OPTION_SILENT_MASK 0x100

ssize_t Instruction::code_from_string(std::uint32_t *code, size_t buffsize,
                       QString inst_base, QVector<QString> &inst_fields,
                       std::uint32_t inst_addr, RelocExpressionList *reloc,
                       int line, bool pseudo_opt, int options)
{
    if (str_to_instruction_code_map.isEmpty())
        instruction_from_string_build_base();

    int field = 0;
    std::uint32_t inst_code = 0;
    auto i = str_to_instruction_code_map.lowerBound(inst_base);
    for (; ; i++) {
        if (i == str_to_instruction_code_map.end())
            break;
        if (i.key() != inst_base)
            break;
        inst_code = i.value();
        const InstructionMap &im = InstructionMapFind(inst_code);

        field = 0;
        foreach (const QString &arg, im.args) {
            if (field >= inst_fields.count()) {
                field = -1;
                break;
            }
            QString fl = inst_fields.at(field++);
            foreach (QChar ao, arg) {
                bool need_reloc = false;
                const char *p;
                char *r;
                uint a = ao.toLatin1();
                if (!a)
                    continue;
                fl = fl.trimmed();
                const ArgumentDesc *adesc = argdesbycode[a];
                if (adesc == nullptr) {
                   if (!fl.count()) {
                       field = -1;
                       break;
                   }
                   if (fl.at(0) != ao) {
                       field = -1;
                       break;
                   }
                   fl = fl.mid(1);
                   continue;
                }
                uint bits = IMF_SUB_GET_BITS(adesc->loc);
                uint shift = IMF_SUB_GET_SHIFT(adesc->loc);
                int shift_right = adesc->shift;
                std::uint64_t val = 0;
                uint chars_taken = 0;

                // if ((adesc->min < 0) && (field & (1 << bits - 1)))
                //    field -= 1 << bits;
                // field <<= adesc->shift;

                switch (adesc->kind) {
                case 'g':
                    val += parse_reg_from_string(fl, &chars_taken);
                    break;
                case 'p':
                    val -= (inst_addr + 4);
                    FALLTROUGH
                case 'o':
                case 'n':
                    shift_right += options & 0xff;
                    if(fl.at(0).isDigit() || (reloc == nullptr)) {
                        std::uint64_t num_val;
                        int i;
                        // Qt functions are limited, toLongLong would be usable
                        // but does not return information how many characters
                        // are processed. Used solution has horrible overhead
                        // but is usable for now
                        char cstr[fl.count() + 1];
                        for (i = 0; i < fl.count(); i++)
                            cstr[i] = fl.at(i).toLatin1();
                        cstr[i] = 0;
                        p = cstr;
                        if (adesc->min < 0)
                            num_val = std::strtoll(p, &r, 0);
                        else
                            num_val = std::strtoull(p, &r, 0);
                        while(*r && std::isspace(*r))
                            r++;
                        if (*r && std::strchr("+-/*|&^~", *r))
                            need_reloc = true;
                        else
                            val += num_val;
                        chars_taken = r - p;
                    } else {
                        need_reloc = true;
                    }
                    if (need_reloc && (reloc != nullptr)) {
                        reloc_append(reloc, fl, inst_addr, val, adesc, &chars_taken, line, options);
                        val = 0;
                    }
                    break;
                case 'a':
                    shift_right += options & 0xff;
                    val -= (inst_addr + 4) & 0xf0000000;
                    if(fl.at(0).isDigit() || (reloc == nullptr)) {
                        std::uint64_t num_val;
                        int i;
                        char cstr[fl.count() + 1];
                        for (i = 0; i < fl.count(); i++)
                            cstr[i] = fl.at(i).toLatin1();
                        cstr[i] = 0;
                        p = cstr;
                        num_val = std::strtoull(p, &r, 0);
                        while(*r && std::isspace(*r))
                            r++;
                        if (*r && std::strchr("+-/*|&^~", *r))
                            need_reloc = true;
                        else
                            val += num_val;
                        chars_taken = r - p;
                    } else {
                        need_reloc = true;
                    }
                    if (need_reloc && (reloc != nullptr)) {
                        reloc_append(reloc, fl, val, inst_addr, adesc, &chars_taken, line, options);
                        val = 0;
                    }
                    break;
                }
                if (chars_taken <= 0) {
                    field = -1;
                    break;
                }
                if ((val & ((1 << adesc->shift) - 1)) &&
                    !(options & CFS_OPTION_SILENT_MASK)) {
                    field = -1;
                    break;
                }
                if (adesc->min >= 0)
                   val = (val >> shift_right) ;
                else
                    val = (std::uint64_t)((std::int64_t)val >> shift_right) ;
                if (!(options & CFS_OPTION_SILENT_MASK)) {
                    if (adesc->min < 0) {
                        if (((std::int64_t)val < adesc->min) ||
                            ((std::int64_t)val > adesc->max)) {
                            field = -1;
                            break;
                        }
                    } else {
                        if ((val < (std::uint64_t)adesc->min) ||
                            (val > (std::uint64_t)adesc->max)) {
                            field = -1;
                            break;
                        }
                    }
                }
                val = (val & ((1 << bits) - 1)) << shift;
                inst_code += val;
                fl = fl.mid(chars_taken);
            }
            if (field == -1)
                break;
        }
        if (field != inst_fields.count())
            continue;

        if (buffsize >= 4)
            *code = inst_code;
        return 4;
    }

    ssize_t ret = -1;
    inst_code = 0;
    if ((inst_base == "NOP") && (inst_fields.size() == 0)) {
        inst_code = 0;
        ret = 4;
    } else if (pseudo_opt) {
        if (((inst_base == "LA") || (inst_base == "LI")) && (inst_fields.size() == 2)) {
            if(code_from_string(code, buffsize, "LUI", inst_fields,
                             inst_addr, reloc, line, false,
                             CFS_OPTION_SILENT_MASK + 16) < 0)
                return -1;
            inst_fields.insert(1, "$0");
            if (code_from_string(code + 1, buffsize - 4, "ORI", inst_fields,
                             inst_addr + 4, reloc, line, false,
                             CFS_OPTION_SILENT_MASK + 0) < 0)
                return -1;
            return 8;
        }
    }
    if (buffsize >= 4)
        *code = inst_code;
    return ret;
}

ssize_t Instruction::code_from_string(std::uint32_t *code, size_t buffsize,
                       QString str, std::uint32_t inst_addr,
                       RelocExpressionList *reloc, int line, bool pseudo_opt, int options)
{
    if (str_to_instruction_code_map.isEmpty())
        instruction_from_string_build_base();

    QString inst_base = "";
    QVector<QString> inst_fields(0);
    bool prev_white = true;
    bool act_white;
    bool comma = false;
    bool next_comma = false;
    int field = 0;
    bool error = false;

    for (int k = 0, l = 0; k < str.count() + 1; k++, prev_white = act_white) {
        if (next_comma)
            comma = true;
        next_comma = false;
        if (k >= str.count()) {
            act_white = true;
        } else {
            act_white = str.at(k).isSpace();
            if (str.at(k) == ',')
                next_comma = act_white = true;
        }

        if (prev_white and !act_white)
            l = k;
        if (!prev_white and act_white) {
            if (inst_base.count() == 0) {
                if (comma) {
                    error = true;
                    break;
                }
                inst_base = str.mid(l, k - l).toUpper();
            } else {
                if ((field && !comma) || (!field && comma)) {
                    error = true;
                    break;
                }
                inst_fields.append(str.mid(l, k - l));
                comma = false;
                field++;
            }
            l = k;
        }
    }

    if (error)
         return -1;

    return code_from_string(code, buffsize, inst_base, inst_fields, inst_addr,
                            reloc, line, pseudo_opt, options);
}

bool Instruction::update(std::int64_t val, RelocExpression *relocexp) {
    std::int64_t mask = (((std::int64_t)1 << relocexp->bits) - 1) << relocexp->lsb_bit;
    dt &= ~ mask;
    val += relocexp->offset;
    if ((val & ((1 << relocexp->shift) - 1)) &&
        !(relocexp->options & CFS_OPTION_SILENT_MASK)) {
        return false;
    }
    int shift_right = relocexp->shift + (relocexp->options & 0xff);
    if (relocexp->min >= 0)
        val = (val >> shift_right) ;
    else
        val = (std::uint64_t)((std::int64_t)val >> shift_right);
    if (!(relocexp->options & CFS_OPTION_SILENT_MASK)) {
        if (relocexp->min < 0) {
            if (((std::int64_t)val < relocexp->min) ||
                ((std::int64_t)val > relocexp->max)) {
                return false;
            }
        } else {
            if (((std::uint64_t)val < (std::uint64_t)relocexp->min) ||
                ((std::uint64_t)val > (std::uint64_t)relocexp->max)) {
                return false;
            }
        }
    }
    dt |= (val << relocexp->lsb_bit) & mask;
    return true;
}

void Instruction::append_recognized_instructions(QStringList &list) {
    if (str_to_instruction_code_map.isEmpty())
        instruction_from_string_build_base();

    foreach (const QString &str, str_to_instruction_code_map.keys())
        list.append(str);
    list.append("LA");
    list.append("LI");
    list.append("NOP");
}
