#include "instruction.h"

#include "common/logging.h"
#include "memory/backend/memory.h"
#include "simulator_exception.h"
#include "utils.h"

#include <QChar>
#include <QMultiMap>
#include <QStringList>
#include <cctype>
#include <cstring>
#include <initializer_list>
#include <string>
#include <utility>

LOG_CATEGORY("instruction");

using namespace machine;

bool Instruction::symbolic_registers_fl = false;

#define IMF_SUB_ENCODE(bits, shift) (((bits) << 8) | (shift))
#define IMF_SUB_GET_BITS(subcode) (((subcode) >> 8) & 0xff)
#define IMF_SUB_GET_SHIFT(subcode) ((subcode)&0xff)

struct ArgumentDesc {
    // TODO: maybe signed?
    char name;
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
};

static const ArgumentDesc argdeslist[] = {
    ArgumentDesc('d', 'g', 0, 0x1f, { { { 5, 7 } }, 0 }),
    ArgumentDesc('s', 'g', 0, 0x1f, { { { 5, 15 } }, 0 }),
    ArgumentDesc('t', 'g', 0, 0x1f, { { { 5, 20 } }, 0 }),
    ArgumentDesc('j', 'n', -0x800, 0x7ff, { { { 12, 20 } }, 0 }),
    ArgumentDesc('>', 'n', 0, 0x1f, { { { 5, 20 } }, 0 }),
    ArgumentDesc('a', 'a', -0x80000, 0x7ffff, { { { 10, 21 }, { 1, 20 }, { 8, 12 }, { 1, 31 } }, 1 }),
    ArgumentDesc('u', 'n', 0, 0xfffff000, { { { 20, 12 } }, 12 }),
    ArgumentDesc('p', 'p', -0x800, 0x7ff, { { { 4, 8 }, { 6, 25 }, { 1, 7 }, { 1, 31 } }, 1 }),
    ArgumentDesc('o', 'o', -0x800, 0x7ff, { { { 12, 20 } }, 0 }),
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
    foreach (const QString &arg, im.args) {
        res += next_delim;
        next_delim = ", ";
        foreach (QChar ao, arg) {
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
    int options = 0) {
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
        options));
    if (chars_taken != nullptr) { *chars_taken = i; }
}

ssize_t Instruction::code_from_string(
    uint32_t *code,
    size_t buffsize,
    const QString &inst_base,
    QStringList &inst_fields,
    QString &error,
    Address inst_addr,
    RelocExpressionList *reloc,
    const QString &filename,
    int line,
    bool pseudo_opt,
    bool silent) {
    Q_UNUSED(pseudo_opt);
    const char *err = "unknown instruction";
    if (str_to_instruction_code_map.isEmpty()) { instruction_from_string_build_base(); }

    int field = 0;
    uint32_t inst_code = 0;
    auto i = str_to_instruction_code_map.lowerBound(inst_base);
    for (;; i++) {
        if (i == str_to_instruction_code_map.end()) { break; }
        if (i.key() != inst_base) { break; }
        inst_code = i.value();
        const InstructionMap &im = InstructionMapFind(inst_code);

        field = 0;
        foreach (const QString &arg, im.args) {
            if (field >= inst_fields.count()) {
                err = "number of arguments does not match";
                field = -1;
                break;
            }
            QString fl = inst_fields.at(field++);
            foreach (QChar ao, arg) {
                bool need_reloc = false;
                const char *p;
                char *r;
                uint a = ao.toLatin1();
                if (!a) { continue; }
                fl = fl.trimmed();
                const ArgumentDesc *adesc = argdesbycode[a];
                if (adesc == nullptr) {
                    if (!fl.count()) {
                        err = "empty argument encountered";
                        field = -1;
                        break;
                    }
                    if (fl.at(0) != ao) {
                        err = "argument does not match instruction template";
                        field = -1;
                        break;
                    }
                    fl = fl.mid(1);
                    continue;
                }

                uint64_t val = 0;
                uint chars_taken = 0;

                switch (adesc->kind) {
                case 'g': val += parse_reg_from_string(fl, &chars_taken); break;
                case 'p':
                case 'a': val -= inst_addr.get_raw(); FALLTROUGH
                case 'o':
                case 'n':
                    if (fl.at(0).isDigit() || fl.at(0) == '-' || (reloc == nullptr)) {
                        uint64_t num_val;
                        int i;
                        // Qt functions are limited, toLongLong would be usable
                        // but does not return information how many characters
                        // are processed. Used solution has horrible overhead
                        // but is usable for now
                        char cstr[fl.count() + 1];
                        for (i = 0; i < fl.count(); i++) {
                            cstr[i] = fl.at(i).toLatin1();
                        }
                        cstr[i] = 0;
                        p = cstr;
                        if (adesc->min < 0) {
                            num_val = std::strtoll(p, &r, 0);
                        } else {
                            num_val = std::strtoull(p, &r, 0);
                        }
                        while (*r && std::isspace(*r)) {
                            r++;
                        }
                        if (*r && std::strchr("+-/*|&^~", *r)) {
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
                            reloc, fl, inst_addr, val, adesc, &chars_taken, filename, line);
                        val = 0;
                    }
                    break;
                }
                if (chars_taken <= 0) {
                    err = "argument parse error";
                    field = -1;
                    break;
                }
                if (!silent) {
                    if (adesc->min < 0) {
                        if (((int64_t)val < adesc->min) || ((int64_t)val > adesc->max)) {
                            err = "argument range exceed";
                            field = -1;
                            break;
                        }
                    } else {
                        if ((val < (uint64_t)adesc->min) || (val > (uint64_t)adesc->max)) {
                            err = "argument range exceed";
                            field = -1;
                            break;
                        }
                    }
                }
                // val = (val & ((1 << bits) - 1)) << shift;
                inst_code |= adesc->arg.encode(val);
                fl = fl.mid(chars_taken);
            }
            if (field == -1) { break; }
            if (fl.trimmed() != "") {
                err = "excessive characters in argument";
                field = -1;
                break;
            }
        }
        if (field != inst_fields.count()) { continue; }

        if (buffsize >= 4) { *code = inst_code; }
        return 4;
    }

    ssize_t ret = -1;
    inst_code = 0;
    if (inst_base == "nop" && inst_fields.size() == 0) {
        inst_code = 0x13; // hardcoded for now, maybe it would be better to extract it
                          // from somewhere, but instruction maps are constructed at runtime as of
                          // now.
        ret = 4;
    }
    if (buffsize >= 4) { *code = inst_code; }
    if (ret < 0) { error = err; }
    return ret;
}

ssize_t Instruction::code_from_string(
    uint32_t *code,
    size_t buffsize,
    QString str,
    QString &error,
    Address inst_addr,
    RelocExpressionList *reloc,
    const QString &filename,
    int line,
    bool pseudo_opt,
    bool silent) {
    int k = 0, l;
    while (k < str.count()) {
        if (!str.at(k).isSpace()) { break; }
        k++;
    }
    l = k;
    while (l < str.count()) {
        if (!str.at(l).isLetterOrNumber()) { break; }
        l++;
    }
    QString inst_base = str.mid(k, l - k).toLower();
    str = str.mid(l + 1).trimmed();
    QStringList inst_fields;
    if (str.count()) { inst_fields = str.split(","); }

    if (!inst_base.count()) {
        error = "empty instruction field";
        return -1;
    }

    return code_from_string(
        code, buffsize, inst_base, inst_fields, error, inst_addr, reloc, filename, line, pseudo_opt,
        silent);
}

bool Instruction::update(int64_t val, RelocExpression *relocexp) {
    dt &= ~relocexp->arg->encode(~0);
    val += relocexp->offset;
    if (!relocexp->silent && (val & ((1 << relocexp->arg->shift) - 1))) { return false; }
    if (!(relocexp->silent)) {
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

void Instruction::append_recognized_instructions(QStringList &list) {
    if (str_to_instruction_code_map.isEmpty()) { instruction_from_string_build_base(); }

    foreach (const QString &str, str_to_instruction_code_map.keys())
        list.append(str);
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

Instruction::ParseError::ParseError(QString message) : message(std::move(message)) {}
