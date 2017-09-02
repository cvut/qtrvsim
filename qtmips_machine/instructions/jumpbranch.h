#ifndef JUMPBRANCH_H
#define JUMPBRANCH_H

#include "instruction.h"

class InstructionJump : InstructionJ {
public:
    InstructionJump(bool link, std::uint32_t address);
    QVector<QString> to_strs();
private:
    bool link;
};

class InstructionJumpRegister : InstructionR {
public:
    InstructionJumpRegister(bool link, std::uint8_t rs);
    QVector<QString> to_strs();
private:
    bool link;
};

enum InstructionBranchT {

};

class InstructionBranch : InstructionI {
public:
    InstructionBranch();
    QVector<QString> to_strs();
private:
    // TODO
};

#endif // JUMPBRANCH_H
