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
#include "instruction.h"
#include "alu.h"
#include "memory.h"
#include "qtmipsexception.h"

using namespace machine;

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

#define FLAGS_J_B_PC_TO_R31 (IMF_SUPPORTED | IMF_PC_TO_R31 | IMF_REGWRITE)

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
     .flags = FLAGS_ALU_T_R_TD_SHAMT},
    IM_UNKNOWN,
    {"SRL",    IT_R, ALU_OP_SRL, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_TD_SHAMT},
    {"SRA",    IT_R, ALU_OP_SRA, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_TD_SHAMT},
    {"SLLV",   IT_R, ALU_OP_SLLV, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD_SHV},
    IM_UNKNOWN,
    {"SRLV",   IT_R, ALU_OP_SRLV, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD_SHV},
    {"SRAV",   IT_R, ALU_OP_SRAV, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD_SHV},
    {"JR",     IT_R, ALU_OP_NOP, NOMEM, nullptr,
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_JUMP},
    {"JALR",   IT_R, ALU_OP_PASS_T, NOMEM, nullptr,
     .flags = IMF_SUPPORTED | IMF_REGD | IMF_REGWRITE | IMF_BJR_REQ_RS | IMF_PC8_TO_RT | IMF_JUMP},
    {"MOVZ",   IT_R, ALU_OP_MOVZ, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
    {"MOVN",   IT_R, ALU_OP_MOVN, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},
    {"SYSCALL",IT_R, ALU_OP_SYSCALL, NOMEM, nullptr,
     .flags = IMF_SUPPORTED | IMF_EXCEPTION},
    {"BREAK",  IT_R, ALU_OP_BREAK, NOMEM, nullptr,
     .flags = IMF_SUPPORTED | IMF_EXCEPTION},
    IM_UNKNOWN,
    {"SYNC", IT_I, NOALU, NOMEM, nullptr,       // SYNC
     .flags = IMF_SUPPORTED},
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
    IM_UNKNOWN, // 44
    IM_UNKNOWN, // 45
    IM_UNKNOWN, // 46
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
    IM_UNKNOWN, // 62
    IM_UNKNOWN, // 63
};

static const struct InstructionMap  special2_instruction_map[] = {
    IM_UNKNOWN,	//	0
    IM_UNKNOWN,	//	1
    {"MUL",    IT_R, ALU_OP_MUL, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_STD},     // 32
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
    IM_UNKNOWN,	//	24
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

static const struct InstructionMap  special3_instruction_map[] = {
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
    IM_UNKNOWN,	//	24
    IM_UNKNOWN,	//	25
    IM_UNKNOWN,	//	26
    IM_UNKNOWN,	//	27
    IM_UNKNOWN,	//	28
    IM_UNKNOWN,	//	29
    IM_UNKNOWN,	//	30
    IM_UNKNOWN,	//	31
    {"BSHFL", IT_R, ALU_OP_BSHFL, NOMEM, nullptr,
     .flags = FLAGS_ALU_T_R_TD},
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
    {"RDHWR", IT_R, ALU_OP_RDHWR, NOMEM, nullptr,
     .flags = IMF_SUPPORTED | IMF_REGWRITE},
    IM_UNKNOWN,	//	60
    IM_UNKNOWN,	//	61
    IM_UNKNOWN,	//	62
    IM_UNKNOWN,	//	63
};

static const struct InstructionMap  regimm_instruction_map[] = {
    {"BLTZ", IT_I, NOALU, NOMEM, nullptr,       // BLTZ
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH},
    {"BGEZ", IT_I, NOALU, NOMEM, nullptr,       // BGEZ
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_BJ_NOT},
    {"BLTZL", IT_I, NOALU, NOMEM, nullptr,       // BLTZL
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_NB_SKIP_DS},
    {"BGEZL", IT_I, NOALU, NOMEM, nullptr,       // BGEZL
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_NB_SKIP_DS | IMF_BJ_NOT},
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"TGEI", IT_I, NOALU, NOMEM, nullptr,       // TGEI
     .flags = IMF_BJR_REQ_RS},
    {"TGEIU", IT_I, NOALU, NOMEM, nullptr,       // TGEIU
     .flags = IMF_BJR_REQ_RS},
    {"TLTI", IT_I, NOALU, NOMEM, nullptr,       // TLTI
     .flags = IMF_BJR_REQ_RS},
    {"TLTIU", IT_I, NOALU, NOMEM, nullptr,       // TLTIU
     .flags = IMF_BJR_REQ_RS},
    {"TEQI", IT_I, NOALU, NOMEM, nullptr,       // TEQI
     .flags = IMF_BJR_REQ_RS},
    IM_UNKNOWN,
    {"TNEI", IT_I, NOALU, NOMEM, nullptr,       // TNEI
     .flags = IMF_BJR_REQ_RS},
    IM_UNKNOWN,
    {"BLTZAL", IT_I, ALU_OP_PASS_T, NOMEM, nullptr, // BLTZAL
     .flags = FLAGS_J_B_PC_TO_R31 | IMF_BJR_REQ_RS | IMF_BRANCH},
    {"BGEZAL", IT_I, ALU_OP_PASS_T, NOMEM, nullptr, // BGEZAL
     .flags = FLAGS_J_B_PC_TO_R31 | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_BJ_NOT},
    {"BLTZALL", IT_I, ALU_OP_PASS_T, NOMEM, nullptr, // BLTZALL
     .flags = FLAGS_J_B_PC_TO_R31 | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_NB_SKIP_DS },
    {"BGEZALL", IT_I, ALU_OP_PASS_T, NOMEM, nullptr, // BGEZALL
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
    {"SYNCI", IT_I,  ALU_OP_ADDU, AC_CACHE_OP, nullptr, // SYNCI
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS},
};

const std::int32_t instruction_map_opcode_field = IMF_SUB_ENCODE(6, 26);

// This table is indexed by opcode
static const struct InstructionMap instruction_map[] = {
    {"ALU",    IT_R, NOALU, NOMEM, alu_instruction_map,          // Alu operations
     .flags = IMF_SUB_ENCODE(6, 0)},
    {"REGIMM", IT_I, NOALU, NOMEM, regimm_instruction_map,       // REGIMM (BLTZ, BGEZ)
     .flags = IMF_SUB_ENCODE(5, 16)},
    {"J",      IT_J, NOALU, NOMEM, nullptr,          // J
     .flags = IMF_SUPPORTED | IMF_JUMP},
    {"JAL",    IT_J, ALU_OP_PASS_T, NOMEM, nullptr,  // JAL
     .flags = FLAGS_J_B_PC_TO_R31 | IMF_JUMP},
    {"BEQ",    IT_I, NOALU, NOMEM, nullptr,         // BEQ
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BJR_REQ_RT | IMF_BRANCH},
    {"BNE",    IT_I, NOALU, NOMEM, nullptr,          // BNE
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BJR_REQ_RT | IMF_BRANCH | IMF_BJ_NOT},
    {"BLEZ",   IT_I, NOALU, NOMEM, nullptr,          // BLEZ
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_BGTZ_BLEZ},
    {"BGTZ",   IT_I, NOALU, NOMEM, nullptr,          // BGTZ
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_BGTZ_BLEZ | IMF_BJ_NOT},
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
     .flags = FLAGS_ALU_I_NO_RS},
    IM_UNKNOWN,  // 16
    IM_UNKNOWN,  // 17
    IM_UNKNOWN,  // 18
    IM_UNKNOWN,  // 19
    {"BEQL",    IT_I, NOALU, NOMEM, nullptr,         // BEQL
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BJR_REQ_RT | IMF_BRANCH | IMF_NB_SKIP_DS},
    {"BNEL",    IT_I, NOALU, NOMEM, nullptr,         // BNEL
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BJR_REQ_RT | IMF_BRANCH | IMF_NB_SKIP_DS | IMF_BJ_NOT},
    {"BLEZL",   IT_I, NOALU, NOMEM, nullptr,         // BLEZL
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_NB_SKIP_DS | IMF_BGTZ_BLEZ},
    {"BGTZL",   IT_I, NOALU, NOMEM, nullptr,         // BGTZL
     .flags = IMF_SUPPORTED | IMF_BJR_REQ_RS | IMF_BRANCH | IMF_NB_SKIP_DS | IMF_BGTZ_BLEZ | IMF_BJ_NOT},
    IM_UNKNOWN,  // 24
    IM_UNKNOWN,  // 25
    IM_UNKNOWN,  // 26
    IM_UNKNOWN,  // 27
    {"REGIMM", IT_I, NOALU, NOMEM, special2_instruction_map,  // SPECIAL2
    .flags = IMF_SUB_ENCODE(6, 0)},
    IM_UNKNOWN,  // 29
    IM_UNKNOWN,  // 30
    {"SPECIAL3", IT_R, NOALU, NOMEM, special3_instruction_map, //
    .flags = IMF_SUB_ENCODE(6, 0)},
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
     .flags = IMF_MEM | IMF_MEMWRITE},
    {"SW",     IT_I, ALU_OP_ADDU, AC_WORD, nullptr,  // SW
     .flags = FLAGS_ALU_I_STORE},
    IM_UNKNOWN,  // 44
    IM_UNKNOWN,  // 45
    {"SWR",    IT_I, ALU_OP_ADDU, NOMEM, nullptr,    // SWR
     .flags = IMF_MEM | IMF_MEMWRITE},
    {"CACHE",  IT_I, ALU_OP_ADDU, AC_CACHE_OP, nullptr, // CACHE
     .flags = IMF_SUPPORTED | IMF_ALUSRC | IMF_MEM},
    {"LL",     IT_I, ALU_OP_ADDU, AC_LOAD_LINKED, nullptr,  // LL
     .flags = FLAGS_ALU_I_LOAD},
    IM_UNKNOWN,  // 49
    IM_UNKNOWN,  // 50
    {"PREF", IT_I, NOALU, NOMEM, nullptr,            // PREF
     .flags = IMF_SUPPORTED},
    IM_UNKNOWN,  // 52
    IM_UNKNOWN,  // 53
    IM_UNKNOWN,  // 54
    IM_UNKNOWN,  // 55
    {"SC",     IT_I, ALU_OP_ADDU, AC_STORE_CONDITIONAL, nullptr,  // SW
     .flags = FLAGS_ALU_I_STORE | IMF_MEMREAD | IMF_REGWRITE},
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

#define RS_SHIFT 21
#define RT_SHIFT 16
#define RD_SHIFT 11
#define SHAMT_SHIFT 6

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
    QString res;
    QString next_delim = " ";
    switch (im.type) {
    case T_I:
        res += im.name;
        if (im.flags & IMF_MEM) {
            res += " $" + QString::number(rt());
            res += ", 0x" + QString::number(immediate(), 16).toUpper() + "($" + QString::number(rs()) + ")";
        } else {
            if (im.flags & IMF_REGWRITE) {
                res += next_delim + "$" + QString::number(rt());
                next_delim = ", ";
            }
            if (im.flags & (IMF_BJR_REQ_RS | IMF_ALU_REQ_RS)) {
                res += next_delim + "$" + QString::number(rs());
                next_delim = ", ";
            }
            if (im.flags & (IMF_BJR_REQ_RT | IMF_ALU_REQ_RT) && !(im.flags & IMF_REGWRITE)) {
                res += next_delim + "$" + QString::number(rt());
                next_delim = ", ";
            }
            if (im.flags & IMF_BRANCH) {
                std::uint32_t target = inst_addr + ((int16_t)immediate() << 2) + 4;
                res += next_delim + "0x" + QString::number(target, 16).toUpper();
            } else if (im.flags & IMF_ZERO_EXTEND) {
                res += next_delim + "0x" + QString::number(immediate(), 16).toUpper();
            } else {
                res += next_delim + QString::number((std::int16_t)immediate(), 10).toUpper();
            }
        }
        break;
    case T_J:
        res += im.name;
        {
            std::uint32_t target = (inst_addr & 0xF0000000) | (address() << 2);
            res += " 0x" + QString::number(target, 16).toUpper();
        }
        break;
    case T_R:
        {
        res += im.name;
        if (im.flags & IMF_REGD) {
            res += next_delim + "$" + QString::number(rd());
            next_delim = ", ";
        }
        if (!(im.flags & IMF_ALU_SHIFT)) {
            if (im.flags & (IMF_BJR_REQ_RS | IMF_ALU_REQ_RS)) {
                res += next_delim + "$" + QString::number(rs());
                next_delim = ", ";
            }
        }
        if (im.flags & (IMF_BJR_REQ_RT | IMF_ALU_REQ_RT)) {
            res += next_delim + "$" + QString::number(rt());
            next_delim = ", ";
        }
        if (im.flags & IMF_ALU_SHIFT) {
            if (im.flags & (IMF_BJR_REQ_RS | IMF_ALU_REQ_RS)) {
                res += next_delim + "$" + QString::number(rs());
                next_delim = ", ";
            } else {
                res += next_delim + QString::number(shamt());
                next_delim = ", ";
            }
        }
        if ((im.flags & IMF_REGWRITE) && !(im.flags & IMF_REGD)) {
            res += next_delim + "$" + QString::number(rd());
            next_delim = ", ";
        }
        break;
        }
	case T_UNKNOWN:
		return QString("UNKNOWN");
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
    }
}

static int parse_reg_from_string(QString str)
{
    bool ok;
    int res;
    if (str.count() < 2 || str.at(0) != '$')
        return -1;
    str = str.mid(1, str.count() - 1);
    res = str.toULong(&ok, 10);
    if (!ok)
        return -1;
    if (res > 31)
        return -1;
    return res;
}

Instruction Instruction::from_string(QString str, bool *pok) {
    std::uint32_t code;
    int val;
    bool ok = false;

    if (str_to_instruction_code_map.isEmpty())
        instruction_from_string_build_base();

    str = str.toUpper();

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
                inst_base = str.mid(l, k - l);
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

    if (error) {
        if (pok != nullptr)
            *pok = true;
        return Instruction(0);
    }

    auto i = str_to_instruction_code_map.lowerBound(inst_base);
    for (; ; i++) {
        if (i == str_to_instruction_code_map.end())
            break;
        if (i.key() != inst_base)
            break;
        code = i.value();
        const InstructionMap &im = InstructionMapFind(code);

        field = 0;
        switch (im.type) {
        case T_I:
            if (im.flags & IMF_MEM) {
                if (field >= inst_fields.count())
                    continue;
                if ((val = parse_reg_from_string(inst_fields.at(field++))) < 0)
                    continue;
                code |= val << RT_SHIFT;
                if (field >= inst_fields.count())
                    continue;
                int lpar = inst_fields.at(field).indexOf('(');
                int rpar = inst_fields.at(field).indexOf(')');
                if (lpar == -1  || rpar == -1)
                    continue;
                if ((val = parse_reg_from_string(inst_fields.
                    at(field).mid(lpar+1, rpar - lpar - 1))) < 0)
                    continue;
                code |= val << RS_SHIFT;

                val = inst_fields.at(field).mid(0, lpar).toLong(&ok, 0);
                if (!ok)
                    continue;
                ok = false;
                code |= val & 0xffff;
                field ++;
            } else {
                if (im.flags & IMF_REGWRITE) {
                    if (field >= inst_fields.count())
                        continue;
                    if ((val = parse_reg_from_string(inst_fields.at(field++))) < 0)
                        continue;
                    code |= val << RT_SHIFT;
                }
                if (im.flags & (IMF_BJR_REQ_RS | IMF_ALU_REQ_RS)) {
                    if (field >= inst_fields.count())
                        continue;
                    if ((val = parse_reg_from_string(inst_fields.at(field++))) < 0)
                        continue;
                    code |= val << RS_SHIFT;
                }
                if (im.flags & (IMF_BJR_REQ_RT | IMF_ALU_REQ_RT) && !(im.flags & IMF_REGWRITE)) {
                    if (field >= inst_fields.count())
                        continue;
                    if ((val = parse_reg_from_string(inst_fields.at(field++))) < 0)
                        continue;
                    code |= val << RT_SHIFT;
                }
                if (field >= inst_fields.count())
                    continue;
                val = inst_fields.at(field++).toLong(&ok, 0);
                if (!ok)
                    continue;
                ok = false;
                code |= val & 0xffff;
            }
            break;
        case T_J:
            if (field >= inst_fields.count())
                continue;
            val = inst_fields.at(field++).toLong(&ok, 0);
            code |= (val >> 2) & ~0xF0000000;
            break;
        case T_R:
            if (im.flags & IMF_REGD) {
                if (field >= inst_fields.count())
                    continue;
                if ((val = parse_reg_from_string(inst_fields.at(field++))) < 0)
                    continue;
                code |= val << RD_SHIFT;
            }
            if (!(im.flags & IMF_ALU_SHIFT)) {
                if (im.flags & (IMF_BJR_REQ_RS | IMF_ALU_REQ_RS)) {
                    if (field >= inst_fields.count())
                        continue;
                    if ((val = parse_reg_from_string(inst_fields.at(field++))) < 0)
                        continue;
                    code |= val << RS_SHIFT;
                }
            }
            if (im.flags & (IMF_BJR_REQ_RT | IMF_ALU_REQ_RT)) {
                if (field >= inst_fields.count())
                    continue;
                if ((val = parse_reg_from_string(inst_fields.at(field++))) < 0)
                    continue;
                code |= val << RT_SHIFT;
            }
            if (im.flags & IMF_ALU_SHIFT) {
                if (im.flags & (IMF_BJR_REQ_RS | IMF_ALU_REQ_RS)) {
                    if (field >= inst_fields.count())
                        continue;
                    if ((val = parse_reg_from_string(inst_fields.at(field++))) < 0)
                        continue;
                    code |= val << RS_SHIFT;
                } else {
                    if (field >= inst_fields.count())
                        continue;
                    val = inst_fields.at(field++).toLong(&ok, 0);
                    if (!ok)
                        continue;
                    ok = false;
                    code |= val << (SHAMT_SHIFT & 0x1f);
                }
            }
            if ((im.flags & IMF_REGWRITE) && !(im.flags & IMF_REGD)) {
                if (field >= inst_fields.count())
                    continue;
                if ((val = parse_reg_from_string(inst_fields.at(field++))) < 0)
                    continue;
                code |= val << RD_SHIFT;
            }
            break;
        case T_UNKNOWN:
            break;
        }

        if (field != inst_fields.count())
            continue;

        if (pok != nullptr)
            *pok = true;

        return Instruction(code);
    }

    if (str == "NOP")
        ok = true;
    if (pok != nullptr)
        *pok = ok;
    return Instruction(0);
}
