#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <qstring.h>
#include <qvector.h>
#include "registers.h"
#include "memory.h"

class Instruction {
public:
    // TODO return types should be according to what instruction can pass from this stage
    //virtual void decode(Registers *regs) = 0; // Read and prepare instructions
    //virtual void execute() = 0; // ALU operations
    //virtual void memory(Memory *mem) = 0; // Read or write to memory
    //virtual void write_back(Registers *regs) = 0; // Write results to registers

    virtual QVector<QString> to_strs() = 0; // Returns all fields of instructions in string
};

class InstructionR : public Instruction {
public:
    InstructionR(std::uint8_t rs, std::uint8_t rd, std::uint8_t rt, std::uint8_t sa);

    QVector<QString> to_strs();
protected:
    std::uint8_t rs, rd, rt, sa;
};

class InstructionI : public Instruction {
public:
    InstructionI(std::uint8_t rs, std::uint8_t rt, std::uint16_t immediate);

    QVector<QString> to_strs();
protected:
    std::uint8_t rs, rt;
    std::uint16_t immediate;
};

class InstructionJ : public Instruction {
public:
    InstructionJ(std::uint32_t address);

    QVector<QString> to_strs();
protected:
    std::uint32_t address;
};

#endif // INSTRUCTION_H
