#include "alu.h"

#include "common/polyfills/mulh64.h"

namespace machine {

RegisterValue alu_combined_operate(
    AluCombinedOp op,
    AluComponent component,
    bool w_operation,
    bool modified,
    RegisterValue a,
    RegisterValue b) {
    switch (component) {
    case AluComponent::ALU:
        return (w_operation) ? alu32_operate(op.alu_op, modified, a, b)
                             : alu64_operate(op.alu_op, modified, a, b);
    case AluComponent::MUL:
        return (w_operation) ? mul32_operate(op.mul_op, a, b) : mul64_operate(op.mul_op, a, b);
    case AluComponent::PASS:
        return a;
    default: qDebug("ERROR, unknown alu component: %hhx", uint8_t(component)); return 0;
    }
}

/**
 * Shift operations are limited to shift by 31 bits.
 * Other bits of the operand may be used as flags and need to be masked out
 * before any ALU operation is performed.
 */
constexpr uint64_t SHIFT_MASK32 = 0b011111; // == 31
constexpr uint64_t SHIFT_MASK64 = 0b111111; // == 63

int64_t alu64_operate(AluOp op, bool modified, RegisterValue a, RegisterValue b) {
    uint64_t _a = a.as_u64();
    uint64_t _b = b.as_u64();

    switch (op) {
    case AluOp::ADD: return _a + ((modified) ? -_b : _b);
    case AluOp::SLL: return _a << (_b & SHIFT_MASK64);
    case AluOp::SLT: return a.as_i64() < b.as_i64();
    case AluOp::SLTU: return _a < _b;
    case AluOp::XOR:
        return _a ^ _b;
        // Most compilers should calculate SRA correctly, but it is UB.
    case AluOp::SR:
        return (modified) ? (a.as_i64() >> (_b & SHIFT_MASK64)) : (_a >> (_b & SHIFT_MASK64));
    case AluOp::OR: return _a | _b;
    case AluOp::AND:
        return ((modified) ? ~_a : _a) & _b; // Modified: clear bits of b using mask
                                             // in a
    default: qDebug("ERROR, unknown alu operation: %hhx", uint8_t(op)); return 0;
    }
}

int32_t alu32_operate(AluOp op, bool modified, RegisterValue a, RegisterValue b) {
    uint32_t _a = a.as_u32();
    uint32_t _b = b.as_u32();

    switch (op) {
    case AluOp::ADD: return _a + ((modified) ? -_b : _b);
    case AluOp::SLL: return _a << (_b & SHIFT_MASK32);
    case AluOp::SLT: return a.as_i32() < b.as_i32();
    case AluOp::SLTU: return _a < _b;
    case AluOp::XOR:
        return _a ^ _b;
        // Most compilers should calculate SRA correctly, but it is UB.
    case AluOp::SR:
        return (modified) ? (a.as_i32() >> (_b & SHIFT_MASK32)) : (_a >> (_b & SHIFT_MASK32));
    case AluOp::OR: return _a | _b;
    case AluOp::AND:
        return ((modified) ? ~_a : _a) & _b; // Modified: clear bits of b using mask in a
    default: qDebug("ERROR, unknown alu operation: %hhx", uint8_t(op)); return 0;
    }
}

int64_t mul64_operate(MulOp op, RegisterValue a, RegisterValue b) {
    switch (op) {
    case MulOp::MUL: return a.as_u64() * b.as_u64();
    case MulOp::MULH: return mulh64(a.as_i64(), b.as_i64());
    case MulOp::MULHSU: return mulhsu64(a.as_i64(), b.as_u64());
    case MulOp::MULHU: return mulhu64(a.as_u64(), b.as_u64());
    case MulOp::DIV:
        if (b.as_i64() == 0) {
            return -1; // Division by zero is defined.
        } else if (a.as_i64() == INT64_MIN && b.as_i64() == -1) {
            return INT64_MIN; // Overflow.
        } else {
            return a.as_i64() / b.as_i64();
        }
    case MulOp::DIVU:
        return (b.as_u64() == 0) ? UINT64_MAX // Division by zero is defined.
                                 : a.as_u64() / b.as_u64();
    case MulOp::REM:
        if (b.as_i64() == 0) {
            return a.as_i64(); // Division by zero is defined.
        } else if (a.as_i64() == INT64_MIN && b.as_i64() == -1) {
            return 0; // Overflow.
        } else {
            return a.as_i64() % b.as_i64();
        }
    case MulOp::REMU:
        return (b.as_u64() == 0) ? a.as_u64() // Division by zero reminder
                                              // is defined.
                                 : a.as_u64() % b.as_u64();
    default: qDebug("ERROR, unknown multiplication operation: %hhx", uint8_t(op)); return 0;
    }
}

int32_t mul32_operate(MulOp op, RegisterValue a, RegisterValue b) {
    switch (op) {
    case MulOp::MUL: return a.as_u32() * b.as_u32();
    case MulOp::MULH: return ((uint64_t)a.as_i32() * (uint64_t)b.as_i32()) >> 32;
    case MulOp::MULHSU: return ((uint64_t)a.as_i32() * (uint64_t)b.as_u32()) >> 32;
    case MulOp::MULHU: return ((uint64_t)a.as_u32() * (uint64_t)b.as_u32()) >> 32;
    case MulOp::DIV:
        if (b.as_i32() == 0) {
            return -1; // Division by zero is defined.
        } else if (a.as_i32() == INT32_MIN && b.as_i32() == -1) {
            return INT32_MIN; // Overflow.
        } else {
            return a.as_i32() / b.as_i32();
        }
    case MulOp::DIVU:
        return (b.as_u32() == 0) ? UINT32_MAX // Division by zero is defined.
                                 : a.as_u32() / b.as_u32();
    case MulOp::REM:
        if (b.as_i32() == 0) {
            return a.as_i32(); // Division by zero is defined.
        } else if (a.as_i32() == INT32_MIN && b.as_i32() == -1) {
            return 0; // Overflow.
        } else {
            return a.as_i32() % b.as_i32();
        }
    case MulOp::REMU:
        return (b.as_u32() == 0) ? a.as_u32() // Division by zero reminder
                                              // is defined.
                                 : a.as_u32() % b.as_u32();
    default: qDebug("ERROR, unknown multiplication operation: %hhx", uint8_t(op)); return 0;
    }
}

} // namespace machine
