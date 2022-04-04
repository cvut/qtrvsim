#include "instruction.h"

#include "common/containers/constexpr/cstring.h"
#include "common/containers/lookuptable.h"
#include "common/logging.h"
#include "common/math/bit_ops.h"
#include "execute/alu.h"
#include "instruction/argument_desc.h"
#include "instruction/instruction_flags.h"
#include "instruction/riscv/register_names.h"
#include "register_value.h"

#include <QChar>
#include <QMultiMap>
#include <cctype>
#include <cstring>

LOG_CATEGORY("instruction");

namespace machine {

using std::array;

namespace non_arg_fields {
    static constexpr InstructionField OPCODE = { { { 7, 0 } }, 0 };
    static constexpr InstructionField FUNCT7 = { { { 7, 25 } }, 0 };
    static constexpr InstructionField FUNCT3 = { { { 3, 12 } }, 0 };
    static constexpr InstructionField FUNCT { { { 7, 25 }, { 3, 12 } }, 0 };
} // namespace non_arg_fields

static constexpr carray<ArgumentDesc, 10> arg_desc_list = {
    // Destination register (rd)
    ArgumentDesc('d', 'g', 0, 0x1f, { { { 5, 7 } }, 0 }),
    // Source register 1 (rs1/rs)
    ArgumentDesc('s', 'g', 0, 0x1f, { { { 5, 15 } }, 0 }),
    // Source register 2 (rs2/rt)
    ArgumentDesc('t', 'g', 0, 0x1f, { { { 5, 20 } }, 0 }),
    // I-type immediate for arithmetic instructions (12bits)
    ArgumentDesc('j', 'n', -0x800, 0x7ff, { { { 12, 20 } }, 0 }),
    // Shamt for bit shift instructions (5bits)
    ArgumentDesc('>', 'n', 0, 0x1f, { { { 5, 20 } }, 0 }),
    // J-type address offset immediate (20bits), encoded in multiples of 2 bytes
    ArgumentDesc('a', 'a', -0x80000, 0x7ffff, { { { 10, 21 }, { 1, 20 }, { 8, 12 }, { 1, 31 } }, 1 }),
    // U-type immediate for LUI and AUIPC (20bits)
    ArgumentDesc('u', 'n', 0, 0xfffff000, { { { 20, 12 } }, 0 }),
    // B-type immediate for branches (12 bits)
    ArgumentDesc('p', 'p', -0x800, 0x7ff, { { { 4, 8 }, { 6, 25 }, { 1, 7 }, { 1, 31 } }, 1 }),
    // Offset immediate for load instructions (12 bits)
    ArgumentDesc('o', 'o', -0x800, 0x7ff, { { { 12, 20 } }, 0 }),
    // Offset immediate for store instructions (12 bits)
    ArgumentDesc('q', 'o', -0x800, 0x7ff, { { { 5, 7 }, { 7, 25 } }, 0 }),
};

LookUpTable<char, ArgumentDesc> arg_desc_by_code { arg_desc_list, &ArgumentDesc::name };

#define FLAGS_ALU_I InstructionFlags(IMF_SUPPORTED | IMF_ALUSRC | IMF_REGWRITE | IMF_ALU_REQ_RS)
#define FLAGS_ALU_I_LOAD                                                                           \
    InstructionFlags(                                                                              \
        IMF_SUPPORTED | IMF_ALUSRC | IMF_REGWRITE | IMF_MEMREAD | IMF_MEM | IMF_ALU_REQ_RS)
#define FLAGS_ALU_I_STORE                                                                          \
    InstructionFlags(                                                                              \
        IMF_SUPPORTED | IMF_ALUSRC | IMF_MEMWRITE | IMF_MEM | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT)
#define FLAGS_ALU_T_R_D InstructionFlags(IMF_SUPPORTED | IMF_REGWRITE)
#define FLAGS_ALU_T_R_STD InstructionFlags(FLAGS_ALU_T_R_D | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT)

#define NOALU                                                                                      \
    { .alu_op = AluOp::ADD }
#define NOMEM .mem_ctl = AC_NONE

// TODO NOTE: if unknown is defined as all 0, instruction map can be significantly simplified
//  using zero initialization.
#define IM_UNKNOWN                                                                                 \
    {                                                                                              \
        "unknown", Instruction::Type::UNKNOWN, NOALU, NOMEM, nullptr, {}, 0, 0, {                  \
            .flags = InstructionFlags(0)                                                           \
        }                                                                                          \
    }

struct InstructionMap {
    const char *name;
    Instruction::Type type;
    AluCombinedOp alu;
    AccessControl mem_ctl;
    const struct InstructionMap *subclass; // when subclass is used then flags
                                           // has special meaning
    const cvector<cstring, 3> args;
    uint32_t code;
    uint32_t mask;
    union {
        unsigned flags;
        InstructionField::Subfield subfield;
    };
};

#define IT_R Instruction::Type::R
#define IT_I Instruction::Type::I
#define IT_S Instruction::Type::S
#define IT_B Instruction::Type::B
#define IT_U Instruction::Type::U
#define IT_J Instruction::Type::J
#define IT_UNKNOWN Instruction::Type::UNKNOWN

// clang-format off

static constexpr struct InstructionMap LOAD_map[] = {
    {"lb",  IT_I, { .alu_op=AluOp::ADD }, AC_I8,  nullptr, {"d", "o(s)"}, 0x00000003,0x0000707f, { .flags = FLAGS_ALU_I_LOAD }}, // LB
    {"lh",  IT_I, { .alu_op=AluOp::ADD }, AC_I16, nullptr, {"d", "o(s)"}, 0x00001003,0x0000707f, { .flags = FLAGS_ALU_I_LOAD }}, // LH
    {"lw",  IT_I, { .alu_op=AluOp::ADD }, AC_I32, nullptr, {"d", "o(s)"}, 0x00002003,0x0000707f, { .flags = FLAGS_ALU_I_LOAD }}, // LW
    {"ld",  IT_I, { .alu_op=AluOp::ADD }, AC_I64, nullptr, {"d", "o(s)"}, 0x00003003,0x0000707f, { .flags = FLAGS_ALU_I_LOAD }}, // LD
    {"lbu", IT_I, { .alu_op=AluOp::ADD }, AC_U8,  nullptr, {"d", "o(s)"}, 0x00004003,0x0000707f, { .flags = FLAGS_ALU_I_LOAD }}, // LBU
    {"lhu", IT_I, { .alu_op=AluOp::ADD }, AC_U16, nullptr, {"d", "o(s)"}, 0x00005003,0x0000707f, { .flags = FLAGS_ALU_I_LOAD }}, // LHU
    {"lwu", IT_I, { .alu_op=AluOp::ADD }, AC_U32, nullptr, {"d", "o(s)"}, 0x00006003,0x0000707f, { .flags = FLAGS_ALU_I_LOAD }}, // LWU
    IM_UNKNOWN,
};

static constexpr struct InstructionMap SRI_map[] = {
    {"srli", IT_I, { .alu_op=AluOp::SR }, NOMEM, nullptr, {"d", "s", ">"}, 0x00005013,0xfe00707f, { .flags = FLAGS_ALU_I }}, // SRLI
    {"srai", IT_I, { .alu_op=AluOp::SR }, NOMEM, nullptr, {"d", "s", ">"}, 0x40005013,0xfe00707f, { .flags = InstructionFlags(FLAGS_ALU_I | IMF_ALU_MOD) }}, // SRAI
};

static constexpr struct InstructionMap OP_IMM_map[] = {
    {"addi",  IT_I, { .alu_op=AluOp::ADD },  NOMEM, nullptr, {"d", "s", "j"}, 0x00000013,0x0000707f, { .flags = FLAGS_ALU_I }}, // ADDI
    {"slli",  IT_I, { .alu_op=AluOp::SLL },  NOMEM, nullptr, {"d", "s", ">"}, 0x00001013,0xfe00707f, { .flags = FLAGS_ALU_I }}, // SLLI
    {"slti",  IT_I, { .alu_op=AluOp::SLT },  NOMEM, nullptr, {"d", "s", "j"}, 0x00002013,0x0000707f, { .flags = FLAGS_ALU_I }}, // SLTI
    {"sltiu", IT_I, { .alu_op=AluOp::SLTU }, NOMEM, nullptr, {"d", "s", "j"}, 0x00003013,0x0000707f, { .flags = FLAGS_ALU_I }}, // SLTIU
    {"xori",  IT_I, { .alu_op=AluOp::XOR },  NOMEM, nullptr, {"d", "s", "j"}, 0x00004013,0x0000707f, { .flags = FLAGS_ALU_I }}, // XORI
    {"sri",   IT_I, NOALU,       NOMEM, SRI_map,              {}, 0x00005013, 0xbe00707f, { .subfield = {1, 30} }}, // SRLI, SRAI
    {"ori",   IT_I, { .alu_op=AluOp::OR },   NOMEM, nullptr, {"d", "s", "j"}, 0x00006013,0x0000707f, { .flags = FLAGS_ALU_I }}, // ORI
    {"andi",  IT_I, { .alu_op=AluOp::AND },  NOMEM, nullptr, {"d", "s", "j"}, 0x00007013,0x0000707f, { .flags = FLAGS_ALU_I }}, // ANDI
};

static constexpr struct InstructionMap STORE_map[] = {
    {"sb", IT_S, { .alu_op=AluOp::ADD }, AC_U8,  nullptr, {"t", "q(s)"}, 0x00000023, 0x0000707f, { .flags = FLAGS_ALU_I_STORE }}, // SB
    {"sh", IT_S, { .alu_op=AluOp::ADD }, AC_U16, nullptr, {"t", "q(s)"}, 0x00001023, 0x0000707f, { .flags = FLAGS_ALU_I_STORE }}, // SH
    {"sw", IT_S, { .alu_op=AluOp::ADD }, AC_U32, nullptr, {"t", "q(s)"}, 0x00002023, 0x0000707f, { .flags = FLAGS_ALU_I_STORE }}, // SW
    {"sd", IT_S, { .alu_op=AluOp::ADD }, AC_U64, nullptr, {"t", "q(s)"}, 0x00003023, 0x0000707f, { .flags = FLAGS_ALU_I_STORE }}, // SD
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
};

static constexpr struct InstructionMap ADD_map[] = {
    {"add", IT_R, { .alu_op=AluOp::ADD }, NOMEM, nullptr, {"d", "s", "t"}, 0x00000033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }},
    {"sub", IT_R, { .alu_op=AluOp::ADD }, NOMEM, nullptr, {"d", "s", "t"}, 0x40000033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD | IMF_ALU_MOD }},
};

static constexpr struct InstructionMap SR_map[] = {
    {"srl", IT_R, { .alu_op=AluOp::SR }, NOMEM, nullptr, {"d", "s", "t"}, 0x00005033,0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }}, // SRL
    {"sra", IT_R, { .alu_op=AluOp::SR }, NOMEM, nullptr, {"d", "s", "t"}, 0x40005033,0xfe00707f,  { .flags = FLAGS_ALU_T_R_STD | IMF_ALU_MOD }}, // SRA
};

static constexpr struct InstructionMap OP_ALU_map[] = {
    {"add/sub", IT_R, NOALU,    NOMEM, ADD_map,              {}, 0x00000033, 0xbe00707f, { .subfield = {1, 30} }},
    {"sll",  IT_R, { .alu_op=AluOp::SLL },  NOMEM, nullptr, {"d", "s", "t"}, 0x00001033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }}, // SLL
    {"slt",  IT_R, { .alu_op=AluOp::SLT },  NOMEM, nullptr, {"d", "s", "t"}, 0x00002033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }}, // SLT
    {"sltu", IT_R, { .alu_op=AluOp::SLTU }, NOMEM, nullptr, {"d", "s", "t"}, 0x00003033,0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }}, // SLTU
    {"xor",  IT_R, { .alu_op=AluOp::XOR },  NOMEM, nullptr, {"d", "s", "t"}, 0x00004033,0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }}, // XOR
    {"sr",   IT_R, NOALU,       NOMEM,  SR_map,              {}, 0x00005033, 0xbe00707f, { .subfield = {1, 30} }}, // SRL, SRA
    {"or",   IT_R, { .alu_op=AluOp::OR },   NOMEM, nullptr, {"d", "s", "t"}, 0x00006033,0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }}, // OR
    {"and",  IT_R, { .alu_op=AluOp::AND },  NOMEM, nullptr, {"d", "s", "t"}, 0x00007033,0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }}, // AND
};

// RV32M
#define MUL_MAP_ITEM(NAME, OP, CODE) \
    { NAME, IT_R, { .mul_op = (OP) }, NOMEM, nullptr, {"d", "s", "t"}, (0x02000033 | (CODE)), 0xfc00707f, { .flags = (FLAGS_ALU_T_R_STD | IMF_MUL) }}

static const struct InstructionMap OP_MUL_map[] = {
    MUL_MAP_ITEM("mul",     MulOp::MUL,     0x0000),
    MUL_MAP_ITEM("mulh",    MulOp::MULH,    0x1000),
    MUL_MAP_ITEM("mulhsu",  MulOp::MULHSU,  0x2000),
    MUL_MAP_ITEM("mulhu",   MulOp::MULHU,   0x3000),
    MUL_MAP_ITEM("div",     MulOp::DIV,     0x4000),
    MUL_MAP_ITEM("divu",    MulOp::DIVU,    0x5000),
    MUL_MAP_ITEM("rem",     MulOp::REM,     0x6000),
    MUL_MAP_ITEM("remu",    MulOp::REMU,    0x7000),
};

static const struct InstructionMap OP_map[] = {
    {"alu", IT_R, NOALU, NOMEM, OP_ALU_map, {}, 0x00000033, 0x0000707f, { .subfield = {3, 12} }},
    {"mul", IT_R, NOALU, NOMEM, OP_MUL_map, {}, 0x02000033, 0xfc00707f, { .subfield = {3, 12} }},
};

static constexpr struct InstructionMap BRANCH_map[] = {
    {"beq",  IT_B, { .alu_op=AluOp::ADD }, NOMEM,  nullptr, {"s", "t", "p"}, 0x00000063,0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT | IMF_ALU_MOD }}, // BEQ
    {"bne",  IT_B, { .alu_op=AluOp::ADD }, NOMEM,  nullptr, {"s", "t", "p"}, 0x00001063, 0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT | IMF_ALU_MOD | IMF_BJ_NOT }}, // BNE
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"blt",  IT_B, { .alu_op=AluOp::SLT }, NOMEM,  nullptr, {"s", "t", "p"}, 0x00004063, 0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT | IMF_BJ_NOT }}, // BLT
    {"bge",  IT_B, { .alu_op=AluOp::SLT }, NOMEM,  nullptr, {"s", "t", "p"}, 0x00005063,0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT }}, // BGE
    {"bltu", IT_B, { .alu_op=AluOp::SLTU }, NOMEM, nullptr, {"s", "t", "p"}, 0x00006063, 0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT | IMF_BJ_NOT }}, // BLTU
    {"bgeu", IT_B, { .alu_op=AluOp::SLTU }, NOMEM, nullptr, {"s", "t", "p"}, 0x00007063,0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT }}, // BGEU
};

static constexpr struct InstructionMap SYSTEM_map[] = {
    {"ecall", IT_I, NOALU, NOMEM, nullptr, {}, 0x00000073, 0xffffffff, { .flags = IMF_SUPPORTED | IMF_EXCEPTION | IMF_ECALL }},
    {"ebreak", IT_I, NOALU, NOMEM, nullptr, {}, 0x00100073, 0xffffffff, { .flags = IMF_SUPPORTED | IMF_EXCEPTION | IMF_EBREAK }},
};

static const struct InstructionMap MISC_MEM_map[] = {
    {"fence", IT_I, NOALU, AC_CACHE_OP, nullptr, {}, 0x0000000f, 0x0000703f, { .flags = IMF_SUPPORTED | IMF_MEM }},
    {"fence.i", IT_I, NOALU, AC_CACHE_OP, nullptr, {}, 0x000100f, 0x0000703f, { .flags = IMF_SUPPORTED | IMF_MEM }},
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
};

static constexpr struct InstructionMap I_inst_map[] = {
    {"load", IT_I, NOALU, NOMEM, LOAD_map, {}, 0x03, 0x7f, { .subfield = {3, 12} }}, // LOAD
    IM_UNKNOWN, // LOAD-FP
    IM_UNKNOWN, // custom-0
    {"misc-mem", IT_I, NOALU, NOMEM, MISC_MEM_map, {}, 0x0f, 0x7f, { .subfield = {3, 12} }}, // MISC-MEM
    {"op-imm", IT_I, NOALU, NOMEM, OP_IMM_map, {}, 0x13, 0x7f, { .subfield = {3, 12} }}, // OP-IMM
    {"auipc", IT_U, { .alu_op=AluOp::ADD }, NOMEM, nullptr, {"d", "u"}, 0x17, 0x7f, { .flags = IMF_SUPPORTED | IMF_ALUSRC | IMF_REGWRITE | IMF_PC_TO_ALU }}, // AUIPC
    IM_UNKNOWN, // OP-IMM-32
    IM_UNKNOWN, // 48b
    {"store", IT_I, NOALU, NOMEM, STORE_map, {}, 0x23, 0x7f, { .subfield = {3, 12} }}, // STORE
    IM_UNKNOWN, // STORE-FP
    IM_UNKNOWN, // custom-1
    IM_UNKNOWN, // AMO
    {"op", IT_R, NOALU, NOMEM, OP_map, {}, 0x33, 0x7f, { .subfield = {1, 25} }}, // OP
    {"lui", IT_U, { .alu_op=AluOp::ADD }, NOMEM, nullptr, {"d", "u"}, 0x37, 0x7f, { .flags = IMF_SUPPORTED | IMF_ALUSRC | IMF_REGWRITE }}, // LUI
    IM_UNKNOWN, // OP-32
    IM_UNKNOWN, // 64b
    IM_UNKNOWN, // MADD
    IM_UNKNOWN, // MSUB
    IM_UNKNOWN, // NMSUB
    IM_UNKNOWN, // NMADD
    IM_UNKNOWN, // OP-FP
    IM_UNKNOWN, // reserved
    IM_UNKNOWN, // custom-2/rv128
    IM_UNKNOWN, // 48b
    {"branch", IT_B, NOALU, NOMEM, BRANCH_map, {}, 0x63, 0x7f, { .subfield = {3, 12} }}, // BRANCH
    {"jalr", IT_I, { .alu_op=AluOp::ADD }, NOMEM, nullptr, {"d", "o(s)"}, 0x67, 0x7f, { .flags =
IMF_SUPPORTED | IMF_REGWRITE | IMF_BRANCH_JALR | IMF_ALUSRC | IMF_ALU_REQ_RS }}, // JALR
    IM_UNKNOWN, // reserved
    {"jal", IT_J, { .alu_op=AluOp::ADD }, NOMEM, nullptr, {"d", "a"}, 0x6f, 0x7f, { .flags =
IMF_SUPPORTED |
IMF_REGWRITE | IMF_JUMP | IMF_PC_TO_ALU | IMF_ALUSRC }}, // JAL
    {"system", IT_I, NOALU, NOMEM, SYSTEM_map, {}, 0x73, 0x7f, { .subfield = {1, 20} }}, // SYSTEM
    IM_UNKNOWN, // reserved
    IM_UNKNOWN, // custom-3/rv128
    IM_UNKNOWN, // >= 80b
};

static constexpr struct InstructionMap C_inst_map[] = {
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"i", IT_UNKNOWN, NOALU, NOMEM, I_inst_map, {}, 0x3, 0x3, { .subfield = {5, 2} }},
};

// clang-format on

constexpr InstructionField::Subfield instruction_map_opcode_field = { 2, 0 };

constexpr const InstructionMap &InstructionMapFind(uint32_t code) {
    const InstructionMap *im = &C_inst_map[instruction_map_opcode_field.decode(code)];
    while (im->subclass != nullptr) {
        im = &im->subclass[im->subfield.decode(code)];
    }
    return *im;
}

constexpr const InstructionMap &nop = InstructionMapFind(0x13);

const std::array<const QString, 34> RECOGNIZED_PSEUDOINSTRUCTIONS {
    "nop",    "la",     "li",     "mv",     "not",  "neg",  "negw", "sext.b", "sext.h",
    "sext.w", "zext.b", "zext.h", "zext.w", "seqz", "snez", "sltz", "slgz",   "beqz",
    "bnez",   "blez",   "bgez",   "bltz",   "bgtz", "bgt",  "ble",  "bgtu",   "bleu",
    "j",      "jal",    "jr",     "jalr",   "ret",  "call", "tail"
};

bool Instruction::symbolic_registers_enabled = false;
const Instruction Instruction::NOP = Instruction(0x00000013);
const Instruction Instruction::UNKNOWN_INST = Instruction(0x0);

uint8_t Instruction::size() const { // NOLINT(readability-convert-member-functions-to-static)
    return 4;
}

uint8_t Instruction::opcode() const {
    // Includes the 2 bits marking it's not a 16b instruction.
    return non_arg_fields::OPCODE.decode(dt);
}

uint8_t Instruction::rs() const {
    return arg_desc_by_code['s']->arg.decode(dt);
}

uint8_t Instruction::rt() const {
    return arg_desc_by_code['t']->arg.decode(dt);
}

uint8_t Instruction::rd() const {
    return arg_desc_by_code['d']->arg.decode(dt);
}

uint8_t Instruction::shamt() const {
    return arg_desc_by_code['<']->arg.decode(dt);
}

uint16_t Instruction::funct() const {
    return non_arg_fields::FUNCT.decode(dt);
}

constexpr Instruction::imm_type immediate_by_arg_code(Instruction::data_type data, char arg_code) {
    const auto &arg = arg_desc_by_code[arg_code]->arg;
    return (Instruction::imm_type)sign_extend(arg.decode(data), arg.bits_used);
}

Instruction::imm_type Instruction::immediate() const {
    switch (this->type()) {
    case Type::I: return immediate_by_arg_code(dt, 'j');
    case Type::S: return immediate_by_arg_code(dt, 'q');
    case Type::B: return immediate_by_arg_code(dt, 'p');
    case Type::U: return immediate_by_arg_code(dt, 'u');
    case Type::J: return immediate_by_arg_code(dt, 'a');
    case Type::R: {
        WARN("Requested immediate of R-type instruction.");
        return 0;
    }
    case Type::UNKNOWN: {
        WARN("Requested immediate of UNKNOWN instruction.");
        return 0;
    }
    }
}

Instruction::data_type Instruction::data() const {
    return dt;
}

bool Instruction::imm_sign() const {
    return dt >> (std::numeric_limits<typeof(dt)>::digits - 1);
}

enum Instruction::Type Instruction::type() const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    return im.type;
}

bool Instruction::operator==(const Instruction &c) const {
    return (this->data() == c.data());
}

bool Instruction::operator!=(const Instruction &c) const {
    return !this->operator==(c);
}

QString Instruction::to_str(Address inst_addr) const {
    const InstructionMap &im = InstructionMapFind(dt);
    // TODO there are exception where some fields are zero and such so we should
    // not print them in such case
    QString res;
    QString next_delim = " ";
    if (im.type == Type::UNKNOWN) { return { "unknown" }; }
    if (this->dt == NOP.dt) { return { "nop" }; }

    res += im.name;
    for (const auto &arg_string : im.args) {
        res += next_delim;
        next_delim = ", ";
        for (size_t pos = 0; pos < arg_string.size(); pos += 1) {
            char arg_letter = arg_string[pos];
            const ArgumentDesc *arg_desc = arg_desc_by_code[arg_letter];
            if (arg_desc == nullptr) {
                res += arg_letter;
                continue;
            }
            auto field = (int32_t)arg_desc->arg.decode(this->dt);
            if (arg_desc->min < 0) { field = sign_extend(field, arg_desc->arg.bits_used); }
            switch (arg_desc->kind) {
            case 'g': {
                if (symbolic_registers_enabled) {
                    res += QString(register_names[field].data());
                } else {
                    res += "x" + QString::number(field);
                }
                break;
            }
            case 'p':
            case 'a': {
                field += (int32_t)inst_addr.get_raw();
                res += "0x" + QString::number(uint32_t(field), 16);
                break;
            }
            case 'o':
            case 'n': {
                if (arg_desc->min < 0) {
                    res += QString::number((int32_t)field, 10);
                } else {
                    res += "0x" + QString::number(uint32_t(field), 16);
                }
                break;
            }
            }
        }
    }
    return res;
}

QMultiMap<QString, uint32_t> str_to_instruction_code_map;

void instruction_from_string_build_base(
    const InstructionMap *im,
    InstructionField::Subfield field,
    uint32_t base_code) {
    uint32_t code;
    uint8_t bits = field.count;
    uint8_t shift = field.offset;

    for (unsigned int i = 0; i < 1U << bits; i++, im++) {
        code = base_code | (i << shift);
        if (im->subclass) {
            instruction_from_string_build_base(im->subclass, im->subfield, code);
            continue;
        }
        if (!(im->flags & IMF_SUPPORTED)) { continue; }
        if (im->code != code) {
            DEBUG("code mismatch %s computed 0x%08x found 0x%08x", im->name, code, im->code);
            continue;
        }
        str_to_instruction_code_map.insert(im->name, code);
    }
#if 0
    for (auto i = str_to_instruction_code_map.begin();
         i != str_to_instruction_code_map.end(); i++)
        std::cout << i.key().toStdString() << ' ';
#endif
}

void instruction_from_string_build_base() {
    return instruction_from_string_build_base(C_inst_map, instruction_map_opcode_field, 0);
}

static int parse_reg_from_string(const QString &str, uint *chars_taken = nullptr) {
    if (str.size() < 2) { return -1; }
    if (str.at(0) == 'x') {
        int res = 0;
        int ctk = 1;
        for (; ctk < str.size(); ctk += 1) {
            auto c = str.at(ctk);
            if (c >= '0' && c <= '9') {
                res *= 10;
                res += c.unicode() - '0';
            } else {
                break;
            }
        }
        if (ctk == 0) {
            return -1;
        } else {
            *chars_taken = ctk;
            return res;
        }
    } else {
        auto data = str.toLocal8Bit();
        int regnum = -1;
        for (size_t i = 0; i < register_names.size(); i++) {
            size_t len = register_names[i].size();
            if (size_t(data.size()) < len) continue;
            if (std::strncmp(data.data(), register_names[i].data(), len) == 0) {
                *chars_taken = len;
                regnum = (int)i;
            }
        }
        return regnum;
    }
}

static void reloc_append(
    RelocExpressionList *reloc,
    const QString &fl,
    Address inst_addr,
    int64_t offset,
    const ArgumentDesc *adesc,
    uint *chars_taken = nullptr,
    const QString &filename = "",
    int line = 0,
    Instruction::Modifier pseudo_mod = machine::Instruction::Modifier::NONE) {
    QString expression = "";
    QString allowed_operators = "+-/*|&^~";
    int i = 0;
    for (; i < fl.size(); i++) {
        QChar ch = fl.at(i);
        if (ch.isSpace()) { continue; }
        if (ch.isLetterOrNumber() || (ch == '_')) {
            expression.append(ch);
        } else if (allowed_operators.indexOf(ch) >= 0) {
            expression.append(ch);
        } else {
            break;
        }
    }

    reloc->emplace_back(new RelocationDesc(
        inst_addr, expression, offset, adesc->min, adesc->max, &adesc->arg, filename, line,
        pseudo_mod));
    if (chars_taken != nullptr) { *chars_taken = i; }
}

size_t Instruction::code_from_tokens(
    uint32_t *code,
    size_t buffer_size,
    TokenizedInstruction &input,
    RelocExpressionList *reloc,
    bool pseudoinst_enabled) {
    if (str_to_instruction_code_map.isEmpty()) { instruction_from_string_build_base(); }

    Instruction result = base_from_tokens(input, reloc);
    if (result.data() != 0) {
        if (result.size() > buffer_size) {
            // NOTE: this is bug, not user error.
            throw ParseError("insufficient buffer size to write parsed instruction");
        }
        *code = result.data();
        return result.size();
    }

    if (pseudoinst_enabled) {
        size_t pseudo_result = pseudo_from_tokens(code, buffer_size, input, reloc);
        if (pseudo_result != 0) { return pseudo_result; }
    }
    throw ParseError("unknown instruction");
}
size_t Instruction::pseudo_from_tokens(
    uint32_t *code,
    size_t buffsize,
    TokenizedInstruction &inst,
    RelocExpressionList *reloc) {
    constexpr Modifier UPPER = Modifier::COMPOSED_IMM_UPPER;
    constexpr Modifier LOWER = Modifier::COMPOSED_IMM_LOWER;

    if (inst.base == QLatin1String("nop")) {
        Instruction result;
        if (!inst.fields.empty()) { throw ParseError("`nop` does not allow any arguments"); }
        result = Instruction::NOP;
        *code = result.data();
        return result.size();
    }
    if (inst.base == QLatin1String("la")) {
        if (inst.fields.size() != 2) { throw ParseError("number of arguments does not match"); }
        *code = base_from_tokens(
                    { "auipc", inst.fields, inst.address, inst.filename, inst.line }, reloc, UPPER,
                    -inst.address.get_raw())
                    .data();
        code += 1;
        inst.fields.insert(0, inst.fields.at(0));
        *code = base_from_tokens(
                    { "addi", inst.fields, inst.address + 4, inst.filename, inst.line }, reloc,
                    LOWER, -inst.address.get_raw())
                    .data();
        return 8;
    }

    if (inst.base == QLatin1String("li")) {
        if (inst.fields.size() != 2) { throw ParseError("number of arguments does not match"); }
        *code = base_from_tokens(
                    { "lui", inst.fields, inst.address, inst.filename, inst.line }, reloc, UPPER)
                    .data();
        code += 1;
        inst.fields.insert(0, inst.fields.at(0));
        *code
            = base_from_tokens(
                  { "addi", inst.fields, inst.address + 4, inst.filename, inst.line }, reloc, LOWER)
                  .data();
        return 8;
    }
    if (inst.base == QLatin1String("mv")) {
        return partially_apply("addi", 2, 2, "0", code, buffsize, inst, reloc);
    }
    if (inst.base[0] == 'n') {
        if (inst.base == QLatin1String("not")) {
            return partially_apply("xori", 2, 2, "-1", code, buffsize, inst, reloc);
        }
        if (inst.base == QLatin1String("neg")) {
            return partially_apply("sub", 2, 1, "x0", code, buffsize, inst, reloc);
        }
        if (inst.base == QLatin1String("negw")) {
            return partially_apply("subw", 2, 1, "x0", code, buffsize, inst, reloc);
        }
    }
    if (inst.base[0] == 's') {
        if (inst.base == QLatin1String("sext.b")) {
            if (inst.fields.size() != 2) { throw ParseError("number of arguments does not match"); }
            inst.base = "slli";
            inst.fields.append("XLEN-8");
            *code = base_from_tokens(inst, reloc).data();
            code += 1;
            inst.base = "srai";
            inst.fields[1] = inst.fields[0];
            *code = base_from_tokens(inst, reloc).data();
            return 8;
        }
        if (inst.base == QLatin1String("sext.h")) {
            if (inst.fields.size() != 2) { throw ParseError("number of arguments does not match"); }
            inst.base = "slli";
            inst.fields.append("XLEN-16");
            *code = base_from_tokens(inst, reloc).data();
            code += 1;
            inst.base = "srai";
            inst.fields[1] = inst.fields[0];
            *code = base_from_tokens(inst, reloc).data();
            return 8;
        }
        if (inst.base == QLatin1String("sext.w")) {
            return partially_apply("addiw", 2, 2, "0", code, buffsize, inst, reloc);
        }
    }
    if (inst.base[0] == 'z') {
        if (inst.base == QLatin1String("zext.b")) {
            return partially_apply("addi", 2, 2, "255", code, buffsize, inst, reloc);
        }
        if (inst.base == QLatin1String("zext.h")) {
            if (inst.fields.size() != 2) { throw ParseError("number of arguments does not match"); }
            inst.base = "slli";
            inst.fields.append("XLEN-16");
            *code = base_from_tokens(inst, reloc).data();
            code += 1;
            inst.base = "srli";
            inst.fields[1] = inst.fields[0];
            inst.fields.append("XLEN-16");
            *code = base_from_tokens(inst, reloc).data();
            return 8;
        }
        if (inst.base == QLatin1String("zext.w")) {
            if (inst.fields.size() != 2) { throw ParseError("number of arguments does not match"); }
            inst.base = "slli";
            inst.fields.append("XLEN-32");
            *code = base_from_tokens(inst, reloc).data();
            code += 1;
            inst.base = "srli";
            inst.fields[1] = inst.fields[0];
            inst.fields.append("XLEN-32");
            *code = base_from_tokens(inst, reloc).data();
            return 8;
        }
    }
    if (inst.base[0] == 's') {
        if (inst.base == QLatin1String("seqz")) {
            return partially_apply("sltiu", 2, 2, "1", code, buffsize, inst, reloc);
        }
        if (inst.base == QLatin1String("snez")) {
            return partially_apply("sltu", 2, 1, "x0", code, buffsize, inst, reloc);
        }
        if (inst.base == QLatin1String("sltz")) {
            return partially_apply("slt", 2, 2, "x0", code, buffsize, inst, reloc);
        }
        if (inst.base == QLatin1String("slgz")) {
            return partially_apply("slt", 2, 1, "x0", code, buffsize, inst, reloc);
        }
    }
    if (inst.base[0] == 'b') {
        if (inst.base == QLatin1String("beqz")) {
            return partially_apply("beq", 2, 1, "x0", code, buffsize, inst, reloc);
        }
        if (inst.base == QLatin1String("bnez")) {
            return partially_apply("bne", 2, 1, "x0", code, buffsize, inst, reloc);
        }
        if (inst.base == QLatin1String("blez")) {
            return partially_apply("ble", 2, 0, "x0", code, buffsize, inst, reloc);
        }
        if (inst.base == QLatin1String("bgez")) {
            return partially_apply("bge", 2, 0, "x0", code, buffsize, inst, reloc);
        }
        if (inst.base == QLatin1String("bltz")) {
            return partially_apply("blt", 2, 1, "x0", code, buffsize, inst, reloc);
        }
        if (inst.base == QLatin1String("bgtz")) {
            return partially_apply("blt", 2, 0, "x0", code, buffsize, inst, reloc);
        }
        if (inst.base == QLatin1String("bgt")) {
            if (inst.fields.size() != 3) { throw ParseError("number of arguments does not match"); }
            inst.base = "blt";
            std::swap(inst.fields[0], inst.fields[1]);
            return code_from_tokens(code, buffsize, inst, reloc, false);
        }
        if (inst.base == QLatin1String("ble")) {
            if (inst.fields.size() != 3) { throw ParseError("number of arguments does not match"); }
            inst.base = "bgt";
            std::swap(inst.fields[0], inst.fields[1]);
            return code_from_tokens(code, buffsize, inst, reloc, false);
        }
        if (inst.base == QLatin1String("bgtu")) {
            if (inst.fields.size() != 3) { throw ParseError("number of arguments does not match"); }
            inst.base = "bltu";
            std::swap(inst.fields[0], inst.fields[1]);
            return code_from_tokens(code, buffsize, inst, reloc, false);
        }
        if (inst.base == QLatin1String("bleu")) {
            if (inst.fields.size() != 3) { throw ParseError("number of arguments does not match"); }
            inst.base = "bgeu";
            std::swap(inst.fields[0], inst.fields[1]);
            return code_from_tokens(code, buffsize, inst, reloc, false);
        }
    }
    if (inst.base[0] == 'j') {
        if (inst.base == QLatin1String("j")) {
            if (inst.fields.size() != 1) { throw ParseError("number of arguments does not match"); }
            inst.base = "jal";
            inst.fields.insert(0, "x0");
            return code_from_tokens(code, buffsize, inst, reloc, false);
        }
        if (inst.base == QLatin1String("jal")) {
            if (inst.fields.size() != 1) { throw ParseError("number of arguments does not match"); }
            inst.base = "jal";
            inst.fields.insert(0, "x1");
            return code_from_tokens(code, buffsize, inst, reloc, false);
        }
        if (inst.base == QLatin1String("jr")) {
            if (inst.fields.size() != 1) { throw ParseError("number of arguments does not match"); }
            inst.base = "jalr";
            inst.fields.insert(0, "x0");
            inst.fields[1] = QString("0x0(%0)").arg(inst.fields[1]);
            return code_from_tokens(code, buffsize, inst, reloc, false);
        }
        if (inst.base == QLatin1String("jalr")) {
            if (inst.fields.size() != 1) { throw ParseError("number of arguments does not match"); }
            inst.base = "jalr";
            inst.fields.insert(0, "x1");
            inst.fields[1] = QString("0x0(%0)").arg(inst.fields[1]);
            return code_from_tokens(code, buffsize, inst, reloc, false);
        }
    }
    if (inst.base == QLatin1String("ret")) {
        if (!inst.fields.empty()) { throw ParseError("number of arguments does not match"); }
        inst.base = "jalr";
        inst.fields.append("x0");
        inst.fields.append("0x0(x1)");
        return code_from_tokens(code, buffsize, inst, reloc, false);
    }
    if (inst.base == QLatin1String("call")) {
        if (inst.fields.size() != 1) { throw ParseError("number of arguments does not match"); }
        inst.fields.insert(0, "x6");
        *code = base_from_tokens(
                    { "auipc", inst.fields, inst.address, inst.filename, inst.line }, reloc, UPPER,
                    -inst.address.get_raw())
                    .data();
        code += 1;
        inst.fields[0] = QString("x1");
        inst.fields[1] = QString("%0(x6)").arg(inst.fields[1]);
        *code = base_from_tokens(
                    { "jalr", inst.fields, inst.address + 4, inst.filename, inst.line }, reloc,
                    LOWER, -inst.address.get_raw())
                    .data();
        return 8;
    }
    if (inst.base == QLatin1String("tail")) {
        if (inst.fields.size() != 1) { throw ParseError("number of arguments does not match"); }
        inst.fields.insert(0, "x6");
        *code = base_from_tokens(
                    { "auipc", inst.fields, inst.address, inst.filename, inst.line }, reloc, UPPER,
                    -inst.address.get_raw())
                    .data();
        code += 1;
        inst.fields[0] = QString("x0");
        inst.fields[1] = QString("%0(x6)").arg(inst.fields[1]);
        *code = base_from_tokens(
                    { "jalr", inst.fields, inst.address + 4, inst.filename, inst.line }, reloc,
                    LOWER, -inst.address.get_raw())
                    .data();
        return 8;
    }

    return 0;
}
size_t Instruction::partially_apply(
    const char *base,
    int argument_count,
    int position,
    const char *value,
    uint32_t *code,
    size_t buffsize,
    TokenizedInstruction &inst,
    RelocExpressionList *reloc) {
    if (inst.fields.size() != argument_count) {
        throw ParseError("number of arguments does not match");
    }
    inst.base = base;
    inst.fields.insert(position, value);
    return code_from_tokens(code, buffsize, inst, reloc, false);
}
Instruction Instruction::base_from_tokens(
    const TokenizedInstruction &input,
    RelocExpressionList *reloc,
    Modifier pseudo_mod,
    uint64_t initial_immediate_value) {
    auto iter_range = str_to_instruction_code_map.equal_range(input.base);
    if (iter_range.first == iter_range.second) {
        DEBUG("Base instruction of the name %s not found.", qPrintable(input.base));
        return Instruction::UNKNOWN_INST;
    }
    // Process all codes associated with given instruction name and try matching the supplied
    // instruction field tokens to fields. First matching instruction is used.
    // TODO: Can the map contain more codes for single name?
    // TODO: Why not store `InstructionMap` directly in the multimap?
    for (; iter_range.first != iter_range.second; iter_range.first += 1) {
        uint32_t inst_code = iter_range.first.value();
        const InstructionMap &imap = InstructionMapFind(inst_code);

        if ((size_t)input.fields.count() != imap.args.size()) { continue; }

        for (size_t field_index = 0; field_index < imap.args.size(); field_index += 1) {
            const auto &arg = imap.args[field_index];
            QString field_token = input.fields[field_index];
            inst_code |= parse_field(
                field_token, arg, input.address, reloc, input.filename, input.line, pseudo_mod,
                initial_immediate_value);
        }
        return Instruction(inst_code);
    }

    DEBUG(
        "Base instruction of the name %s not matched to any known base format.",
        qPrintable(input.base));
    // Another instruction format for this base may be found in pseudoinstructions.
    return Instruction::UNKNOWN_INST;
}

void parse_immediate_value(
    const QString &field_token,
    Address &inst_addr,
    RelocExpressionList *reloc,
    const QString &filename,
    unsigned int line,
    bool need_reloc,
    const ArgumentDesc *adesc,
    const Instruction::Modifier &effective_mod,
    uint64_t &val,
    uint &chars_taken);

uint32_t Instruction::parse_field(
    QString &field_token,
    const cstring &arg,
    Address inst_addr,
    RelocExpressionList *reloc,
    const QString &filename,
    unsigned int line,
    Modifier pseudo_mod,
    uint64_t initial_immediate_value) {
    uint32_t inst_code = 0;
    for (size_t i = 0; i < arg.size(); i += 1) {
        bool need_reloc = false;
        uint a = arg[i];
        if (!a) { continue; }
        field_token = field_token.trimmed();
        const ArgumentDesc *adesc = arg_desc_by_code[a];
        if (adesc == nullptr) {
            if (!field_token.count()) { throw ParseError("empty argument encountered"); }
            if (field_token.at(0) != a) {
                throw ParseError("argument does not match instruction template");
            }
            field_token = field_token.mid(1);
            continue;
        }

        // Only apply modifier to immediate fields
        const Modifier effective_mod = (adesc->is_imm()) ? pseudo_mod : Modifier::NONE;

        uint64_t val = 0;
        uint chars_taken = 0;

        switch (adesc->kind) {
        case 'g': val += parse_reg_from_string(field_token, &chars_taken); break;
        case 'p':
        case 'a': val -= inst_addr.get_raw(); FALLTROUGH
        case 'o':
        case 'n': {
            val += initial_immediate_value;
            parse_immediate_value(
                field_token, inst_addr, reloc, filename, line, need_reloc, adesc, effective_mod,
                val, chars_taken);
            break;
        }
        }
        if (chars_taken <= 0) { throw ParseError("argument parse error"); }

        if (effective_mod != Modifier::NONE) {
            val = modify_pseudoinst_imm(effective_mod, val);
        } else if (!adesc->is_value_in_field_range(val)) {
            throw ParseError("argument range exceed");
        }

        inst_code |= adesc->arg.encode(val);
        field_token = field_token.mid(chars_taken);
    }
    if (field_token.trimmed() != "") { throw ParseError("excessive characters in argument"); }
    return inst_code;
}

void parse_immediate_value(
    const QString &field_token,
    Address &inst_addr,
    RelocExpressionList *reloc,
    const QString &filename,
    unsigned int line,
    bool need_reloc,
    const ArgumentDesc *adesc,
    const Instruction::Modifier &effective_mod,
    uint64_t &val,
    uint &chars_taken) {
    if (field_token.at(0).isDigit() || field_token.at(0) == '-' || (reloc == nullptr)) {
        uint64_t num_val = 0;
        // Qt functions are limited, toLongLong would be usable
        // but does not return information how many characters
        // are processed. Used solution has horrible overhead
        // but is usable for now
        int i;
        char cstr[field_token.count() + 1];
        for (i = 0; i < field_token.count(); i++) {
            cstr[i] = field_token.at(i).toLatin1();
        }
        cstr[i] = 0;
        const char *p = cstr;
        char *r;
        if (adesc->min < 0) {
            num_val = strtoll(p, &r, 0);
        } else {
            num_val = strtoull(p, &r, 0);
        }
        while (*r && isspace(*r)) {
            r++;
        }
        chars_taken = r - p;
        if (*r && strchr("+-/*|&^~", *r)) {
            need_reloc = true;
        } else {
            // extend signed bits
            val += num_val;
        }
    } else {
        need_reloc = true;
    }
    if (need_reloc && (reloc != nullptr)) {
        reloc_append(
            reloc, field_token, inst_addr, val, adesc, &chars_taken, filename, line, effective_mod);
        val = 0;
    }
}

bool Instruction::update_value_with_dynamic_relocation(int64_t new_val, RelocationDesc *reloc_desc) {
    // Clear all bit of the updated argument.
    dt &= ~reloc_desc->arg->encode(~0);
    new_val += reloc_desc->offset;

    if (reloc_desc->pseudo_mod != Modifier::NONE) {
        new_val = (int64_t)modify_pseudoinst_imm(reloc_desc->pseudo_mod, new_val);
    } else {
        if ((new_val & ((1 << reloc_desc->arg->shift) - 1))) { return false; }
        if (reloc_desc->min < 0) {
            if (((int64_t)new_val < reloc_desc->min) || ((int64_t)new_val > reloc_desc->max)) {
                if (((int64_t)new_val - 0x100000000 < reloc_desc->min)
                    || ((int64_t)new_val - 0x100000000 > reloc_desc->max)) {
                    return false;
                }
            }
        } else {
            if (((uint64_t)new_val < (uint64_t)reloc_desc->min)
                || ((uint64_t)new_val > (uint64_t)reloc_desc->max)) {
                return false;
            }
        }
    }

    dt |= reloc_desc->arg->encode(new_val);
    return true;
}

constexpr uint64_t Instruction::modify_pseudoinst_imm(Instruction::Modifier mod, uint64_t value) {
    // Example: la rd, symbol -> auipc rd, symbol[31:12] + symbol[11], addi rd, rd, symbol[11:0]

    switch (mod) {
    case Modifier::NONE: return value;
    case Modifier::COMPOSED_IMM_UPPER: return get_bits(value, 31, 12) + get_bit(value, 11);
    case Modifier::COMPOSED_IMM_LOWER: return get_bits(value, 11, 0);
    }
}

// highlighter
void Instruction::get_recognized_instruction_names(QStringList &list) {
    if (str_to_instruction_code_map.isEmpty()) { instruction_from_string_build_base(); }

    for (auto iter = str_to_instruction_code_map.keyBegin();
         iter != str_to_instruction_code_map.keyEnd(); iter++) {
        list.append(*iter);
    }
    for (const auto &str : RECOGNIZED_PSEUDOINSTRUCTIONS) {
        list.append(str);
    }
}

void Instruction::get_recognized_registers(QStringList &list) {
    for (auto name : register_names) {
        list.append(QLatin1String(name.data(), name.size()));
    }
}

void Instruction::set_symbolic_registers(bool enable) {
    symbolic_registers_enabled = enable;
}

size_t Instruction::code_from_string(
    uint32_t *code_output,
    size_t buffer_size,
    QString input,
    Address inst_addr,
    RelocExpressionList *reloc,
    const QString &filename,
    unsigned line,
    bool pseudoinst_enabled) {
    auto inst = TokenizedInstruction::from_line(std::move(input), inst_addr, filename, line);
    return Instruction::code_from_tokens(code_output, buffer_size, inst, reloc, pseudoinst_enabled);
}

constexpr Instruction Instruction::create_type_r(
    uint8_t opcode,
    uint8_t rs,
    uint8_t rt,
    uint8_t rd,
    uint8_t shamt,
    uint8_t funct3,
    uint8_t funct7) {
    Instruction::data_type dt = 0;
    dt |= non_arg_fields::OPCODE.encode(opcode);
    dt |= arg_desc_by_code['d']->arg.encode(rd);
    dt |= arg_desc_by_code['s']->arg.encode(rs);
    dt |= arg_desc_by_code['t']->arg.encode(rt);
    dt |= arg_desc_by_code['<']->arg.encode(shamt);
    dt |= non_arg_fields::FUNCT3.encode(funct3);
    dt |= non_arg_fields::FUNCT7.encode(funct7);
    return Instruction(dt);
}

constexpr Instruction
Instruction::crate_type_i(uint8_t opcode, uint8_t rs, uint8_t rt, uint16_t immediate) {
    Instruction::data_type dt = 0;
    dt |= non_arg_fields::OPCODE.encode(opcode);
    dt |= arg_desc_by_code['s']->arg.encode(rs);
    dt |= arg_desc_by_code['t']->arg.encode(rt);
    dt |= arg_desc_by_code['j']->arg.encode(immediate);
    return Instruction(dt);
}

constexpr Instruction Instruction::create_type_j(uint8_t opcode, Address address) {
    Instruction::data_type dt = 0;
    dt |= non_arg_fields::OPCODE.encode(opcode);
    dt |= arg_desc_by_code['a']->arg.encode(address.get_raw());
    return Instruction(dt);
}

Instruction::ParseError::ParseError(QString message) : message(std::move(message)) {}

TokenizedInstruction TokenizedInstruction::from_line(
    QString line_str,
    Address inst_addr,
    const QString &filename,
    unsigned line) {
    int start = 0, end;
    while (start < line_str.size()) {
        if (!line_str.at(start).isSpace()) { break; }
        start++;
    }
    end = start;
    while (end < line_str.size()) {
        if (!line_str.at(end).isLetterOrNumber()) { break; }
        end++;
    }
    QString inst_base = line_str.mid(start, end - start).toLower();
    if (!inst_base.size()) { throw Instruction::ParseError("empty instruction field"); }

    line_str = line_str.mid(end + 1).trimmed();
    QStringList inst_fields;
    if (line_str.size() > 0) { inst_fields = line_str.split(","); }

    return { inst_base, inst_fields, inst_addr, filename, line };
}

TokenizedInstruction::TokenizedInstruction(
    QString base,
    QStringList fields,
    const Address &address,
    QString filename,
    unsigned int line)
    : base(std::move(base))
    , fields(std::move(fields))
    , address(address)
    , filename(std::move(filename))
    , line(line) {}

InstructionSemantics::InstructionSemantics(const Instruction &instruction)
    : instruction(instruction)
    , instruction_map(InstructionMapFind(instruction.data())) {}

InstructionFlags InstructionSemantics::flags() const {
    return static_cast<InstructionFlags>(
        ((instruction.data() ^ instruction_map.code) & (instruction_map.mask))
            ? (instruction_map.flags & ~IMF_SUPPORTED)
            : instruction_map.flags);
}

AluCombinedOp InstructionSemantics::alu_op() const {
    return instruction_map.alu;
}

AccessControl InstructionSemantics::mem_ctl() const {
    return instruction_map.mem_ctl;
}
} // namespace machine