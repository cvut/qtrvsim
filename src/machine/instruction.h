#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "execute/alu.h"
#include "machinedefs.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <utility>

namespace machine {

#define REGISTER_CODES 32

const char *const Rv_regnames[REGISTER_CODES] = {
    "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
    "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
    "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
};

enum InstructionFlags {
    IMF_SUPPORTED = 1L << 0, /**< Instruction is supported */
    IMF_MEMWRITE = 1L << 1,  /**< Write to the memory when memory stage is reached */
    IMF_MEMREAD = 1L << 2,   /**< Read from the memory when memory stage is reached */
    IMF_ALUSRC = 1L << 3,    /**< The second ALU source is immediate operand */
    IMF_REGWRITE = 1L << 5,  /**< Instruction result (ALU or memory) is written to register file */
    IMF_MEM = 1L << 11,      /**< Instruction is memory access instruction */
    IMF_ALU_REQ_RS = 1L << 12, /**< Execution phase/ALU requires RS value */
    IMF_ALU_REQ_RT = 1L << 13, /**< Execution phase/ALU/mem requires RT value */
    IMF_BRANCH = 1L << 17, /**< Operation is conditional or unconditional branch or branch and link
                              when PC_TO_R31 is set */
    IMF_JUMP = 1L << 18,   /**< Jump operation - JAL, JALR */
    IMF_BJ_NOT = 1L << 19, /**< Negate condition for branch instruction */
    IMF_EXCEPTION = 1L << 22, /**< Instruction causes synchronous exception */
    IMF_ALU_MOD = 1L << 24,   /**< ADD and right-shift modifier */
    IMF_PC_TO_ALU = 1L << 25, /**< PC is loaded instead of RS to ALU */
    IMF_ECALL = 1L << 26,     // seems easiest to encode ecall and ebreak as flags, but they might
    IMF_EBREAK = 1L << 27,    // be moved elsewhere in case we run out of InstructionFlag space.
    // TODO do we want to add those signals to the visualization?
};

struct BitArg {
    struct Field {
        const uint8_t count;
        const uint8_t offset;
        template<typename T>
        T decode(T val) const {
            return (val >> offset) & ((1L << count) - 1);
        }
        template<typename T>
        T encode(T val) const {
            return ((val & ((1L << count) - 1)) << offset);
        }
    };
    const std::vector<Field> fields;
    const size_t shift;

    BitArg(const std::vector<Field> fields, size_t shift = 0) : fields(fields), shift(shift) {}
    std::vector<Field>::const_iterator begin() const { return fields.begin(); }
    std::vector<Field>::const_iterator end() const { return fields.end(); }
    uint32_t decode(uint32_t ins) const {
        uint32_t ret = 0;
        size_t offset = 0;
        for (Field field : *this) {
            ret |= field.decode(ins) << offset;
            offset += field.count;
        }
        return ret << shift;
    }
    uint32_t encode(uint32_t imm) const {
        uint32_t ret = 0;
        imm >>= shift;
        for (Field field : *this) {
            ret |= field.encode(imm);
            imm >>= field.count;
        }
        return ret;
    }
};

struct RelocExpression;
typedef QVector<RelocExpression *> RelocExpressionList;

class Instruction {
public:
    Instruction();
    explicit Instruction(uint32_t inst);
    // Instruction(
    //     uint8_t opcode,
    //     uint8_t rs,
    //     uint8_t rt,
    //     uint8_t rd,
    //     uint8_t shamt,
    //     uint8_t funct); // Type R
    // Instruction(
    //     uint8_t opcode,
    //     uint8_t rs,
    //     uint8_t rt,
    //     uint16_t immediate);                      // Type I
    // Instruction(uint8_t opcode, Address address); // Type J
    Instruction(const Instruction &);

    static const Instruction NOP;

    enum Type { R, I, S, B, U, J, UNKNOWN };

    /** Modified encoding to enable pseudoinstructions. */
    enum class Modifier {
        /** Normal processing. All fields are checked for value max and min. */
        NONE,
        /**
         * Encodes upper part of immediate from a pseudoinstruction.
         *
         * `imm = symbol[31:12] + symbol[11]`
         * NOTE: `symbol[11]` compensates for sign extension from addition of the lower part.
         */
        COMPOSED_IMM_UPPER,
        /**
         * Encodes lower part of immediate from a pseudoinstruction.
         *
         * `imm = symbol[11:0]`
         * Upper bits of immediate may are discarded.
         */
        COMPOSED_IMM_LOWER,
    };

    struct ParseError;

    /** Returns size of instruction in bytes */
    uint8_t size() const;
    uint8_t opcode() const;
    uint8_t rs() const;
    uint8_t rt() const;
    uint8_t rd() const;
    uint8_t shamt() const;
    uint16_t funct() const;
    uint8_t cop0sel() const;
    int32_t immediate() const;
    Address address() const;
    uint32_t data() const;
    bool imm_sign() const;
    enum Type type() const;
    enum InstructionFlags flags() const;
    enum AluOp alu_op() const;
    enum AccessControl mem_ctl() const;

    void flags_alu_op_mem_ctl(
        enum InstructionFlags &flags,
        enum AluOp &alu_op,
        enum AccessControl &mem_ctl) const;

    bool operator==(const Instruction &c) const;
    bool operator!=(const Instruction &c) const;
    Instruction &operator=(const Instruction &c);

    QString to_str(Address inst_addr = Address::null()) const;

    static size_t code_from_string(
        uint32_t *code,
        size_t buffsize,
        const QString &inst_base,
        QStringList &inst_fields,
        Address inst_addr,
        RelocExpressionList *reloc,
        const QString &filename,
        int line,
        bool pseudoinst_enabled = true);

    /**
     * Parses instruction from tokens.
     *
     * @throws Instruction::ParseError if unable to parse
     */
    static size_t code_from_string(
        uint32_t *code,
        size_t buffsize,
        QString str,
        Address inst_addr = Address::null(),
        RelocExpressionList *reloc = nullptr,
        const QString &filename = "",
        int line = 0,
        bool pseudo_opt = false,
        bool silent = false);

    bool update(int64_t val, RelocExpression *relocexp);

    static void append_recognized_instructions(QStringList &list);
    static void set_symbolic_registers(bool enable);
    static void append_recognized_registers(QStringList &list);
    static constexpr uint64_t modify_pseudoinst_imm(Modifier mod, uint64_t value);

private:
    uint32_t dt;
    static bool symbolic_registers_fl;

    inline int32_t extend(uint32_t value, uint32_t used_bits) const;
    static uint32_t parse_field(
        Address inst_addr,
        RelocExpressionList *reloc,
        const QString &filename,
        int line,
        Modifier pseudo_mod,
        const QString &arg,
        QString &field_token,
        uint64_t value);
    static Instruction base_from_string(
        const QString &inst_base,
        const QStringList &inst_fields,
        Address inst_addr,
        RelocExpressionList *reloc,
        const QString &filename,
        int line,
        Modifier pseudo_mod = Modifier::NONE,
        uint64_t initial_immediate_value = 0);
};

struct Instruction::ParseError : public std::exception {
    QString message;

    explicit ParseError(QString message);
};

struct RelocExpression {
    inline RelocExpression(
        Address location,
        QString expression,
        int64_t offset,
        int64_t min,
        int64_t max,
        const BitArg *arg,
        QString filename,
        int line,
        Instruction::Modifier pseudo_mod = Instruction::Modifier::NONE) {
        this->location = location;
        this->expression = std::move(expression);
        this->offset = offset;
        this->min = min;
        this->max = max;
        this->arg = arg;
        this->filename = std::move(filename);
        this->line = line;
        this->pseudo_mod = pseudo_mod;
    }
    Address location;
    QString expression;
    int64_t offset;
    int64_t min;
    int64_t max;
    const BitArg *arg;
    QString filename;
    int line;
    Instruction::Modifier pseudo_mod;
};

} // namespace machine

Q_DECLARE_METATYPE(machine::Instruction)

#endif // INSTRUCTION_H
