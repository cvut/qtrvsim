#ifndef PROGRAMMEMORY_H
#define PROGRAMMEMORY_H

#include <vector>
#include "programloader.h"
#include "memory.h"
#include "instruction.h"

class ProgramMemory {
public:
    ProgramMemory(MemoryAccess *memory);

    void load(ProgramLoader *l);
    Instruction *at(std::uint32_t address); // return instruction isntance for given address

private:
    MemoryAccess *memory;
    Instruction *decode_r(std::uint32_t dt);
    Instruction *decode_j(std::uint32_t dt, std::uint8_t opcode);
    Instruction *decode_i(std::uint32_t dt, std::uint8_t opcode);
};

#endif // PROGRAMMEMORY_H
