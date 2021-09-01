#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "machinedefs.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <utility>

namespace machine {

enum InstructionFlags {
    IMF_NONE = 0,
    IMF_SUPPORTED = 1L << 0, /**< Instruction is supported */
    IMF_MEMWRITE = 1L << 1,  /**< Write to the memory when memory stage is
                                reached */
    IMF_MEMREAD = 1L << 2,   /**< Read from the memory when memory stage is
                                reached */
    IMF_ALUSRC = 1L << 3,    /**< The second ALU source is immediate operand */
    IMF_REGD = 1L << 4, /**< RD field specifies register to be updated, if not
                             set and REGWRITE = 1, then destination reg in RT */
    IMF_REGWRITE = 1L << 5, /**< Instruction result (ALU or memory) is written
                               to register file */
    IMF_ZERO_EXTEND = 1L << 6, /**< Immediate operand is zero extended, else
                                  sign */
    IMF_PC_TO_R31 = 1L << 7,  /**< PC value will be stored to register R31/RA */
    IMF_BJR_REQ_RS = 1L << 8, /**< Branch or jump operation reguires RS value */
    IMF_BJR_REQ_RT = 1L << 9, /**< Branch or jump operation requires RT value */
    IMF_ALU_SHIFT = 1L << 10, /**< Operation is shift of RT by RS or SHAMT */
    IMF_MEM = 1L << 11,       /**< Instruction is memory access instruction */
    IMF_ALU_REQ_RS = 1L << 12, /**< Execution phase/ALU requires RS value */
    IMF_ALU_REQ_RT = 1L << 13, /**< Execution phase/ALU/mem requires RT value */
    IMF_READ_HILO = 1L << 14,  /**< Operation reads value from  HI or LO
                                  registers */
    IMF_WRITE_HILO = 1L << 15, /**< Operation writes value to HI and/or LO
                                  registers */
    IMF_PC8_TO_RT = 1L << 16,  /**< PC value will be stored in RT specified
                                  register */
    IMF_BRANCH = 1L << 17, /**< Operation is conditional or unconditional branch
                                or branch and link when PC_TO_R31 is set */
    IMF_JUMP = 1L << 18,   /**< Jump operation - J, JAL, JR or JALR */
    IMF_BJ_NOT = 1L << 19, /**< Negate condition for branch instructiion */
    IMF_BGTZ_BLEZ = 1L << 20, /**< BGTZ/BLEZ, else BEGT/BLTZ or BEQ, BNE when RT
                               */
    IMF_NB_SKIP_DS = 1L << 21, /**< Skip instruction in delay slot if branch not
                                  taken */
    IMF_EXCEPTION = 1L << 22,  /**< Instruction causes synchronous exception */
    IMF_STOP_IF = 1L << 23,    /**< Stop instruction fetch until instruction
                                  processed */
};

struct RelocExpression {
    // TODO is location a address
    inline RelocExpression(
        Address location,
        QString expression,
        int64_t offset,
        int64_t min,
        int64_t max,
        unsigned lsb_bit,
        unsigned bits,
        unsigned shift,
        QString filename,
        int line,
        int options) {
        this->location = location;
        this->expression = std::move(expression);
        this->offset = offset;
        this->min = min;
        this->max = max;
        this->lsb_bit = lsb_bit;
        this->bits = bits;
        this->shift = shift;
        this->filename = std::move(filename);
        this->line = line;
        this->options = options;
    }
    Address location;
    QString expression;
    int64_t offset;
    int64_t min;
    int64_t max;
    unsigned lsb_bit;
    unsigned bits;
    unsigned shift;
    QString filename;
    int line;
    int options;
};

typedef QVector<RelocExpression *> RelocExpressionList;

class Instruction {
public:
    Instruction();
    explicit Instruction(uint32_t inst);
    Instruction(
        uint8_t opcode,
        uint8_t rs,
        uint8_t rt,
        uint8_t rd,
        uint8_t shamt,
        uint8_t funct); // Type R
    Instruction(uint8_t opcode, uint8_t rs, uint8_t rt,
                uint16_t immediate);              // Type I
    Instruction(uint8_t opcode, Address address); // Type J
    Instruction(const Instruction &);

    enum Type { T_R, T_I, T_J, T_UNKNOWN };

    uint8_t opcode() const;
    uint8_t rs() const;
    uint8_t rt() const;
    uint8_t rd() const;
    uint8_t shamt() const;
    uint8_t funct() const;
    uint8_t cop0sel() const;
    uint16_t immediate() const;
    Address address() const;
    uint32_t data() const;
    enum Type type() const;
    enum InstructionFlags flags() const;
    enum AluOp alu_op() const;
    enum AccessControl mem_ctl() const;
    enum ExceptionCause encoded_exception() const;

    void flags_alu_op_mem_ctl(
        enum InstructionFlags &flags,
        enum AluOp &alu_op,
        enum AccessControl &mem_ctl) const;

    bool is_break() const;

    bool operator==(const Instruction &c) const;
    bool operator!=(const Instruction &c) const;
    Instruction &operator=(const Instruction &c);

    QString to_str(Address inst_addr = Address::null()) const;

    static ssize_t code_from_string(
        uint32_t *code,
        size_t buffsize,
        const QString &inst_base,
        QStringList &inst_fields,
        QString &error,
        Address inst_addr = Address::null(),
        RelocExpressionList *reloc = nullptr,
        const QString &filename = "",
        int line = 0,
        bool pseudo_opt = false,
        int options = 0);

    static ssize_t code_from_string(
        uint32_t *code,
        size_t buffsize,
        QString str,
        QString &error,
        Address inst_addr = Address::null(),
        RelocExpressionList *reloc = nullptr,
        const QString &filename = "",
        int line = 0,
        bool pseudo_opt = false,
        int options = 0);

    bool update(int64_t val, RelocExpression *relocexp);

    static void append_recognized_instructions(QStringList &list);
    static void set_symbolic_registers(bool enable);
    static void append_recognized_registers(QStringList &list);

private:
    uint32_t dt;
    static bool symbolic_registers_fl;
};

} // namespace machine

Q_DECLARE_METATYPE(machine::Instruction)

#endif // INSTRUCTION_H
