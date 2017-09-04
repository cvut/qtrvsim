#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <qstring.h>
#include <qvector.h>
#include "registers.h"
#include "memory.h"

enum InstructionState {
    IS_FETCH,
    IS_DECODE,
    IS_EXECUTE,
    IS_MEMORY,
    IS_WRITE_BACK,
};

class Instruction {
public:
    Instruction();

    // TODO return some info for forwarding, stall, flush
    virtual void decode(Registers *regs); // Read and prepare instructions
    virtual void execute(); // ALU operations
    virtual void memory(Memory *mem); // Read or write to memory
    virtual void write_back(Registers *regs); // Write results to registers

    enum InstructionState state();
    bool running();
    bool done();

    virtual QVector<QString> to_strs() = 0; // Returns all fields of instructions in string

private:
    enum InstructionState st;
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
