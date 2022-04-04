/**
 * @file
 *
 * NOTE ON TERMINOLOGY:
 *
 * - Argument   variable argument of instruction in assembly or during execution.
 *              (Generally, when we think about instructions more line functions.)
 * - Field      same value as argument but in context of encoding.
 * - Subfield   continuous part of encoded instruction, encoding some bit range of the field.
 *
 * This terminology is based on the RISC-V Instruction Set Manual: Volume I.
 */

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "common/containers/constexpr/cstring.h"
#include "execute/alu.h"
#include "execute/alu_op.h"
#include "instruction/bit_arg.h"
#include "instruction/instruction_flags.h"
#include "machinedefs.h"
#include "memory/address.h"

#include <QString>
#include <QStringList>

namespace machine {

struct TokenizedInstruction;
struct RelocationDesc;
using RelocExpressionList = std::vector<RelocationDesc *>;
class InstructionSemantics;
struct InstructionMap;

/**
 * Raw instruction representation.
 *
 * Provides "syntactic"/encoding-level manipulation of instructions (with no memory overhead).
 * This includes encoding, decoding and field access.
 * To get information about instruction behaviour a lookup to instruction map has to be performed.
 * `InstructionSemantics` provides access to instruction map data without repeated lookup.
 */
class Instruction {
public:
    /** Instruction type as in ISA specification */
    enum class Type { R, I, S, B, U, J, UNKNOWN };

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

    /** Exception which is raised when unable to parse assembly. */
    struct ParseError;

    /** Type used to store code of an instruction. */
    using data_type = uint32_t;
    /** Type of an immediate value. */
    using imm_type = int32_t;

    constexpr Instruction() : dt(0) {};
    explicit constexpr Instruction(data_type code) : dt(code) {};

    constexpr Instruction(const Instruction &) = default;
    constexpr Instruction &operator=(const Instruction &) = default;

    /** Type R */
    constexpr static Instruction create_type_r(
        uint8_t opcode,
        uint8_t rs,
        uint8_t rt,
        uint8_t rd,
        uint8_t shamt,
        uint8_t funct3,
        uint8_t funct7);
    /** Type I */
    constexpr static Instruction
    crate_type_i(uint8_t opcode, uint8_t rs, uint8_t rt, uint16_t immediate);
    /** Type J */
    constexpr static Instruction create_type_j(uint8_t opcode, Address address);

    /** Returns size of instruction in bytes */
    uint8_t size() const;
    uint8_t opcode() const;
    uint8_t rs() const;
    uint8_t rt() const;
    uint8_t rd() const;
    uint8_t shamt() const;
    uint16_t funct() const;
    imm_type immediate() const;
    uint32_t data() const;
    bool imm_sign() const;
    enum Type type() const;

    bool operator==(const Instruction &c) const;
    bool operator!=(const Instruction &c) const;

    /**
     * Disassemble instruction
     *
     * @param inst_addr   instruction address - used to disassemble pc relative instructions
     */
    QString to_str(Address inst_addr) const;

    /**
     * Parses instruction from string containing one assembler line.
     *
     * @param code_output     buffer, where parsed instructions are saved
     * @param buffer_size     available space in buffer (in multiples of uint32_t)
     * @param input           string of one instruction in assembly
     * @param inst_addr       address of the instruction in memory
     * @param reloc           list of dynamic relocations, null if dynamic relocation should be
     *                        disabled
     * @param filename        filename of the source file, if compiled from file, "" otherwise
     * @param line            line in source file, if compiled from file, 0 otherwise
     * @param pseudoinst_enabled  switch for pseudoinstruction support, currently only used
     *                            internally
     * @throws Instruction::ParseError if unable to parse
     * @return size of buffer used (same units as buffer_size)
     */
    static size_t code_from_string(
        uint32_t *code_output,
        size_t buffer_size,
        QString input,
        Address inst_addr,
        RelocExpressionList *reloc = nullptr,
        const QString &filename = "",
        unsigned line = 0,
        bool pseudoinst_enabled = true);

    /**
     * Parses instruction from prepare tokenized form.
     *
     * @param code_output     buffer, where parsed instructions are saved
     * @param buffer_size     available space in buffer (in multiples of uint32_t)
     * @param input           tokens of one instruction in assembly + helper info
     * @param reloc           list of dynamic relocations, null if dynamic relocations should be
     *                        disabled
     * @param pseudoinst_enabled  switch for pseudoinstruction support, currently only used
     *                            internally
     * @throws Instruction::ParseError if unable to parse
     * @return size of buffer used (same units as buffer_size)
     */
    static size_t code_from_tokens(
        uint32_t *code,
        size_t buffer_size,
        TokenizedInstruction &input,
        RelocExpressionList *reloc = nullptr,
        bool pseudoinst_enabled = true);

    /**
     * Updates one field in the instruction with value from dynamic relocation
     *
     * @param new_val       new value for the field, old value will be deleted
     * @param reloc_desc    information about this relocation
     * @return success of value
     */
    bool update_value_with_dynamic_relocation(int64_t new_val, RelocationDesc *reloc_desc);

    /**
     * Appends all recognized instruction names into the supplied list.
     *
     * This is used for syntax highlighting.
     */
    static void get_recognized_instruction_names(QStringList &list);

    /**
     * Appends all recognized register names into the supplied list.
     *
     * This is used for syntax highlighting.
     */
    static void get_recognized_registers(QStringList &list);

    /**
     * Controls usage of register names other than x<n> (x0,x1...).
     */
    static void set_symbolic_registers(bool enable);

    static const Instruction NOP;
    static const Instruction UNKNOWN_INST;

private:
    uint32_t dt;

    static bool symbolic_registers_enabled;

    static Instruction base_from_tokens(
        const TokenizedInstruction &input,
        RelocExpressionList *reloc,
        Modifier pseudo_mod = Modifier::NONE,
        uint64_t initial_immediate_value = 0);
    static uint32_t parse_field(
        QString &field_token,
        const cstring &arg,
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
    static constexpr uint64_t modify_pseudoinst_imm(Modifier mod, uint64_t value);
};

struct Instruction::ParseError : public std::exception {
    QString message;

    explicit ParseError(QString message);

    const char *what() const noexcept override { return message.toUtf8().data(); }
};

/**
 * Interface to properties regarding instruction behavior.
 *
 * This struct exists to cache lookups to instruction map. */
class InstructionSemantics {
public:
    explicit InstructionSemantics(const Instruction &instruction);

    InstructionFlags flags() const;
    AluCombinedOp alu_op() const;
    AccessControl mem_ctl() const;

private:
    const Instruction instruction;
    const InstructionMap &instruction_map;
};

struct RelocationDesc {
    inline RelocationDesc(
        Address location,
        QString expression,
        int64_t offset,
        int64_t min,
        int64_t max,
        const InstructionField *arg,
        QString filename,
        int line,
        Instruction::Modifier pseudo_mod = Instruction::Modifier::NONE)
        : location(location)
        , expression(std::move(expression))
        , offset(offset)
        , min(min)
        , max(max)
        , arg(arg)
        , filename(std::move(filename))
        , line(line)
        , pseudo_mod(pseudo_mod) {}

    Address location;
    QString expression;
    int64_t offset;
    int64_t min;
    int64_t max;
    const InstructionField *arg;
    QString filename;
    int line;
    Instruction::Modifier pseudo_mod;
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

} // namespace machine

#endif // INSTRUCTION_H
