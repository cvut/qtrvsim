#ifndef ALU_H
#define ALU_H

#include "execute/alu_op.h"
#include "execute/mul_op.h"
#include "register_value.h"

#include <cstdint>

namespace machine {

/**
 * Components available in combined ALU.
 */
enum class AluComponent {
    ALU, //> RV32/64I
    MUL, //> RV32/64M
};

union AluCombinedOp {
    AluOp alu_op;
    MulOp mul_op;
};

/**
 * Dispatcher for specialised ALUs
 *
 * @param op          alu and mul operands are isomorphic
 * @param component   specifies which specialization to use
 * @param w_operation word operation false=64b, true=32b
 * @param modified    see alu64/32
 * @param a           operand 1
 * @param b           operand 2
 * @return            result of specified ALU operation (always, no traps)
 */
[[gnu::const]] RegisterValue alu_combined_operate(
    AluCombinedOp op,
    AluComponent component,
    bool w_operation,
    bool modified,
    RegisterValue a,
    RegisterValue b);

/**
 * RV64I for OP and OP-IMM instructions
 *
 * ALU conforming to Base Integer Instruction Set, Version 2.0.
 *
 * @param op        operation specifier (funct3 in instruction)
 * @param modified  modifies behavior of ADD (to SUB) and SRL (to SRA)
 *                  encoded by bit 30 if applicable
 * @param a         operand 1
 * @param b         operand 2
 * @return          result of specified ALU operation (always, no traps)
 *                  integer type is returned to ensure correct signe extension
 *                  to arbitrary implementation of RegisterValue
 */
[[gnu::const]] int64_t alu64_operate(AluOp op, bool modified, RegisterValue a, RegisterValue b);

/**
 * RV32I for OP and OP-IMM instructions and RV64I OP-32 and OP-IMM-32
 *
 * ALU conforming to Base Integer Instruction Set, Version 2.0.
 *
 * @param op        operation specifier (funct3 in instruction)
 * @param modified  modifies behavior of ADD (to SUB) and SRL (to SRA)
 *                  encoded by bit 30 if applicable
 * @param a         operand 1
 * @param b         operand 2
 * @return          result of specified ALU operation (always, no traps)
 *                  integer type is returned to ensure correct signe extension
 *                  to arbitrary implementation of RegisterValue
 */
[[gnu::const]] int32_t alu32_operate(AluOp op, bool modified, RegisterValue a, RegisterValue b);

/**
 * RV64 "M" for OP instructions
 *
 * Multiplier conforming to Standard Extension for Integer Multiplication and
 * Division, Version 2.0.
 *
 * Implements operation for instructions: MUL, MUL[[S]H], DIV[U], REM[U].
 * Division by zero is defined §7.2 table 7.1 (or see implementation).
 *
 * @param op  operation specifier (funct3 in the instruction)
 * @param a   operand 1
 * @param b   operand 2
 * @return    result of specified operation (always, no traps)
 *            integer type is returned to ensure correct signe extension
 *            to arbitrary implementation of RegisterValue
 */
[[gnu::const]] int64_t mul64_operate(MulOp op, RegisterValue a, RegisterValue b);

/**
 * RV32 "M" for OP instructions and RV64 "M" for OP-32 instructions
 *
 * Multiplier conforming to Standard Extension for Integer Multiplication and
 * Division, Version 2.0.
 *
 * Implements operation for instructions:
 *    RV32: MUL, MUL[[S]H]W, DIV[U]W, REM[U]W.
 *    RV64: MULW, MUL[[S]H]W, DIV[U], REM[U].
 *
 * Division by zero is defined §7.2 table 7.1 (or see implementation).
 *
 * @param op  operation specifier (funct3 in the instruction)
 * @param a   operand 1
 * @param b   operand 2
 * @return    result of specified operation (always, no traps)
 *            integer type is returned to ensure correct signe extension
 *            to arbitrary implementation of RegisterValue
 */
[[gnu::const]] int32_t mul32_operate(MulOp op, RegisterValue a, RegisterValue b);

} // namespace machine

#endif // ALU_H
