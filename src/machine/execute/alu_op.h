#ifndef ALU_OP_H
#define ALU_OP_H

#include <QMetaType>
#include <cstdint>
using std::uint8_t;

namespace machine {

enum class AluOp : uint8_t {
    ADD = 0b000,
    SLL = 0b001,
    SLT = 0b010,
    SLTU = 0b011,
    XOR = 0b100,
    SR = 0b101,
    OR = 0b110,
    AND = 0b111,
};

}

Q_DECLARE_METATYPE(machine::AluOp)

#endif // ALU_OP_H
