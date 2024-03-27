#include "instruction.h"

#include "common/logging.h"
#include "common/math/bit_ops.h"
#include "common/string_utils.h"
#include "csr/controlstate.h"
#include "simulator_exception.h"
#include "utils.h"

#include <QChar>
#include <QMultiMap>
#include <cctype>
#include <cstring>
#include <cinttypes>
#include <set>
#include <type_traits>
#include <utility>

LOG_CATEGORY("machine.instruction");

using namespace machine;
using std::underlying_type;

namespace machine {
}

struct ArgumentDesc {
    char name;
    /**
     * Possible values:
     *  @val g: gp register id
     *  @val n: numeric immediate
     *  @val a: pc relative address offset
     *  @val b: pc relative address offset
     *  @val o: offset immediate
     */
    char kind;
    int64_t min;
    int64_t max;
    BitArg arg;
    inline ArgumentDesc(char name, char kind, int64_t min, int64_t max, BitArg arg)
        : name(name)
        , kind(kind)
        , min(min)
        , max(max)
        , arg(arg) {}

    /** Check whether given value fits into this instruction field. */
    [[nodiscard]] constexpr bool is_value_in_field_range(RegisterValue val) const {
        if (min < 0) {
            return val.as_i64() <= max && val.as_i64() >= min;
        } else {
            return val.as_u64() <= static_cast<uint64_t>(max)
                   && val.as_u64() >= static_cast<uint64_t>(min);
        }
    }

    [[nodiscard]] constexpr bool is_imm() const { return kind != 'g'; }
};

static const ArgumentDesc arg_desc_list[] = {
    // Destination register (rd)
    ArgumentDesc('d', 'g', 0, 0x1f, { { { 5, 7 } }, 0 }),
    // Source register 1 (rs1/rs)
    ArgumentDesc('s', 'g', 0, 0x1f, { { { 5, 15 } }, 0 }),
    // Source register 2 (rs2/rt)
    ArgumentDesc('t', 'g', 0, 0x1f, { { { 5, 20 } }, 0 }),
    // I-type immediate for arithmetic instructions (12bits)
    ArgumentDesc('j', 'n', -0x800, 0x7ff, { { { 12, 20 } }, 0 }),
    // Shift for bit shift instructions (5bits)
    ArgumentDesc('>', 'n', 0, 0x1f, { { { 5, 20 } }, 0 }),
    // Address offset immediate (20bits), encoded in multiples of 2 bytes
    ArgumentDesc('a', 'a', -0x80000, 0x7ffff, { { { 10, 21 }, { 1, 20 }, { 8, 12 }, { 1, 31 } }, 1 }),
    // U-type immediate for LUI and AUIPC (20bits)
    ArgumentDesc('u', 'n', 0, 0xfffff000, { { { 20, 12 } }, 0 }),
    // B-type immediate for branches (12 bits)
    ArgumentDesc('p', 'p', -0x1000, 0x0fff, { { { 4, 8 }, { 6, 25 }, { 1, 7 }, { 1, 31 } }, 1 }),
    // Offset immediate for load instructions (12 bits)
    ArgumentDesc('o', 'o', -0x800, 0x7ff, { { { 12, 20 } }, 0 }),
    // Offset immediate for store instructions (12 bits)
    ArgumentDesc('q', 'o', -0x800, 0x7ff, { { { 5, 7 }, { 7, 25 } }, 0 }),
    // 5-bit CSR value immediate
    // (https://sourceware.org/git/?p=binutils-gdb.git;a=blob;f=opcodes/riscv-opc.c;h=7e95f645c5c5fe0a7c93c64c2f1719efaec67972;hb=HEAD#l928)
    ArgumentDesc('Z', 'n', 0, 0x1f, { { { 5, 15 } }, 0 }),
    // 12-bit CSR address
    // (https://sourceware.org/git/?p=binutils-gdb.git;a=blob;f=opcodes/riscv-opc.c;h=7e95f645c5c5fe0a7c93c64c2f1719efaec67972;hb=HEAD#l928)
    ArgumentDesc('E', 'E', 0, 0xfff, { { { 12, 20 } }, 0 }),
};

static const ArgumentDesc *arg_desc_by_code[(int)('z' + 1)];

static bool fill_argdesbycode() {
    for (const auto &desc : arg_desc_list) {
        arg_desc_by_code[(uint)(unsigned char)desc.name] = &desc;
    }
    return true;
}

bool argdesbycode_filled = fill_argdesbycode();

#define FLAGS_ALU_I (IMF_SUPPORTED | IMF_ALUSRC | IMF_REGWRITE | IMF_ALU_REQ_RS)
#define FLAGS_ALU_I_LOAD                                                                           \
    (IMF_SUPPORTED | IMF_ALUSRC | IMF_REGWRITE | IMF_MEMREAD | IMF_MEM | IMF_ALU_REQ_RS)
#define FLAGS_ALU_I_STORE                                                                          \
    (IMF_SUPPORTED | IMF_ALUSRC | IMF_MEMWRITE | IMF_MEM | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT)
#define FLAGS_ALU_T_R_D (IMF_SUPPORTED | IMF_REGWRITE)
#define FLAGS_ALU_T_R_STD (FLAGS_ALU_T_R_D | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT)

#define FLAGS_AMO_LOAD                                                                             \
    (FLAGS_ALU_I_LOAD | IMF_AMO)
// FLAGS_AMO_STORE for store conditional requires IMF_MEMREAD to ensure stalling because
// forwarding is not possible from memory stage after memory read, TODO to solve better way
#define FLAGS_AMO_STORE                                                                            \
    (FLAGS_ALU_I_STORE | FLAGS_ALU_T_R_D | IMF_AMO | IMF_MEMREAD)
#define FLAGS_AMO_MODIFY                                                                           \
    (FLAGS_ALU_I_LOAD | FLAGS_AMO_STORE | IMF_AMO)

#define NOALU                                                                                      \
    { .alu_op = AluOp::ADD }
#define NOMEM .mem_ctl = AC_NONE

// TODO NOTE: if unknown is defined as all 0, instruction map can be significantly simplified
//  using zero initialization.
#define IM_UNKNOWN                                                                                 \
    {                                                                                              \
        "unknown", Instruction::UNKNOWN, NOALU, NOMEM, nullptr, {}, 0, 0, {                        \
            0                                                                                      \
        },                                                                                         \
        nullptr                                                                                    \
    }

struct InstructionMap {
    const char *name;
    Instruction::Type type = Instruction::UNKNOWN;
    AluCombinedOp alu = { .alu_op = AluOp::ADD } ;
    AccessControl mem_ctl = AC_NONE;
    const struct InstructionMap *subclass = nullptr; // when subclass is used then flags
                                           // has special meaning
    const cvector<QString, 3> args;
    uint32_t code;
    uint32_t mask;
    union {
        decltype(underlying_type<InstructionFlags>::type()) flags;
        BitField subfield;
    };
    const InstructionMap *aliases = nullptr;
};

#define IT_R Instruction::R
#define IT_I Instruction::I
#define IT_S Instruction::S
#define IT_B Instruction::B
#define IT_U Instruction::U
#define IT_J Instruction::J
#define IT_AMO Instruction::AMO
#define IT_UNKNOWN Instruction::UNKNOWN

// clang-format off

// alliases for instructions for internal assembler and possibly
// disassembler if simplified format is requested.
// They are not used during decoding and execution

#define INST_ALIAS_LIST_END {.name = nullptr, .args = {}, .code = 0 , .mask = 0, .flags = 0 }

static const struct InstructionMap inst_aliases_addi[] = {
    { .name = "mv", .args = {"d", "s"}, .code = 0x13, .mask = 0x707f | (0xffful << 20), .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_andi[] = {
    { .name = "zext.b", .args = {"d", "s"}, .code = 0x7013 | (0xfful << 20), .mask = 0x707f | (0xffful << 20), .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_addiw[] = {
    { .name = "sext.w", .args = {"d", "s"}, .code = 0x1b, .mask = 0x707f | (0xffful << 20), .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_xori[] = {
    { .name = "not", .args = {"d", "s"}, .code = 0x4013 | (0xffful << 20), .mask = 0x707f | (0xffful << 20), .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_sub[] = {
    { .name = "neg", .args = {"d", "t"}, .code = 0x40000033 | (0 << 15), .mask = 0xfe00707f | (31 << 15), .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_subw[] = {
    { .name = "negw", .args = {"d", "t"}, .code = 0x4000003b | (0 << 15), .mask = 0xfe00707f | (31 << 15), .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_sltiu[] = {
    { .name = "seqz", .args = {"d", "s"}, .code = 0x3013| (1 << 20), .mask = 0x0000707f | (0xffful << 20), .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_sltu[] = {
    { .name = "snez", .args = {"d", "t"}, .code = 0x3033 | (0 << 15), .mask = 0xfe00707f | (31 << 15), .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_slt[] = {
    { .name = "sltz", .args = {"d", "s"}, .code = 0x2033 | (0 << 20), .mask = 0xfe00707f | (31 << 20), .flags = IMF_SUPPORTED },
    { .name = "sgtz", .args = {"d", "t"}, .code = 0x2033 | (0 << 15), .mask = 0xfe00707f | (31 << 15), .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_beq[] = { //0x00000063,0x0000707f
    { .name = "beqz", .args = {"s", "p"}, .code = 0x0063 | (0 << 20), .mask = 0x707f | (31 << 20), .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_bne[] = { //0x00001063, 0x0000707f
    { .name = "bnez", .args = {"s", "p"}, .code = 0x1063 | (0 << 20), .mask = 0x707f | (31 << 20), .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_blt[] = { //0x00004063, 0x0000707f
    { .name = "bltz", .args = {"s", "p"}, .code = 0x4063 | (0 << 20), .mask = 0x707f | (31 << 20), .flags = IMF_SUPPORTED },
    { .name = "bgtz", .args = {"t", "p"}, .code = 0x4063 | (0 << 15), .mask = 0x707f | (31 << 15), .flags = IMF_SUPPORTED },
    { .name = "bgt", .args = {"t", "s", "p"}, .code = 0x4063, .mask = 0x707f, .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_bge[] = { //0x00005063,0x0000707f
    { .name = "bgez", .args = {"s", "p"}, .code = 0x5063 | (0 << 20), .mask = 0x707f | (31 << 20), .flags = IMF_SUPPORTED },
    { .name = "ble", .args = {"t", "s", "p"}, .code = 0x5063, .mask = 0x707f, .flags = IMF_SUPPORTED },
    { .name = "blez", .args = {"t", "p"}, .code = 0x5063 | (0 << 15), .mask = 0x707f | (31 << 15), .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_bltu[] = { //0x00006063, 0x0000707f
    { .name = "bgtu", .args = {"t", "s", "p"}, .code = 0x6063, .mask = 0x707f, .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_bgeu[] = { //0x00007063,0x0000707f
    { .name = "bleu", .args = {"t", "s", "p"}, .code = 0x7063, .mask = 0x707f, .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_jal[] = {
    { .name = "j", .args = {"a"}, .code = 0x6f | (0 << 7) , .mask = 0x7f | (31 << 7), .flags = IMF_SUPPORTED },
    { .name = "jal", .args = {"a"}, .code = 0x6f | (1 << 7) , .mask = 0x7f | (31 << 7), .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_jalr[] = {
    { .name = "ret", .args = {}, .code = 0x67 | (0 << 7) | (1 << 15) , .mask = 0xfffffffful, .flags = IMF_SUPPORTED },
    { .name = "jr", .args = {"s"}, .code = 0x67 | (0 << 7) , .mask = 0x7f | (31 << 7) | (0xffful << 20), .flags = IMF_SUPPORTED },
    { .name = "jr", .args = {"o(s)"}, .code = 0x67 | (0 << 7) , .mask = 0x7f | (31 << 7), .flags = IMF_SUPPORTED },
    { .name = "jalr", .args = {"s"}, .code = 0x67 | (1 << 7) , .mask = 0x7f | (31 << 7) | (0xffful << 20), .flags = IMF_SUPPORTED },
    { .name = "jalr", .args = {"o(s)"}, .code = 0x67 | (1 << 7) , .mask = 0x7f | (31 << 7), .flags = IMF_SUPPORTED },
    { .name = "jalr", .args = {"d", "s", "o"}, .code = 0x67, .mask = 0x7f, .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_csrrw[] = {
    { .name = "csrw", .args = {"E", "s"}, .code = 0x1073 | (0 << 7) | (0 << 15) , .mask = 0x707f | (31 << 7), .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

static const struct InstructionMap inst_aliases_csrrs[] = {
    { .name = "csrr", .args = {"d", "E"}, .code = 0x2073 | (0 << 15) , .mask = 0x707f | (31 << 15), .flags = IMF_SUPPORTED },
    INST_ALIAS_LIST_END,
};

// RV32/64A - Atomi Memory Operations

#define AMO_ARGS_LOAD {"d", "(s)"}
#define AMO_ARGS_STORE {"d", "t", "(s)"}
#define AMO_ARGS_MODIFY {"d", "t", "(s)"}

#define AMO_MAP_4ITEMS(NAME_BASE, CODE_BASE, MASK, MEM_CTL, FLAGS, ARGS) \
    { NAME_BASE, IT_AMO, NOALU, MEM_CTL, nullptr, ARGS , ((CODE_BASE) | 0x00000000), 0xfe00707f, { .flags = FLAGS}, nullptr}, \
    { NAME_BASE ".rl", IT_AMO, NOALU, MEM_CTL, nullptr, ARGS , ((CODE_BASE) | 0x02000000), 0xfe00707f, { .flags = FLAGS}, nullptr}, \
    { NAME_BASE ".aq", IT_AMO, NOALU, MEM_CTL, nullptr, ARGS , ((CODE_BASE) | 0x04000000), 0xfe00707f, { .flags = FLAGS}, nullptr}, \
    { NAME_BASE ".aqrl", IT_AMO, NOALU, MEM_CTL, nullptr, ARGS , ((CODE_BASE) | 0x06000000), 0xfe00707f, { .flags = FLAGS}, nullptr}

static const struct InstructionMap AMO_32_map[] = {
    AMO_MAP_4ITEMS("amoadd.w", 0x0000202f, 0xfe00707f, AC_AMOADD32, FLAGS_AMO_MODIFY, AMO_ARGS_MODIFY),
    AMO_MAP_4ITEMS("amoswap.w", 0x0800202f, 0xfe00707f, AC_AMOSWAP32, FLAGS_AMO_MODIFY, AMO_ARGS_MODIFY),
    AMO_MAP_4ITEMS("lr.w", 0x1000202f, 0xfff0707f, AC_LR32, FLAGS_AMO_LOAD, AMO_ARGS_LOAD),
    AMO_MAP_4ITEMS("sc.w", 0x1800202f, 0xfe00707f, AC_SC32, FLAGS_AMO_STORE, AMO_ARGS_STORE),
    AMO_MAP_4ITEMS("amoxor.w", 0x2000202f, 0xfe00707f, AC_AMOXOR32, FLAGS_AMO_MODIFY, AMO_ARGS_MODIFY),
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    AMO_MAP_4ITEMS("amoor.w", 0x4000202f, 0xfe00707f, AC_AMOOR32, FLAGS_AMO_MODIFY, AMO_ARGS_MODIFY),
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    AMO_MAP_4ITEMS("amoand.w", 0x6000202f, 0xfe00707f, AC_AMOAND32, FLAGS_AMO_MODIFY, AMO_ARGS_MODIFY),
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    AMO_MAP_4ITEMS("amomin.w", 0x8000202f, 0xfe00707f, AC_AMOMIN32, FLAGS_AMO_MODIFY, AMO_ARGS_MODIFY),
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    AMO_MAP_4ITEMS("amomax.w", 0xa000202f, 0xfe00707f, AC_AMOMAX32, FLAGS_AMO_MODIFY, AMO_ARGS_MODIFY),
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    AMO_MAP_4ITEMS("amominu.w", 0xc000202f, 0xfe00707f, AC_AMOMINU32, FLAGS_AMO_MODIFY, AMO_ARGS_MODIFY),
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    AMO_MAP_4ITEMS("amomaxu.w", 0xe000202f, 0xfe00707f, AC_AMOMAXU32, FLAGS_AMO_MODIFY, AMO_ARGS_MODIFY),
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
};

static const struct InstructionMap AMO_64_map[] = {
    AMO_MAP_4ITEMS("amoadd.d", 0x0000302f, 0xfe00707f, AC_AMOADD64, FLAGS_AMO_MODIFY, AMO_ARGS_MODIFY),
    AMO_MAP_4ITEMS("amoswap.d", 0x0800302f, 0xfe00707f, AC_AMOSWAP64, FLAGS_AMO_MODIFY, AMO_ARGS_MODIFY),
    AMO_MAP_4ITEMS("lr.d", 0x1000302f, 0xfff0707f, AC_LR64, FLAGS_AMO_LOAD, AMO_ARGS_LOAD),
    AMO_MAP_4ITEMS("sc.d", 0x1800302f, 0xfe00707f, AC_SC64, FLAGS_AMO_STORE, AMO_ARGS_STORE),
    AMO_MAP_4ITEMS("amoxor.d", 0x2000302f, 0xfe00707f, AC_AMOXOR64, FLAGS_AMO_MODIFY | IMF_RV64, AMO_ARGS_MODIFY),
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    AMO_MAP_4ITEMS("amoor.d", 0x4000302f, 0xfe00707f, AC_AMOOR64, FLAGS_AMO_MODIFY | IMF_RV64, AMO_ARGS_MODIFY),
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    AMO_MAP_4ITEMS("amoand.d", 0x6000302f, 0xfe00707f, AC_AMOAND64, FLAGS_AMO_MODIFY | IMF_RV64, AMO_ARGS_MODIFY),
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    AMO_MAP_4ITEMS("amomin.d", 0x8000302f, 0xfe00707f, AC_AMOMIN64, FLAGS_AMO_MODIFY | IMF_RV64, AMO_ARGS_MODIFY),
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    AMO_MAP_4ITEMS("amomax.d", 0xa000302f, 0xfe00707f, AC_AMOMAX64, FLAGS_AMO_MODIFY | IMF_RV64, AMO_ARGS_MODIFY),
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    AMO_MAP_4ITEMS("amominu.d", 0xc000302f, 0xfe00707f, AC_AMOMINU64, FLAGS_AMO_MODIFY | IMF_RV64, AMO_ARGS_MODIFY),
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    AMO_MAP_4ITEMS("amomaxu.d", 0xe000302f, 0xfe00707f, AC_AMOMAXU64, FLAGS_AMO_MODIFY | IMF_RV64, AMO_ARGS_MODIFY),
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
    IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN, IM_UNKNOWN,
};

static const struct InstructionMap AMO_map[] = {
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"amo-32", IT_R, NOALU, NOMEM, AMO_32_map, {}, 0x0002027, 0x0000707f, { .subfield = {7, 25} }, nullptr}, // OP-32
    {"amo-64", IT_R, NOALU, NOMEM, AMO_64_map, {}, 0x0003027, 0x0000707f, { .subfield = {7, 25} }, nullptr}, // OP-32
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
};

#undef AMO_MAP_4ITEMS

static const struct InstructionMap LOAD_map[] = {
    {"lb",  IT_I, { .alu_op=AluOp::ADD }, AC_I8,  nullptr, {"d", "o(s)"}, 0x00000003,0x0000707f, { .flags = FLAGS_ALU_I_LOAD }, nullptr}, // LB
    {"lh",  IT_I, { .alu_op=AluOp::ADD }, AC_I16, nullptr, {"d", "o(s)"}, 0x00001003,0x0000707f, { .flags = FLAGS_ALU_I_LOAD }, nullptr}, // LH
    {"lw",  IT_I, { .alu_op=AluOp::ADD }, AC_I32, nullptr, {"d", "o(s)"}, 0x00002003,0x0000707f, { .flags = FLAGS_ALU_I_LOAD }, nullptr}, // LW
    {"ld",  IT_I, { .alu_op=AluOp::ADD }, AC_I64, nullptr, {"d", "o(s)"}, 0x00003003,0x0000707f, { .flags = FLAGS_ALU_I_LOAD | IMF_RV64}, nullptr}, // LD
    {"lbu", IT_I, { .alu_op=AluOp::ADD }, AC_U8,  nullptr, {"d", "o(s)"}, 0x00004003,0x0000707f, { .flags = FLAGS_ALU_I_LOAD }, nullptr}, // LBU
    {"lhu", IT_I, { .alu_op=AluOp::ADD }, AC_U16, nullptr, {"d", "o(s)"}, 0x00005003,0x0000707f, { .flags = FLAGS_ALU_I_LOAD }, nullptr}, // LHU
    {"lwu", IT_I, { .alu_op=AluOp::ADD }, AC_U32, nullptr, {"d", "o(s)"}, 0x00006003,0x0000707f, { .flags = FLAGS_ALU_I_LOAD  | IMF_RV64}, nullptr}, // LWU
    IM_UNKNOWN,
};

static const struct InstructionMap SRI_map[] = { // 0xfe00707f mask changed to 0xfc00707f to support RV64I
    {"srli", IT_I, { .alu_op=AluOp::SR }, NOMEM, nullptr, {"d", "s", ">"}, 0x00005013,0xfc00707f, { .flags = FLAGS_ALU_I }, nullptr}, // SRLI
    {"srai", IT_I, { .alu_op=AluOp::SR }, NOMEM, nullptr, {"d", "s", ">"}, 0x40005013,0xfc00707f, { .flags = (FLAGS_ALU_I | IMF_ALU_MOD) }, nullptr}, // SRAI
};

static const struct InstructionMap OP_IMM_map[] = {
    {"addi",  IT_I, { .alu_op=AluOp::ADD },  NOMEM, nullptr, {"d", "s", "j"}, 0x00000013,0x0000707f, { .flags = FLAGS_ALU_I }, inst_aliases_addi}, // ADDI
    {"slli",  IT_I, { .alu_op=AluOp::SLL },  NOMEM, nullptr, {"d", "s", ">"}, 0x00001013,0xfc00707f, { .flags = FLAGS_ALU_I }, nullptr}, // SLLI
    {"slti",  IT_I, { .alu_op=AluOp::SLT },  NOMEM, nullptr, {"d", "s", "j"}, 0x00002013,0x0000707f, { .flags = FLAGS_ALU_I }, nullptr}, // SLTI
    {"sltiu", IT_I, { .alu_op=AluOp::SLTU }, NOMEM, nullptr, {"d", "s", "j"}, 0x00003013,0x0000707f, { .flags = FLAGS_ALU_I }, inst_aliases_sltiu}, // SLTIU
    {"xori",  IT_I, { .alu_op=AluOp::XOR },  NOMEM, nullptr, {"d", "s", "j"}, 0x00004013,0x0000707f, { .flags = FLAGS_ALU_I }, inst_aliases_xori}, // XORI
    {"sri",   IT_I, NOALU,       NOMEM, SRI_map,              {}, 0x00005013, 0xbe00707f, { .subfield = {1, 30} }, nullptr}, // SRLI, SRAI
    {"ori",   IT_I, { .alu_op=AluOp::OR },   NOMEM, nullptr, {"d", "s", "j"}, 0x00006013,0x0000707f, { .flags = FLAGS_ALU_I }, nullptr}, // ORI
    {"andi",  IT_I, { .alu_op=AluOp::AND },  NOMEM, nullptr, {"d", "s", "j"}, 0x00007013,0x0000707f, { .flags = FLAGS_ALU_I }, inst_aliases_andi}, // ANDI
};

static const struct InstructionMap STORE_map[] = {
    {"sb", IT_S, { .alu_op=AluOp::ADD }, AC_U8,  nullptr, {"t", "q(s)"}, 0x00000023, 0x0000707f, { .flags = FLAGS_ALU_I_STORE }, nullptr}, // SB
    {"sh", IT_S, { .alu_op=AluOp::ADD }, AC_U16, nullptr, {"t", "q(s)"}, 0x00001023, 0x0000707f, { .flags = FLAGS_ALU_I_STORE }, nullptr}, // SH
    {"sw", IT_S, { .alu_op=AluOp::ADD }, AC_U32, nullptr, {"t", "q(s)"}, 0x00002023, 0x0000707f, { .flags = FLAGS_ALU_I_STORE }, nullptr}, // SW
    {"sd", IT_S, { .alu_op=AluOp::ADD }, AC_U64, nullptr, {"t", "q(s)"}, 0x00003023, 0x0000707f, { .flags = FLAGS_ALU_I_STORE | IMF_RV64}, nullptr}, // SD
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
};

static const struct InstructionMap ADD_map[] = {
    {"add", IT_R, { .alu_op=AluOp::ADD }, NOMEM, nullptr, {"d", "s", "t"}, 0x00000033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }, nullptr},
    {"sub", IT_R, { .alu_op=AluOp::ADD }, NOMEM, nullptr, {"d", "s", "t"}, 0x40000033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD | IMF_ALU_MOD }, inst_aliases_sub},
};

static const struct InstructionMap SR_map[] = {
    {"srl", IT_R, { .alu_op=AluOp::SR }, NOMEM, nullptr, {"d", "s", "t"}, 0x00005033,0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }, nullptr}, // SRL
    {"sra", IT_R, { .alu_op=AluOp::SR }, NOMEM, nullptr, {"d", "s", "t"}, 0x40005033,0xfe00707f,  { .flags = FLAGS_ALU_T_R_STD | IMF_ALU_MOD }, nullptr}, // SRA
};

static const struct InstructionMap OP_ALU_map[] = {
    {"add/sub", IT_R, NOALU,    NOMEM, ADD_map,              {}, 0x00000033, 0xbe00707f, { .subfield = {1, 30} }, nullptr},
    {"sll",  IT_R, { .alu_op=AluOp::SLL },  NOMEM, nullptr, {"d", "s", "t"}, 0x00001033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }, nullptr}, // SLL
    {"slt",  IT_R, { .alu_op=AluOp::SLT },  NOMEM, nullptr, {"d", "s", "t"}, 0x00002033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }, inst_aliases_slt}, // SLT
    {"sltu", IT_R, { .alu_op=AluOp::SLTU }, NOMEM, nullptr, {"d", "s", "t"}, 0x00003033,0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }, inst_aliases_sltu}, // SLTU
    {"xor",  IT_R, { .alu_op=AluOp::XOR },  NOMEM, nullptr, {"d", "s", "t"}, 0x00004033,0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }, nullptr}, // XOR
    {"sr",   IT_R, NOALU,       NOMEM,  SR_map,              {}, 0x00005033, 0xbe00707f, { .subfield = {1, 30} }, nullptr}, // SRL, SRA
    {"or",   IT_R, { .alu_op=AluOp::OR },   NOMEM, nullptr, {"d", "s", "t"}, 0x00006033,0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }, nullptr}, // OR
    {"and",  IT_R, { .alu_op=AluOp::AND },  NOMEM, nullptr, {"d", "s", "t"}, 0x00007033,0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }, nullptr}, // AND
};

// RV32M
#define MUL_MAP_ITEM(NAME, OP, CODE) \
    { NAME, IT_R, { .mul_op = (OP) }, NOMEM, nullptr, {"d", "s", "t"}, (0x02000033 | (CODE)), 0xfe00707f, { .flags = (FLAGS_ALU_T_R_STD | IMF_MUL) }, nullptr}

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
    {"alu", IT_R, NOALU, NOMEM, OP_ALU_map, {}, 0x00000033, 0x0000707f, { .subfield = {3, 12} }, nullptr},
    {"mul", IT_R, NOALU, NOMEM, OP_MUL_map, {}, 0x02000033, 0xfc00707f, { .subfield = {3, 12} }, nullptr},
};

static const struct InstructionMap BRANCH_map[] = {
    {"beq",  IT_B, { .alu_op=AluOp::ADD }, NOMEM,  nullptr, {"s", "t", "p"}, 0x00000063,0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT | IMF_ALU_MOD }, inst_aliases_beq}, // BEQ
    {"bne",  IT_B, { .alu_op=AluOp::ADD }, NOMEM,  nullptr, {"s", "t", "p"}, 0x00001063, 0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT | IMF_ALU_MOD | IMF_BJ_NOT }, inst_aliases_bne}, // BNE
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"blt",  IT_B, { .alu_op=AluOp::SLT }, NOMEM,  nullptr, {"s", "t", "p"}, 0x00004063, 0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT | IMF_BJ_NOT }, inst_aliases_blt}, // BLT
    {"bge",  IT_B, { .alu_op=AluOp::SLT }, NOMEM,  nullptr, {"s", "t", "p"}, 0x00005063,0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT }, inst_aliases_bge}, // BGE
    {"bltu", IT_B, { .alu_op=AluOp::SLTU }, NOMEM, nullptr, {"s", "t", "p"}, 0x00006063, 0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT | IMF_BJ_NOT }, inst_aliases_bltu}, // BLTU
    {"bgeu", IT_B, { .alu_op=AluOp::SLTU }, NOMEM, nullptr, {"s", "t", "p"}, 0x00007063,0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT }, inst_aliases_bgeu}, // BGEU
};

// Spec vol. 1: 2.8
static const struct InstructionMap ENVIRONMENT_AND_BREAKPOINTS_map[] = {
    {"ecall", IT_I, NOALU, NOMEM, nullptr, {}, 0x00000073, 0xffffffff, { .flags = IMF_SUPPORTED | IMF_EXCEPTION | IMF_ECALL }, nullptr},
    {"ebreak", IT_I, NOALU, NOMEM, nullptr, {}, 0x00100073, 0xffffffff, { .flags = IMF_SUPPORTED | IMF_EXCEPTION | IMF_EBREAK }, nullptr},
};

// Priviledged system isntructions, only 5-bits (29:25) are decoded for now.
// Full decode is should cover 128 entries (31:25) but we radly support hypervisor even in future
static const struct InstructionMap SYSTEM_PRIV_map[] = {
    {"environment_and_breakpoints", IT_I, NOALU, NOMEM, ENVIRONMENT_AND_BREAKPOINTS_map, {}, 0x00000073, 0xffffffff, { .subfield = {1, 20} }, nullptr},
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"sret", IT_I, NOALU, NOMEM, nullptr, {}, 0x10200073, 0xffffffff, { .flags = IMF_SUPPORTED | IMF_XRET }, nullptr},
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
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"mret", IT_I, NOALU, NOMEM, nullptr, {}, 0x30200073, 0xffffffff, { .flags = IMF_SUPPORTED | IMF_XRET }, nullptr},
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
};

#define CSR_MAP_ITEM(NAME, SOURCE, CODE, ALU_OP,  EXTRA_FLAGS, ALIASES) \
    { NAME, Instruction::ZICSR, { .alu_op=AluOp::ALU_OP }, NOMEM, nullptr,  {"d", "E", SOURCE}, 0x00000073 | (CODE), 0x0000707f, { .flags = IMF_SUPPORTED | IMF_CSR | IMF_REGWRITE | IMF_ALU_REQ_RS | (EXTRA_FLAGS) }, ALIASES}

static const struct InstructionMap SYSTEM_map[] = {
    {"system_priviledged", IT_I, NOALU, NOMEM, SYSTEM_PRIV_map, {}, 0x00000073, 0xffffffff, { .subfield = {5, 25} }, nullptr},
    CSR_MAP_ITEM("csrrw", "s", 0x1000, ADD, 0, inst_aliases_csrrw),
    CSR_MAP_ITEM("csrrs", "s", 0x2000, OR, IMF_CSR_TO_ALU, inst_aliases_csrrs),
    CSR_MAP_ITEM("csrrc", "s", 0x3000, AND, IMF_CSR_TO_ALU | IMF_ALU_MOD, nullptr),
    IM_UNKNOWN,
    CSR_MAP_ITEM("csrrwi", "Z", 0x5000, ADD, IMF_ALU_RS_ID, nullptr),
    CSR_MAP_ITEM("csrrsi", "Z", 0x6000, OR, IMF_ALU_RS_ID | IMF_CSR_TO_ALU, nullptr),
    CSR_MAP_ITEM("csrrci", "Z", 0x7000, AND, IMF_ALU_RS_ID | IMF_CSR_TO_ALU | IMF_ALU_MOD, nullptr),
};

#undef CSR_MAP_ITEM

static const struct InstructionMap MISC_MEM_map[] = {
    {"fence", IT_I, NOALU, AC_CACHE_OP, nullptr, {}, 0x0000000f, 0x0000707f, { .flags = IMF_SUPPORTED | IMF_MEM }, nullptr},
    {"fence.i", IT_I, NOALU, AC_CACHE_OP, nullptr, {}, 0x000100f, 0x0000707f, { .flags = IMF_SUPPORTED | IMF_MEM }, nullptr},
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
};

// RV64I specific operations

static const struct InstructionMap SRI_32_map[] = {
    {"srliw", IT_I, { .alu_op=AluOp::SR }, NOMEM, nullptr, {"d", "s", ">"}, 0x0000501b,0xfe00707f, { .flags = FLAGS_ALU_I | IMF_FORCE_W_OP | IMF_RV64 }, nullptr}, // SRLIW
    {"sraiw", IT_I, { .alu_op=AluOp::SR }, NOMEM, nullptr, {"d", "s", ">"}, 0x4000501b,0xfe00707f, { .flags = FLAGS_ALU_I | IMF_ALU_MOD | IMF_FORCE_W_OP | IMF_RV64 }, nullptr}, // SRAIW
};

static const struct InstructionMap OP_IMM_32_map[] = {
    {"addiw", IT_I, { .alu_op=AluOp::ADD },  NOMEM, nullptr, {"d", "s", "j"}, 0x0000001b,0x0000707f, { .flags = FLAGS_ALU_I | IMF_FORCE_W_OP | IMF_RV64 }, inst_aliases_addiw}, // ADDIW
    {"slliw", IT_I, { .alu_op=AluOp::SLL },  NOMEM, nullptr, {"d", "s", ">"}, 0x0000101b,0xfe00707f, { .flags = FLAGS_ALU_I | IMF_FORCE_W_OP | IMF_RV64 }, nullptr}, // SLLIW
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"sriw",   IT_I, NOALU,       NOMEM, SRI_32_map,              {}, 0x0000501b, 0xbe00707f, { .subfield = {1, 30} }, nullptr}, // SRLIW, SRAIW
    IM_UNKNOWN,
    IM_UNKNOWN,
};

static const struct InstructionMap ADD_32_map[] = {
    {"addw", IT_R, { .alu_op=AluOp::ADD }, NOMEM, nullptr, {"d", "s", "t"}, 0x0000003b, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD | IMF_FORCE_W_OP | IMF_RV64 }, nullptr},
    {"subw", IT_R, { .alu_op=AluOp::ADD }, NOMEM, nullptr, {"d", "s", "t"}, 0x4000003b, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD | IMF_ALU_MOD | IMF_FORCE_W_OP | IMF_RV64 }, inst_aliases_subw},
};

static const struct InstructionMap SR_32_map[] = {
    {"srlw", IT_R, { .alu_op=AluOp::SR }, NOMEM, nullptr, {"d", "s", "t"}, 0x0000503b,0xfe00707f, { .flags = FLAGS_ALU_T_R_STD | IMF_FORCE_W_OP | IMF_RV64 }, nullptr}, // SRL
    {"sraw", IT_R, { .alu_op=AluOp::SR }, NOMEM, nullptr, {"d", "s", "t"}, 0x4000503b,0xfe00707f,  { .flags = FLAGS_ALU_T_R_STD | IMF_ALU_MOD | IMF_FORCE_W_OP | IMF_RV64 }, nullptr}, // SRA
};

static const struct InstructionMap OP_ALU_32_map[] = {
    {"addw/subw", IT_R, NOALU,    NOMEM, ADD_32_map,              {}, 0x0000003b, 0xbe00707f, { .subfield = {1, 30} }, nullptr},
    {"sllw",  IT_R, { .alu_op=AluOp::SLL },  NOMEM, nullptr, {"d", "s", "t"}, 0x0000103b, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD | IMF_FORCE_W_OP | IMF_RV64 }, nullptr}, // SLL
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"srw",   IT_R, NOALU,       NOMEM,  SR_32_map,              {}, 0x0000503b, 0xbe00707f, { .subfield = {1, 30} }, nullptr}, // SRL, SRA
    IM_UNKNOWN,
    IM_UNKNOWN,
};

// RV64M
#define MUL_32_MAP_ITEM(NAME, OP, CODE) \
    { NAME, IT_R, { .mul_op = (OP) }, NOMEM, nullptr, {"d", "s", "t"}, (0x0200003b | (CODE)), 0xfe00707f, { .flags = (FLAGS_ALU_T_R_STD | IMF_MUL | IMF_FORCE_W_OP | IMF_RV64 ) }, nullptr}

static const struct InstructionMap OP_MUL_32_map[] = {
    MUL_32_MAP_ITEM("mulw",     MulOp::MUL,     0x0000),
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    MUL_32_MAP_ITEM("divw",     MulOp::DIV,     0x4000),
    MUL_32_MAP_ITEM("divuw",    MulOp::DIVU,    0x5000),
    MUL_32_MAP_ITEM("remw",     MulOp::REM,     0x6000),
    MUL_32_MAP_ITEM("remuw",    MulOp::REMU,    0x7000),
};

static const struct InstructionMap OP_32_map[] = {
    {"aluw", IT_R, NOALU, NOMEM, OP_ALU_32_map, {}, 0x00000033, 0x0000707f, { .subfield = {3, 12} }, nullptr},
    {"mulw", IT_R, NOALU, NOMEM, OP_MUL_32_map, {}, 0x02000033, 0xfc00707f, { .subfield = {3, 12} }, nullptr},
};

// Full, uncomprese, instructions top level map

static const struct InstructionMap I_inst_map[] = {
    {"load", IT_I, NOALU, NOMEM, LOAD_map, {}, 0x03, 0x7f, { .subfield = {3, 12} }, nullptr}, // LOAD
    IM_UNKNOWN, // LOAD-FP
    IM_UNKNOWN, // custom-0
    {"misc-mem", IT_I, NOALU, NOMEM, MISC_MEM_map, {}, 0x0f, 0x7f, { .subfield = {3, 12} }, nullptr}, // MISC-MEM
    {"op-imm", IT_I, NOALU, NOMEM, OP_IMM_map, {}, 0x13, 0x7f, { .subfield = {3, 12} }, nullptr}, // OP-IMM
    {"auipc", IT_U, { .alu_op=AluOp::ADD }, NOMEM, nullptr, {"d", "u"}, 0x17, 0x7f, { .flags = IMF_SUPPORTED | IMF_ALUSRC | IMF_REGWRITE | IMF_PC_TO_ALU }, nullptr}, // AUIPC
    {"op-imm-32", IT_I, NOALU, NOMEM, OP_IMM_32_map, {}, 0x1b, 0x7f, { .subfield = {3, 12} }, nullptr}, // OP-IMM-32    IM_UNKNOWN, // OP-IMM-32
    IM_UNKNOWN, // 48b
    {"store", IT_I, NOALU, NOMEM, STORE_map, {}, 0x23, 0x7f, { .subfield = {3, 12} }, nullptr}, // STORE
    IM_UNKNOWN, // STORE-FP
    IM_UNKNOWN, // custom-1
    {"amo", IT_R, NOALU, NOMEM, AMO_map, {}, 0x2f, 0x7f, { .subfield = {3, 12} }, nullptr}, // OP-32
    {"op", IT_R, NOALU, NOMEM, OP_map, {}, 0x33, 0x7f, { .subfield = {1, 25} }, nullptr}, // OP
    {"lui", IT_U, { .alu_op=AluOp::ADD }, NOMEM, nullptr, {"d", "u"}, 0x37, 0x7f, { .flags = IMF_SUPPORTED | IMF_ALUSRC | IMF_REGWRITE }, nullptr}, // LUI
    {"op-32", IT_R, NOALU, NOMEM, OP_32_map, {}, 0x3b, 0x7f, { .subfield = {1, 25} }, nullptr}, // OP-32
    IM_UNKNOWN, // 64b
    IM_UNKNOWN, // MADD
    IM_UNKNOWN, // MSUB
    IM_UNKNOWN, // NMSUB
    IM_UNKNOWN, // NMADD
    IM_UNKNOWN, // OP-FP
    IM_UNKNOWN, // reserved
    IM_UNKNOWN, // custom-2/rv128
    IM_UNKNOWN, // 48b
    {"branch", IT_B, NOALU, NOMEM, BRANCH_map, {}, 0x63, 0x7f, { .subfield = {3, 12} }, nullptr}, // BRANCH
    {"jalr", IT_I, { .alu_op=AluOp::ADD }, NOMEM, nullptr, {"d", "o(s)"}, 0x67, 0x7f, { .flags =
IMF_SUPPORTED | IMF_REGWRITE | IMF_BRANCH_JALR | IMF_ALUSRC | IMF_ALU_REQ_RS }, inst_aliases_jalr}, // JALR
    IM_UNKNOWN, // reserved
    {"jal", IT_J, { .alu_op=AluOp::ADD }, NOMEM, nullptr, {"d", "a"}, 0x6f, 0x7f, { .flags =
IMF_SUPPORTED |
IMF_REGWRITE | IMF_JUMP | IMF_PC_TO_ALU | IMF_ALUSRC }, inst_aliases_jal}, // JAL
    {"system", IT_I, NOALU, NOMEM, SYSTEM_map, {}, 0x73, 0x7f, { .subfield = {3, 12} }, nullptr}, // SYSTEM
    IM_UNKNOWN, // reserved
    IM_UNKNOWN, // custom-3/rv128
    IM_UNKNOWN, // >= 80b
};

static const struct InstructionMap C_inst_map[] = {
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"i", IT_UNKNOWN, NOALU, NOMEM, I_inst_map, {}, 0x3, 0x3, { .subfield = {5, 2} }, nullptr},
};

static const struct InstructionMap C_inst_unknown = IM_UNKNOWN;

// clang-format on

const BitField instruction_map_opcode_field = { 2, 0 };

static inline const struct InstructionMap &InstructionMapFind(uint32_t code) {
    const struct InstructionMap *im = &C_inst_map[instruction_map_opcode_field.decode(code)];
    while (im->subclass != nullptr) {
        im = &im->subclass[im->subfield.decode(code)];
    }
    if ((code ^ im->code) & im->mask) {
        return C_inst_unknown;
    }
    return *im;
}

const std::array<const QString, 36> RECOGNIZED_PSEUDOINSTRUCTIONS {
    "nop",    "la",     "li",     "sext.b", "sext.h",
    "zext.h", "zext.w", "call", "tail"
};

bool Instruction::symbolic_registers_enabled = false;
const Instruction Instruction::NOP = Instruction(0x00000013);
const Instruction Instruction::UNKNOWN_INST = Instruction(0x0);

Instruction::Instruction() {
    this->dt = 0;
}

Instruction::Instruction(uint32_t inst) {
    this->dt = inst;
}

Instruction::Instruction(const Instruction &i) {
    this->dt = i.data();
}

#define MASK(LEN, OFF) ((this->dt >> (OFF)) & ((1 << (LEN)) - 1))

uint8_t Instruction::opcode() const {
    return (uint8_t)MASK(7, 0); // Does include the 2 bits marking it's not a
                                // 16b instruction
}

uint8_t Instruction::rs() const {
    return (uint8_t)MASK(5, 15);
}

uint8_t Instruction::rt() const {
    return (uint8_t)MASK(5, 20);
}

uint8_t Instruction::rd() const {
    return (uint8_t)MASK(5, 7);
}

uint8_t Instruction::shamt() const {
    return this->rt();
}

uint16_t Instruction::funct() const {
    return uint16_t(MASK(7, 25) << 3 | MASK(3, 12));
}

CSR::Address Instruction::csr_address() const {
    return CSR::Address(MASK(12, 20));
}

int32_t Instruction::immediate() const {
    int32_t ret = 0;
    switch (this->type()) {
    case R: break;
    case I: ret = extend(MASK(12, 20), 12); break;
    case S: ret = extend(MASK(7, 25) << 5 | MASK(5, 7), 12); break;
    case B:
        ret = extend(MASK(4, 8) << 1 | MASK(6, 25) << 5 | MASK(1, 7) << 11 | MASK(1, 31) << 12, 13);
        break;
    case U: ret = extend(MASK(20, 12) << 12, 32); break;
    case J:
        ret = extend(
            MASK(10, 21) << 1 | MASK(1, 20) << 11 | MASK(8, 12) << 12 | MASK(1, 31) << 20, 21);
        break;
    case ZICSR:
    case AMO:
    case UNKNOWN: break;
    }
    return ret;
}

Address Instruction::address() const {
    return Address(MASK(26, 0));
}

uint32_t Instruction::data() const {
    return this->dt;
}

bool Instruction::imm_sign() const {
    return this->dt >> 31;
}

enum Instruction::Type Instruction::type() const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    return im.type;
}

enum InstructionFlags Instruction::flags() const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    return (enum InstructionFlags)im.flags;
}
AluCombinedOp Instruction::alu_op() const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    return im.alu;
}

enum AccessControl Instruction::mem_ctl() const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    return im.mem_ctl;
}

void Instruction::flags_alu_op_mem_ctl(
    InstructionFlags &flags,
    AluCombinedOp &alu_op,
    AccessControl &mem_ctl) const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    flags = (enum InstructionFlags)im.flags;
    alu_op = im.alu;
    mem_ctl = im.mem_ctl;
}

bool Instruction::operator==(const Instruction &c) const {
    return (this->data() == c.data());
}

bool Instruction::operator!=(const Instruction &c) const {
    return !this->operator==(c);
}

Instruction &Instruction::operator=(const Instruction &c) {
    if (this != &c) { this->dt = c.data(); }
    return *this;
}

QString Instruction::to_str(Address inst_addr) const {
    const InstructionMap &im = InstructionMapFind(dt);
    // TODO there are exception where some fields are zero and such so we should
    // not print them in such case
    SANITY_ASSERT(argdesbycode_filled, QString("argdesbycode_filled not initialized"));
    QString res;
    QString next_delim = " ";
    if (im.type == UNKNOWN) { return { "unknown" }; }
    if (this->dt == NOP.dt) { return { "nop" }; }

    res += im.name;
    for (const QString &arg_string : im.args) {
        res += next_delim;
        next_delim = ", ";
        for (int pos = 0; pos < arg_string.size(); pos += 1) {
            char arg_letter = arg_string[pos].toLatin1();
            const ArgumentDesc *arg_desc = arg_desc_by_code[(unsigned char)arg_letter];
            if (arg_desc == nullptr) {
                res += arg_letter;
                continue;
            }
            auto field = (int32_t)arg_desc->arg.decode(this->dt);
            if (arg_desc->min < 0) {
                field = extend(field, [&]() {
                    int sum = (int)arg_desc->arg.shift;
                    for (auto chunk : arg_desc->arg) {
                        sum += chunk.count;
                    }
                    return sum;
                }());
            }
            switch (arg_desc->kind) {
            case 'g': {
                if (symbolic_registers_enabled) {
                    res += QString(Rv_regnames[field]);
                } else {
                    res += "x" + QString::number(field);
                }
                break;
            }
            case 'p':
            case 'a': {
                field += (int32_t)inst_addr.get_raw();
                res.append(str::asHex(uint32_t(field)));
                break;
            }
            case 'o':
            case 'n': {
                if (arg_desc->min < 0) {
                    res += QString::number((int32_t)field, 10);
                } else {
                    res.append(str::asHex(uint32_t(field)));
                }
                break;
            }
            case 'E': {
                if (symbolic_registers_enabled) {
                    try {
                        res += CSR::REGISTERS[CSR::REGISTER_MAP.at(CSR::Address(field))].name;
                    } catch (std::out_of_range &e) {
                        res.append(str::asHex(field));
                    }
                } else {
                    res.append(str::asHex(field));
                }
                break;
            }
            }
        }
    }
    return res;
}

QMultiMap<QString, uint32_t> str_to_instruction_code_map;

static void instruction_from_string_build_base_aliases(uint32_t base_code,
                uint32_t base_mask, const InstructionMap *ia) {
    for (; ia->name != nullptr; ia++) {
        if ((ia->code ^ base_code) & base_mask) {
            ERROR("alias code mismatch %s computed 0x%08" PRIx32 " (mask 0x%08" PRIx32 ") found 0x%08" PRIx32,
                  ia->name, base_code, base_mask, ia->code);
            continue;
        }
        if (~ia->mask & base_mask) {
            ERROR("aliase code mismatch %s computed 0x%08" PRIx32 " (mask 0x%08" PRIx32 ") found 0x%08" PRIx32 " with too wide mask 0x%08" PRIx32,
                  ia->name, base_code, base_mask, ia->code, ia->mask);
            continue;
        }
        bool found = false;
        auto iter_range = str_to_instruction_code_map.equal_range(ia->name);
        for (auto i = iter_range.first; i != iter_range.second; i += 1) {
            if (i.value() == base_code) {
                found = true;
                break;
            }
        }
        if (found)
            continue;

        // store base code, the iteration over alliases is required anyway
        str_to_instruction_code_map.insert(ia->name, base_code);
    }
}

void instruction_from_string_build_base(
    const InstructionMap *im,
    BitField field,
    uint32_t base_code, uint32_t base_mask) {
    uint32_t code;
    uint8_t bits = field.count;
    uint8_t shift = field.offset;

    base_mask |= (((uint32_t)1 << bits) - 1) << shift;

    for (unsigned int i = 0; i < 1U << bits; i++, im++) {
        code = base_code | (i << shift);
        if (im->subclass) {
            instruction_from_string_build_base(im->subclass, im->subfield, code, base_mask);
            continue;
        }
        if (!(im->flags & IMF_SUPPORTED)) { continue; }
        if ((im->code ^ code) & base_mask) {
            ERROR("code mismatch %s computed 0x%08" PRIx32 " (mask 0x%08" PRIx32 ") found 0x%08" PRIx32,
                  im->name, code, base_mask, im->code);
            continue;
        }
        if (~im->mask & base_mask) {
            ERROR("code mismatch %s computed 0x%08" PRIx32 " (mask 0x%08" PRIx32 ") found 0x%08" PRIx32 " with too wide mask 0x%08" PRIx32,
                  im->name, code, base_mask, im->code, im->mask);
            continue;
        }
        str_to_instruction_code_map.insert(im->name, im->code);

        if (im->aliases != nullptr)
            instruction_from_string_build_base_aliases(im->code, im->mask, im->aliases);
    }
#if 0
    for (auto i = str_to_instruction_code_map.begin();
         i != str_to_instruction_code_map.end(); i++)
        std::cout << i.key().toStdString() << ' ';
#endif
}

void instruction_from_string_build_base() {
    return instruction_from_string_build_base(C_inst_map, instruction_map_opcode_field, 0, 0);
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
        for (size_t i = 0; i < Rv_regnames.size(); i++) {
            size_t len = std::strlen(Rv_regnames[i]);
            if (size_t(data.size()) < len) continue;
            if (std::strncmp(data.data(), Rv_regnames[i], len) == 0) {
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
    const QStringList mods = {"%hi(", "%lo("};
    const Instruction::Modifier modcodes[] = {
      machine::Instruction::Modifier::COMPOSED_IMM_UPPER,
      machine::Instruction::Modifier::COMPOSED_IMM_LOWER
    };
    int i = 0, e = fl.size() , a = 0;

    for (unsigned m=0; m<mods.size(); ++m) {
      if (fl.startsWith(mods[m]) && fl.endsWith(")")) {
        pseudo_mod = modcodes[m];
        i = mods[m].length();
        e--;
        a = 1;
        break;
      }
    }

    for (; i < e; i++) {
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

    reloc->append(new RelocExpression(
        inst_addr, expression, offset, adesc->min, adesc->max, &adesc->arg, filename, line,
        pseudo_mod));
    if (chars_taken != nullptr) { *chars_taken = i + a; }
}

size_t Instruction::code_from_tokens(
    uint32_t *code,
    size_t buffsize,
    TokenizedInstruction &inst,
    RelocExpressionList *reloc,
    bool pseudoinst_enabled) {
    if (str_to_instruction_code_map.isEmpty()) { instruction_from_string_build_base(); }

    Instruction result = base_from_tokens(inst, reloc);
    if (result.data() != 0) {
        if (result.size() > buffsize) {
            // NOTE: this is bug, not user error.
            throw ParseError("insufficient buffer size to write parsed instruction");
        }
        *code = result.data();
        return result.size();
    }

    if (pseudoinst_enabled) {
        size_t pseudo_result = pseudo_from_tokens(code, buffsize, inst, reloc);
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
    if ((inst.base == QLatin1String("la")) && (buffsize >= 8)) {
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

    if ((inst.base == QLatin1String("li")) && (buffsize >= 8)) {
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

    if ((inst.base == QLatin1String("call")) && (buffsize >= 8)) {
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

    if ((inst.base == QLatin1String("tail")) && (buffsize >= 8)) {
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

    if (inst.base[0] == 's') {
        if ((inst.base == QLatin1String("sext.b")) && (buffsize >= 8)) {
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
        if ((inst.base == QLatin1String("sext.h")) && (buffsize >= 8)) {
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
    }
    if (inst.base[0] == 'z') {
        if ((inst.base == QLatin1String("zext.h")) && (buffsize >= 8)) {
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
        if ((inst.base == QLatin1String("zext.w")) && (buffsize >= 8)) {
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

static void instruction_code_map_next_im(const InstructionMap *&im, bool &processing_aliases) {
    if (!processing_aliases) {
        processing_aliases = true;
        im = im->aliases;
    } else {
        im++;
        if (im->name == nullptr)
            im = nullptr;
    }
}

Instruction Instruction::base_from_tokens(
    const TokenizedInstruction &inst,
    RelocExpressionList *reloc,
    Modifier pseudo_mod,
    uint64_t initial_immediate_value) {
    int rethrow = false;
    ParseError parse_error = ParseError("no match for arguments combination found");
    auto iter_range = str_to_instruction_code_map.equal_range(inst.base);
    if (iter_range.first == iter_range.second) {
        DEBUG("Base instruction of the name %s not found.", qPrintable(inst.base));
        return Instruction::UNKNOWN_INST;
    }
    // Process all codes associated with given instruction name and try matching the supplied
    // instruction field tokens to fields. First matching instruction is used.
    for (auto it = iter_range.first; it != iter_range.second; it++) {
        uint32_t inst_code = it.value();
        bool processing_aliases = false;

        const InstructionMap *im = &InstructionMapFind(inst_code);
        for ( ; im != nullptr; instruction_code_map_next_im(im, processing_aliases) ) {
            if (inst.base != im->name)
                continue;

            try {
                 inst_code = im->code;

                 if (inst.fields.count() != (int)im->args.size()) {
                     if (!rethrow) {
                         parse_error = ParseError("number of arguments does not match");
                         rethrow = true;
                     }
                     continue;
                 }

                for (int field_index = 0; field_index < (int)im->args.size(); field_index++) {
                    const QString &arg = im->args[field_index];
                    QString field_token = inst.fields[field_index];
                    inst_code |= parse_field(
                        field_token, arg, inst.address, reloc, inst.filename, inst.line, pseudo_mod,
                        initial_immediate_value);
                }
                return Instruction(inst_code);
            } catch(ParseError &pe) {
                rethrow = true;
                parse_error = pe;
            }
        }
    }

    if (rethrow) {
        throw parse_error;
    }

    DEBUG(
        "Base instruction of the name %s not matched to any known base format.",
        qPrintable(inst.base));
    // Another instruction format for this base may be found in pseudoinstructions.
    return Instruction::UNKNOWN_INST;
}

uint16_t parse_csr_address(const QString &field_token, uint &chars_taken);

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
    const QString &arg,
    Address inst_addr,
    RelocExpressionList *reloc,
    const QString &filename,
    unsigned int line,
    Modifier pseudo_mod,
    uint64_t initial_immediate_value) {
    uint32_t inst_code = 0;
    for (QChar ao : arg) {
        bool need_reloc = false;
        uint a = ao.toLatin1();
        if (!a) { continue; }
        field_token = field_token.trimmed();
        const ArgumentDesc *adesc = arg_desc_by_code[a];
        if (adesc == nullptr) {
            if (!field_token.count()) { throw ParseError("empty argument encountered"); }
            if (field_token.at(0) != ao) {
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
        case 'E': val = parse_csr_address(field_token, chars_taken); break;
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

uint16_t parse_csr_address(const QString &field_token, uint &chars_taken) {
    if (field_token.at(0).isLetter()) {
        // TODO maybe optimize
        for (auto &reg : CSR::REGISTERS) {
            if (field_token.startsWith(reg.name, Qt::CaseInsensitive)) {
                chars_taken = strlen(reg.name);
                return reg.address.data;
            }
        }
        chars_taken = 0;
        return 0;
    } else {
        char *r;
        uint64_t val;
        const char *str = field_token.toLocal8Bit().constData();
        val = strtoul(str, &r, 0);
        chars_taken = r - str;
        return val;
    }
}

bool Instruction::update(int64_t val, RelocExpression *relocexp) {
    // Clear all bit of the updated argument.
    dt &= ~relocexp->arg->encode(~0);
    val += relocexp->offset;

    if (relocexp->pseudo_mod != Modifier::NONE) {
        val = (int64_t)modify_pseudoinst_imm(relocexp->pseudo_mod, val);
    } else {
        if ((val & ((1 << relocexp->arg->shift) - 1))) { return false; }
        if (relocexp->min < 0) {
            if (((int64_t)val < relocexp->min) || ((int64_t)val > relocexp->max)) {
                if (((int64_t)val - 0x100000000 < relocexp->min)
                    || ((int64_t)val - 0x100000000 > relocexp->max)) {
                    return false;
                }
            }
        } else {
            if (((uint64_t)val < (uint64_t)relocexp->min)
                || ((uint64_t)val > (uint64_t)relocexp->max)) {
                return false;
            }
        }
    }

    dt |= relocexp->arg->encode(val);
    return true;
}

constexpr uint64_t Instruction::modify_pseudoinst_imm(Instruction::Modifier mod, uint64_t value) {
    // Example: la rd, symbol -> auipc rd, symbol[31:12] + symbol[11], addi rd, rd, symbol[11:0]

    switch (mod) {
    case Modifier::NONE: return value;
    case Modifier::COMPOSED_IMM_UPPER: return get_bits(value, 31, 12) + get_bit(value, 11);
    case Modifier::COMPOSED_IMM_LOWER: return get_bits(value, 11, 0);
    default: UNREACHABLE
    }
}

// highlighter
void Instruction::append_recognized_instructions(QStringList &list) {
    if (str_to_instruction_code_map.isEmpty()) { instruction_from_string_build_base(); }

    for (auto iter = str_to_instruction_code_map.keyBegin();
         iter != str_to_instruction_code_map.keyEnd(); iter++) {
        list.append(*iter);
    }
    for (const auto &str : RECOGNIZED_PSEUDOINSTRUCTIONS) {
        list.append(str);
    }
}

void Instruction::set_symbolic_registers(bool enable) {
    symbolic_registers_enabled = enable;
}

inline int32_t Instruction::extend(uint32_t value, uint32_t used_bits) const {
    return value | ~((value & (1 << (used_bits - 1))) - 1);
}

void Instruction::append_recognized_registers(QStringList &list) {
    for (auto name : Rv_regnames) {
        list.append(name);
    }
}
uint8_t Instruction::size() const {
    return 4;
}
size_t Instruction::code_from_string(
    uint32_t *code,
    size_t buffsize,
    QString str,
    Address inst_addr,
    RelocExpressionList *reloc,
    const QString &filename,
    unsigned line,
    bool pseudoinst_enabled) {
    auto inst = TokenizedInstruction::from_line(std::move(str), inst_addr, filename, line);
    return Instruction::code_from_tokens(code, buffsize, inst, reloc, pseudoinst_enabled);
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
