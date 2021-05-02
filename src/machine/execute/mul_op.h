#ifndef MUL_OP_H
#define MUL_OP_H

#include <QMetaType>

namespace machine {

enum class MulOp : uint8_t {
    MUL = 0b000,
    MULH = 0b001,
    MULHSU = 0b010,
    MULHU = 0b011,
    DIV = 0b100,
    DIVU = 0b101,
    REM = 0b110,
    REMU = 0b111,
};

}

Q_DECLARE_METATYPE(machine::MulOp)

#endif // MUL_OP_H
