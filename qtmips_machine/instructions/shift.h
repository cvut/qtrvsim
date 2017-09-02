#ifndef SHIFT_H
#define SHIFT_H

#include "instruction.h"

enum InstructionShiftT {
    IST_LL, // Left logical
    IST_RL, // Right logical
    IST_RA // Right arithmetic
};

class InstructionShift : public InstructionR {
public:
    InstructionShift(enum InstructionShiftT type, std::uint8_t rt, std::uint8_t rd, std::uint8_t sa);
    QVector<QString> to_strs();
private:
    enum InstructionShiftT type;
};

class InstructionShiftVariable : public InstructionR {
public:
    InstructionShiftVariable(enum InstructionShiftT type, std::uint8_t rs, std::uint8_t rt, std::uint8_t rd);
    QVector<QString> to_strs();
private:
    enum InstructionShiftT type;
};

#endif // SHIFT_H
