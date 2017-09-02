#ifndef NOP_H
#define NOP_H

#include "instruction.h"

class InstructionNop : public Instruction {
public:
    QVector<QString> to_strs();
};

#endif // NOP_H
