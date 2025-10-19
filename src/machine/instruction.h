#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "bitfield.h"
#include "common/containers/cvector.h"
#include "csr/address.h"
#include "execute/alu.h"
#include "machinedefs.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <array>
#include <utility>

namespace machine {

// 4 is max number of parts in currently used instructions.
using BitArg = SplitBitField<4>;

static constexpr std::array<const char *const, 32> Rv_regnames = {
    "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
    "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
    "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
};

enum InstructionFlags : unsigned {
    IMF_SUPPORTED = 1L << 0,  /**< Instruction is supported */
    IMF_MEMWRITE = 1L << 1,   /**< Write to the memory when memory stage is reached */
    IMF_MEMREAD = 1L << 2,    /**< Read from the memory when memory stage is reached */
    IMF_ALUSRC = 1L << 3,     /**< The second ALU source is immediate operand */
    IMF_REGWRITE = 1L << 4,   /**< Instruction result (ALU or memory) is written to register file */
    IMF_MEM = 1L << 5,        /**< Instruction is memory access instruction */
    IMF_ALU_REQ_RS = 1L << 6, /**< Execution phase/ALU requires RS value */
    IMF_ALU_REQ_RT = 1L << 7, /**< Execution phase/ALU/mem requires RT value */
    IMF_BRANCH = 1L << 8,  /**< Operation is conditional or unconditional branch or branch and link
                               when PC_TO_R31 is set */
    IMF_JUMP = 1L << 9,    /**< Jump operation - JAL, JALR */
    IMF_BJ_NOT = 1L << 10, /**< Negate condition for branch instruction */
    IMF_BRANCH_JALR = 1L << 11, /**< Use ALU output as branch/jump target. Used by JALR. */
    IMF_EXCEPTION = 1L << 12,   /**< Instruction causes synchronous exception */
    IMF_ALU_MOD = 1L << 13,     /**< ADD and right-shift modifier */
    IMF_PC_TO_ALU = 1L << 14,   /**< PC is loaded instead of RS to ALU */
    IMF_FORCE_W_OP = 1L << 15,  /**< Force word (32-bit) operation even for XLEN=64  */
    IMF_ECALL = 1L << 16,       // seems easiest to encode ecall and ebreak as flags, but they might
    IMF_EBREAK = 1L << 17,      // be moved elsewhere in case we run out of InstructionFlag space.
    IMF_XRET = 1L << 18,        /**< Return from exception, MRET and SRET  */

    // Extensions:
    // =============================================================================================
    // RV64/32M
    IMF_MUL = 1L << 19, /**< Enables multiplication component of ALU. */
    // Zicsr
    IMF_CSR = 1L << 20,        /**< Implies csr read and write */
    IMF_CSR_TO_ALU = 1L << 21, /**< Instruction modifies the current value */
    IMF_ALU_RS_ID = 1L << 22,
    // RV64/32A - Atomic Memory Operations
    IMF_AMO = 1L << 23,        /**< Instruction is AMO */
    // TODO do we want to add those signals to the visualization?

    IMF_RV64 = 1L << 24, /**< Mark instructions which are available in 64-bit mode only. */
    
    // RV64/32F
    IMF_ALU_REQ_RS_F = 1L << 25, /** < Execution phase/ALU/mem requires RS value in Float Reg*/
    IMF_ALU_REQ_RT_F = 1L << 26, /**< Execution phase/ALU/mem requires RT value in Float Reg*/
    IMF_ALU_REQ_RD_F = 1L << 27, /**< Execution phase/ALU/mem requires RD value in Float Reg*/
};

/**
 * Collection of data necessary to parse instruction from tokens.
 *
 * @TODO Switch to QStringView
 */
struct TokenizedInstruction {
    QString base;
    QStringList fields;
    Address address;
    QString filename;
    unsigned line;

public:
    TokenizedInstruction(
        QString base,
        QStringList fields,
        const Address &address,
        QString filename,
        unsigned int line);

    /** Tokenize assembler line */
    static TokenizedInstruction
    from_line(QString line_str, Address inst_addr, const QString &filename, unsigned line);
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
    static const Instruction UNKNOWN_INST;

    enum Type { R, I, S, B, U, J, ZICSR, AMO, UNKNOWN };

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
    machine::CSR::Address csr_address() const;
    int32_t immediate() const;
    Address address() const;
    uint32_t data() const;
    bool imm_sign() const;
    enum Type type() const;
    enum InstructionFlags flags() const;
    AluCombinedOp alu_op() const;
    enum AccessControl mem_ctl() const;

    void flags_alu_op_mem_ctl(
        enum InstructionFlags &flags,
        AluCombinedOp &alu_op,
        enum AccessControl &mem_ctl) const;

    bool operator==(const Instruction &c) const;
    bool operator!=(const Instruction &c) const;
    Instruction &operator=(const Instruction &c);

    QString to_str(Address inst_addr = Address::null()) const;

    /**
     * Parses instruction from string containing one assembler line.
     *
     * @throws Instruction::ParseError if unable to parse
     */
    static size_t code_from_string(
        uint32_t *code,
        size_t buffsize,
        QString str,
        Address inst_addr,
        RelocExpressionList *reloc = nullptr,
        const QString &filename = "",
        unsigned line = 0,
        bool pseudoinst_enabled = true);

    /**
     * Parses instruction from prepare tokenized form.
     *
     * @throws Instruction::ParseError if unable to parse
     */
    static size_t code_from_tokens(
        uint32_t *code,
        size_t buffsize,
        TokenizedInstruction &inst,
        RelocExpressionList *reloc = nullptr,
        bool pseudoinst_enabled = true);

    bool update(int64_t val, RelocExpression *relocexp);

    static void append_recognized_instructions(QStringList &list);
    static void set_symbolic_registers(bool enable);
    static void append_recognized_registers(QStringList &list);
    static constexpr uint64_t modify_pseudoinst_imm(Modifier mod, uint64_t value);

private:
    uint32_t dt;
    static bool symbolic_registers_enabled;

    static Instruction base_from_tokens(
        const TokenizedInstruction &inst,
        RelocExpressionList *reloc,
        Modifier pseudo_mod = Modifier::NONE,
        uint64_t initial_immediate_value = 0);
    inline int32_t extend(uint32_t value, uint32_t used_bits) const;
    static uint32_t parse_field(
        QString &field_token,
        const QString &arg,
        Address inst_addr,
        RelocExpressionList *reloc,
        const QString &filename,
        unsigned int line,
        Modifier pseudo_mod,
        uint64_t initial_immediate_value);
    static size_t partially_apply(
        const char *base,
        int argument_count,
        int position,
        const char *value,
        uint32_t *code,
        size_t buffsize,
        TokenizedInstruction &inst,
        RelocExpressionList *reloc);
    static size_t pseudo_from_tokens(
        uint32_t *code,
        size_t buffsize,
        TokenizedInstruction &inst,
        RelocExpressionList *reloc);
};

struct Instruction::ParseError : public std::exception {
    QString message;

    explicit ParseError(QString message);

    const char *what() const noexcept override { return message.toUtf8().data(); }
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
