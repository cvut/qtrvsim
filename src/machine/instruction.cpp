#include "instruction.h"

#include "common/logging.h"
#include "common/math/bit_ops.h"
#include "simulator_exception.h"
#include "utils.h"

#include <QChar>
#include <QMultiMap>
#include <QStringList>
#include <cctype>
#include <cstring>
#include <initializer_list>
#include <set>
#include <utility>

LOG_CATEGORY("instruction");

using namespace machine;

bool Instruction::symbolic_registers_fl = false;

#define IMF_SUB_ENCODE(bits, shift) (((bits) << 8) | (shift))
#define IMF_SUB_GET_BITS(subcode) (((subcode) >> 8) & 0xff)
#define IMF_SUB_GET_SHIFT(subcode) ((subcode)&0xff)

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
        , arg(std::move(arg)) {}

    /** Check whether given value fits into this instruction field. */
    constexpr bool is_value_in_field_range(RegisterValue val) const {
        if (min < 0) {
            return val.as_i64() <= max && val.as_i64() >= min;
        } else {
            return val.as_u64() <= static_cast<uint64_t>(max)
                   && val.as_u64() >= static_cast<uint64_t>(min);
        }
    }

    constexpr bool is_imm() const { return kind != 'g'; }
};

static const ArgumentDesc argdeslist[] = {
    // Destination register (rd)
    ArgumentDesc('d', 'g', 0, 0x1f, { { { 5, 7 } }, 0 }),
    // Source register 1 (rs1/rs)
    ArgumentDesc('s', 'g', 0, 0x1f, { { { 5, 15 } }, 0 }),
    // Source register 2 (rs2/rt)
    ArgumentDesc('t', 'g', 0, 0x1f, { { { 5, 20 } }, 0 }),
    // I-type immediate for arithmetic instructions (12bits)
    ArgumentDesc('j', 'n', -0x800, 0x7ff, { { { 12, 20 } }, 0 }),
    // Shaft for bit shift instructions (5bits)
    ArgumentDesc('>', 'n', 0, 0x1f, { { { 5, 20 } }, 0 }),
    // Address offset immediate (20bits), encoded in multiples of 2 bytes
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

static const ArgumentDesc *argdesbycode[(int)('z' + 1)];

static bool fill_argdesbycode() {
    uint i;
    for (i = 0; i < sizeof(argdeslist) / sizeof(*argdeslist); i++) {
        argdesbycode[(uint)argdeslist[i].name] = &argdeslist[i];
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

// #define NOALU .alu = ALU_OP_SLL
#define NOALU .alu = AluOp::ADD
#define NOMEM .mem_ctl = AC_NONE

#define IM_UNKNOWN                                                                                 \
    {                                                                                              \
        "unknown", Instruction::UNKNOWN, NOALU, NOMEM, nullptr, {}, 0, 0, { 0 }                    \
    }
// TODO NOTE: if unknown is defined as all 0, instruction map can be
// significanly simplified using zero initialization.

struct InstructionMap {
    const char *name;
    enum Instruction::Type type;
    enum AluOp alu;
    enum AccessControl mem_ctl;
    const struct InstructionMap *subclass; // when subclass is used then flags
                                           // has special meaning
    const QStringList args;
    uint32_t code;
    uint32_t mask;
    union {
        unsigned int flags;
        BitArg::Field subfield;
    };
};

#define IT_R Instruction::R
#define IT_I Instruction::I
#define IT_S Instruction::S
#define IT_B Instruction::B
#define IT_U Instruction::U
#define IT_J Instruction::J
#define IT_UNKNOWN Instruction::UNKNOWN

// clang-format off

static const struct InstructionMap LOAD_map[] = {
    {"lb",  IT_I, AluOp::ADD, AC_I8,  nullptr, {"d", "o(s)"}, 0x00000003, 0x0000707f, { .flags = FLAGS_ALU_I_LOAD }}, // LB
    {"lh",  IT_I, AluOp::ADD, AC_I16, nullptr, {"d", "o(s)"}, 0x00001003, 0x0000707f, { .flags = FLAGS_ALU_I_LOAD }}, // LH
    {"lw",  IT_I, AluOp::ADD, AC_I32, nullptr, {"d", "o(s)"}, 0x00002003, 0x0000707f, { .flags = FLAGS_ALU_I_LOAD }}, // LW
    {"ld",  IT_I, AluOp::ADD, AC_I64, nullptr, {"d", "o(s)"}, 0x00003003, 0x0000707f, { .flags = FLAGS_ALU_I_LOAD }}, // LD
    {"lbu", IT_I, AluOp::ADD, AC_U8,  nullptr, {"d", "o(s)"}, 0x00004003, 0x0000707f, { .flags = FLAGS_ALU_I_LOAD }}, // LBU
    {"lhu", IT_I, AluOp::ADD, AC_U16, nullptr, {"d", "o(s)"}, 0x00005003, 0x0000707f, { .flags = FLAGS_ALU_I_LOAD }}, // LHU
    {"lwu", IT_I, AluOp::ADD, AC_U32, nullptr, {"d", "o(s)"}, 0x00006003, 0x0000707f, { .flags = FLAGS_ALU_I_LOAD }}, // LWU
    IM_UNKNOWN,
};

static const struct InstructionMap SRI_map[] = {
    {"srli", IT_I, AluOp::SR, NOMEM, nullptr, {"d", "s", ">"}, 0x00005013, 0xfe00707f, { .flags = FLAGS_ALU_I }}, // SRLI
    {"srai", IT_I, AluOp::SR, NOMEM, nullptr, {"d", "s", ">"}, 0x40005013, 0xfe00707f, { .flags = FLAGS_ALU_I | IMF_ALU_MOD }}, // SRAI
};

static const struct InstructionMap OP_IMM_map[] = {
    {"addi",  IT_I, AluOp::ADD,  NOMEM, nullptr, {"d", "s", "j"}, 0x00000013, 0x0000707f, { .flags = FLAGS_ALU_I }}, // ADDI
    {"slli",  IT_I, AluOp::SLL,  NOMEM, nullptr, {"d", "s", ">"}, 0x00001013, 0xfe00707f, { .flags = FLAGS_ALU_I }}, // SLLI
    {"slti",  IT_I, AluOp::SLT,  NOMEM, nullptr, {"d", "s", "j"}, 0x00002013, 0x0000707f, { .flags = FLAGS_ALU_I }}, // SLTI
    {"sltiu", IT_I, AluOp::SLTU, NOMEM, nullptr, {"d", "s", "j"}, 0x00003013, 0x0000707f, { .flags = FLAGS_ALU_I }}, // SLTIU
    {"xori",  IT_I, AluOp::XOR,  NOMEM, nullptr, {"d", "s", "j"}, 0x00004013, 0x0000707f, { .flags = FLAGS_ALU_I }}, // XORI
    {"sri",   IT_I, NOALU,       NOMEM, SRI_map,              {}, 0x00005013, 0xbe00707f, { .subfield = {1, 30} }}, // SRLI, SRAI
    {"ori",   IT_I, AluOp::OR,   NOMEM, nullptr, {"d", "s", "j"}, 0x00006013, 0x0000707f, { .flags = FLAGS_ALU_I }}, // ORI
    {"andi",  IT_I, AluOp::AND,  NOMEM, nullptr, {"d", "s", "j"}, 0x00007013, 0x0000707f, { .flags = FLAGS_ALU_I }}, // ANDI
};

static const struct InstructionMap STORE_map[] = {
    {"sb", IT_S, AluOp::ADD, AC_U8,  nullptr, {"t", "q(s)"}, 0x00000023, 0x0000707f, { .flags = FLAGS_ALU_I_STORE }}, // SB
    {"sh", IT_S, AluOp::ADD, AC_U16, nullptr, {"t", "q(s)"}, 0x00001023, 0x0000707f, { .flags = FLAGS_ALU_I_STORE }}, // SH
    {"sw", IT_S, AluOp::ADD, AC_U32, nullptr, {"t", "q(s)"}, 0x00002023, 0x0000707f, { .flags = FLAGS_ALU_I_STORE }}, // SW
    {"sd", IT_S, AluOp::ADD, AC_U64, nullptr, {"t", "q(s)"}, 0x00003023, 0x0000707f, { .flags = FLAGS_ALU_I_STORE }}, // SD
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
};

static const struct InstructionMap ADD_map[] = {
    {"add", IT_R, AluOp::ADD, NOMEM, nullptr, {"d", "s", "t"}, 0x00000033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }},
    {"sub", IT_R, AluOp::ADD, NOMEM, nullptr, {"d", "s", "t"}, 0x40000033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD | IMF_ALU_MOD }},
};

static const struct InstructionMap SR_map[] = {
    {"srl", IT_R, AluOp::SR, NOMEM, nullptr, {"d", "s", "t"}, 0x00005033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }}, // SRL
    {"sra", IT_R, AluOp::SR, NOMEM, nullptr, {"d", "s", "t"}, 0x40005033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD | IMF_ALU_MOD }}, // SRA
};

static const struct InstructionMap OP_map[] = {
    {"add/sub", IT_R, NOALU,    NOMEM, ADD_map,              {}, 0x00000033, 0xbe00707f, { .subfield = {1, 30} }},
    {"sll",  IT_R, AluOp::SLL,  NOMEM, nullptr, {"d", "s", "t"}, 0x00001033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }}, // SLL
    {"slt",  IT_R, AluOp::SLT,  NOMEM, nullptr, {"d", "s", "t"}, 0x00002033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }}, // SLT
    {"sltu", IT_R, AluOp::SLTU, NOMEM, nullptr, {"d", "s", "t"}, 0x00003033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }}, // SLTU
    {"xor",  IT_R, AluOp::XOR,  NOMEM, nullptr, {"d", "s", "t"}, 0x00004033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }}, // XOR
    {"sr",   IT_R, NOALU,       NOMEM,  SR_map,              {}, 0x00005033, 0xbe00707f, { .subfield = {1, 30} }}, // SRL, SRA
    {"or",   IT_R, AluOp::OR,   NOMEM, nullptr, {"d", "s", "t"}, 0x00006033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }}, // OR
    {"and",  IT_R, AluOp::AND,  NOMEM, nullptr, {"d", "s", "t"}, 0x00007033, 0xfe00707f, { .flags = FLAGS_ALU_T_R_STD }}, // AND
};

static const struct InstructionMap BRANCH_map[] = {
    {"beq",  IT_B, AluOp::ADD, NOMEM,  nullptr, {"s", "t", "p"}, 0x00000063, 0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT | IMF_ALU_MOD }}, // BEQ
    {"bne",  IT_B, AluOp::ADD, NOMEM,  nullptr, {"s", "t", "p"}, 0x00001063, 0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT | IMF_ALU_MOD | IMF_BJ_NOT }}, // BNE
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"blt",  IT_B, AluOp::SLT, NOMEM,  nullptr, {"s", "t", "p"}, 0x00004063, 0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT }}, // BLT
    {"bge",  IT_B, AluOp::SLT, NOMEM,  nullptr, {"s", "t", "p"}, 0x00005063, 0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT | IMF_BJ_NOT }}, // BGE
    {"bltu", IT_B, AluOp::SLTU, NOMEM, nullptr, {"s", "t", "p"}, 0x00006063, 0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT }}, // BLTU
    {"bgeu", IT_B, AluOp::SLTU, NOMEM, nullptr, {"s", "t", "p"}, 0x00007063, 0x0000707f, { .flags = IMF_SUPPORTED | IMF_BRANCH | IMF_ALU_REQ_RS | IMF_ALU_REQ_RT | IMF_BJ_NOT }}, // BGEU
};

static const struct InstructionMap SYSTEM_map[] = {
    {"ecall", IT_I, NOALU, NOMEM, nullptr, {}, 0x00000073, 0xffffffff, { .flags = IMF_SUPPORTED | IMF_EXCEPTION | IMF_ECALL }},
    {"ebreak", IT_I, NOALU, NOMEM, nullptr, {}, 0x00100073, 0xffffffff, { .flags = IMF_SUPPORTED | IMF_EXCEPTION | IMF_EBREAK }},
};

static const struct InstructionMap I_inst_map[] = {
    {"load", IT_I, NOALU, NOMEM, LOAD_map, {}, 0x03, 0x7f, { .subfield = {3, 12} }}, // LOAD
    IM_UNKNOWN, // LOAD-FP
    IM_UNKNOWN, // custom-0
    IM_UNKNOWN, // MISC-MEM
    {"op-imm", IT_I, NOALU, NOMEM, OP_IMM_map, {}, 0x13, 0x7f, { .subfield = {3, 12} }}, // OP-IMM
    {"auipc", IT_U, AluOp::ADD, NOMEM, nullptr, {"d", "u"}, 0x17, 0x7f, { .flags = IMF_SUPPORTED | IMF_ALUSRC | IMF_REGWRITE | IMF_PC_TO_ALU }}, // AUIPC
    IM_UNKNOWN, // OP-IMM-32
    IM_UNKNOWN, // 48b
    {"store", IT_I, NOALU, NOMEM, STORE_map, {}, 0x23, 0x7f, { .subfield = {3, 12} }}, // STORE
    IM_UNKNOWN, // STORE-FP
    IM_UNKNOWN, // custom-1
    IM_UNKNOWN, // AMO
    {"op", IT_R, NOALU, NOMEM, OP_map, {}, 0x33, 0x7f, { .subfield = {3, 12} }}, // OP
    {"lui", IT_U, AluOp::ADD, NOMEM, nullptr, {"d", "u"}, 0x37, 0x7f, { .flags = IMF_SUPPORTED | IMF_ALUSRC | IMF_REGWRITE }}, // LUI
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
    IM_UNKNOWN, // JALR
    IM_UNKNOWN, // reserved
    {"jal", IT_J, AluOp::ADD, NOMEM, nullptr, {"d", "a"}, 0x6f, 0x7f, { .flags = IMF_SUPPORTED | IMF_REGWRITE | IMF_JUMP | IMF_PC_TO_ALU | IMF_ALUSRC }}, // JAL
    {"system", IT_I, NOALU, NOMEM, SYSTEM_map, {}, 0x73, 0x7f, { .subfield = {1, 20} }}, // SYSTEM
    IM_UNKNOWN, // reserved
    IM_UNKNOWN, // custom-3/rv128
    IM_UNKNOWN, // >= 80b
};

static const struct InstructionMap C_inst_map[] = {
    IM_UNKNOWN,
    IM_UNKNOWN,
    IM_UNKNOWN,
    {"i", IT_UNKNOWN, NOALU, NOMEM, I_inst_map, {}, 0x3, 0x3, { .subfield = {5, 2} }},
};

// clang-format on

const BitArg::Field instruction_map_opcode_field = { 2, 0 };

static inline const struct InstructionMap &InstructionMapFind(uint32_t code) {
    const struct InstructionMap *im = &C_inst_map[instruction_map_opcode_field.decode(code)];
    while (im->subclass != nullptr) {
        im = &im->subclass[im->subfield.decode(code)];
    }
    return *im;
}

#undef IM_UNKNOWN
#undef IMF_SUB_ENCODE
#undef IMF_SUB_GET_BITS
#undef IMF_SUB_GET_SHIFT

const std::array<const QString, 2> RECOGNIZED_PSEUDOINSTRUCTIONS { "nop", "la" };

const Instruction Instruction::NOP = Instruction(0x00000013);

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

uint8_t Instruction::cop0sel() const {
    return (uint8_t)MASK(3, 0);
}

int32_t Instruction::immediate() const {
    int32_t ret = 0;
    switch (this->type()) {
    case R: break;
    case I: ret = extend(MASK(12, 20), 12); break;
    case S: ret = extend(MASK(7, 25) << 5 | MASK(5, 7), 12); break;
    case B:
        ret = extend(MASK(4, 8) << 1 | MASK(6, 25) << 5 | MASK(1, 7) << 11 | MASK(1, 31) << 12, 12);
        break;
    case U: ret = this->dt & ~((1 << 12) - 1); break;
    case J:
        ret = extend(
            MASK(10, 21) << 1 | MASK(1, 20) << 11 | MASK(8, 12) << 12 | MASK(1, 31) << 20, 21);
        break;
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
enum AluOp Instruction::alu_op() const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    return im.alu;
}

enum AccessControl Instruction::mem_ctl() const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    return im.mem_ctl;
}

void Instruction::flags_alu_op_mem_ctl(
    enum InstructionFlags &flags,
    enum AluOp &alu_op,
    enum AccessControl &mem_ctl) const {
    const struct InstructionMap &im = InstructionMapFind(dt);
    flags = (enum InstructionFlags)im.flags;
    alu_op = im.alu;
    mem_ctl = im.mem_ctl;
#if 1
    if ((dt ^ im.code) & (im.mask)) { flags = (enum InstructionFlags)(flags & ~IMF_SUPPORTED); }
#endif
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
    if (im.type == UNKNOWN) { return QString("unknown"); }
    if (this->dt == NOP.dt) { return QString("nop"); }

    res += im.name;
    for (const QString &arg : im.args) {
        res += next_delim;
        next_delim = ", ";
        for (QChar ao : arg) {
            uint a = ao.toLatin1();
            if (!a) { continue; }
            const ArgumentDesc *adesc = argdesbycode[a];
            if (adesc == nullptr) {
                res += ao;
                continue;
            }
            int32_t field = adesc->arg.decode(this->dt);
            if (adesc->min < 0) {
                field = extend(field, [&]() {
                    int sum = adesc->arg.shift;
                    for (auto chunk : adesc->arg)
                        sum += chunk.count;
                    return sum - 1;
                }());
            }
            switch (adesc->kind) {
            case 'g':
                if (symbolic_registers_fl) {
                    res += QString(Rv_regnames[field]);
                } else {
                    res += "x" + QString::number(field);
                }
                break;
            case 'p':
            case 'a':
                field += inst_addr.get_raw();
                res += "0x" + QString::number(uint32_t(field), 16);
                break;
            case 'o':
            case 'n':
                if (adesc->min < 0) {
                    res += QString::number((int32_t)field, 10);
                } else {
                    res += "0x" + QString::number(uint32_t(field), 16);
                }
                break;
            }
        }
    }
    return res;
}

QMultiMap<QString, uint32_t> str_to_instruction_code_map;

void instruction_from_string_build_base(
    const InstructionMap *im,
    BitArg::Field field,
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
#if 0
            printf("code mitchmatch %s computed 0x%08x found 0x%08x\n", im->name, code, im->code);
#endif
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

static int parse_reg_from_string(QString str, uint *chars_taken = nullptr) {
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
        for (int i = 0; i < REGISTER_CODES; i++) {
            size_t len = std::strlen(Rv_regnames[i]);
            if (std::strncmp(data.data(), Rv_regnames[i], std::min(size_t(str.size()), len)) == 0) {
                *chars_taken = len;
                return i;
            }
        }
        return -1;
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

    reloc->append(new RelocExpression(
        inst_addr, expression, offset, adesc->min, adesc->max, &adesc->arg, filename, line,
        pseudo_mod));
    if (chars_taken != nullptr) { *chars_taken = i; }
}

size_t Instruction::code_from_tokens(
    uint32_t *code,
    size_t buffsize,
    TokenizedInstruction &inst,
    RelocExpressionList *reloc,
    bool pseudoinst_enabled) {
    if (str_to_instruction_code_map.isEmpty()) { instruction_from_string_build_base(); }

    Instruction result = base_from_string(inst, reloc);
    if (result.data() != 0) {
        if (result.size() > buffsize) {
            // NOTE: this is bug, not user error.
            throw ParseError("insufficient buffer size to write parsed instruction");
        }
        *code = result.data();
        return result.size();
    }

    if (pseudoinst_enabled) {
        constexpr Modifier UPPER = Modifier::COMPOSED_IMM_UPPER;
        constexpr Modifier LOWER = Modifier::COMPOSED_IMM_LOWER;

        if (inst.base == QLatin1String("la")) {
            // la rd, symbol
            // auipc rd, symbol[31:12] + symbol[11]
            *code = base_from_string(
                        { "auipc", inst.fields, inst.address, inst.filename, inst.line }, reloc,
                        UPPER, -inst.address.get_raw())
                        .data();
            code += 1;
            // addi rd, rd, symbol[11:0]
            inst.fields.insert(1, inst.fields.at(0)); // duplicate rd
            *code = base_from_string(
                        { "addi", inst.fields, inst.address + 4, inst.filename, inst.line }, reloc,
                        LOWER, -inst.address.get_raw())
                        .data();
            return 8;
        }
        if (inst.base == QLatin1String("nop")) {
            if (!inst.fields.empty()) { throw ParseError("`nop` does not allow any arguments"); }
            result = Instruction::NOP;
            *code = result.data();
            return result.size();
        }
    }
    throw ParseError("unknown instruction");
}
Instruction Instruction::base_from_string(
    const TokenizedInstruction &inst,
    RelocExpressionList *reloc,
    Modifier pseudo_mod,
    uint64_t initial_immediate_value) {
    auto iter_range = str_to_instruction_code_map.equal_range(inst.base);
    if (iter_range.first == iter_range.second) {
        DEBUG("Base instruction of the name %s not found.", qPrintable(inst.base));
        return Instruction(0);
    }
    // Process all codes associated with given instruction name and try matching the supplied
    // instruction field tokens to fields. First matching instruction is used.
    // TODO: Can the map contain more codes for single name?
    // TODO: Why not store `InstructionMap` directly in the multimap?
    for (; iter_range.first != iter_range.second; iter_range.first += 1) {
        uint32_t inst_code = iter_range.first.value();
        const InstructionMap &imap = InstructionMapFind(inst_code);

        if (inst.fields.count() != imap.args.count()) { continue; }

        for (int field_index = 0; field_index < imap.args.count(); field_index += 1) {
            const QString &arg = imap.args[field_index];
            QString field_token = inst.fields[field_index];
            inst_code |= parse_field(
                field_token, arg, inst.address, reloc, inst.filename, inst.line, pseudo_mod,
                initial_immediate_value);
        }
        return Instruction(inst_code);
    }

    ERROR(
        "No instruction map associated with name \"%s\" has matching number of fields (%u).",
        qPrintable(inst.base), inst.fields.size());
    throw ParseError("number of arguments does not match");
}

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
        const char *p;
        char *r;
        uint a = ao.toLatin1();
        if (!a) { continue; }
        field_token = field_token.trimmed();
        const ArgumentDesc *adesc = argdesbycode[a];
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
            if (field_token.at(0).isDigit() || field_token.at(0) == '-' || (reloc == nullptr)) {
                uint64_t num_val;
                int i;
                // Qt functions are limited, toLongLong would be usable
                // but does not return information how many characters
                // are processed. Used solution has horrible overhead
                // but is usable for now
                char cstr[field_token.count() + 1];
                for (i = 0; i < field_token.count(); i++) {
                    cstr[i] = field_token.at(i).toLatin1();
                }
                cstr[i] = 0;
                p = cstr;
                if (adesc->min < 0) {
                    num_val = strtoll(p, &r, 0);
                } else {
                    num_val = strtoull(p, &r, 0);
                }
                while (*r && isspace(*r)) {
                    r++;
                }
                if (*r && strchr("+-/*|&^~", *r)) {
                    need_reloc = true;
                } else {
                    // extend signed bits
                    val += num_val;
                }
                chars_taken = r - p;
            } else {
                need_reloc = true;
            }
            if (need_reloc && (reloc != nullptr)) {
                reloc_append(
                    reloc, field_token, inst_addr, val, adesc, &chars_taken, filename, line,
                    effective_mod);
                val = 0;
            }
            break;
        }
        }
        if (chars_taken <= 0) { throw ParseError("argument parse error"); }

        if (effective_mod != Modifier::NONE) { val = modify_pseudoinst_imm(effective_mod, val); }
        if (!adesc->is_value_in_field_range(val)) { throw ParseError("argument range exceed"); }

        inst_code |= adesc->arg.encode(val);
        field_token = field_token.mid(chars_taken);
    }
    if (field_token.trimmed() != "") { throw ParseError("excessive characters in argument"); }
    return inst_code;
}

bool Instruction::update(int64_t val, RelocExpression *relocexp) {
    // Clear all bit of the updated argument.
    dt &= ~relocexp->arg->encode(~0);
    val += relocexp->offset;

    if (relocexp->pseudo_mod != Modifier::NONE) {
        val = (int64_t)modify_pseudoinst_imm(relocexp->pseudo_mod, val);
    }

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

    dt |= relocexp->arg->encode(val);
    return true;
}

constexpr uint64_t Instruction::modify_pseudoinst_imm(Instruction::Modifier mod, uint64_t value) {
    switch (mod) {
    case Modifier::NONE: return value;
    case Modifier::COMPOSED_IMM_UPPER: return get_bits(value, 31, 12) + get_bit(value, 11);
    case Modifier::COMPOSED_IMM_LOWER: return get_bits(value, 11, 0);
    }
}

// highlighter
void Instruction::append_recognized_instructions(QStringList &list) {
    if (str_to_instruction_code_map.isEmpty()) { instruction_from_string_build_base(); }

    for (const QString &str : str_to_instruction_code_map.keys()) {
        list.append(str);
    }
    for (const auto &str : RECOGNIZED_PSEUDOINSTRUCTIONS) {
        list.append(str);
    }
}

void Instruction::set_symbolic_registers(bool enable) {
    symbolic_registers_fl = enable;
}

inline int32_t Instruction::extend(uint32_t value, uint32_t used_bits) const {
    return value | ~((value & (1 << (used_bits - 1))) - 1);
}

void Instruction::append_recognized_registers(QStringList &list) {
    int i;
    for (i = 0; i < REGISTER_CODES; i++) {
        QString name = Rv_regnames[i];
        if (name != "") { list.append(name); }
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
