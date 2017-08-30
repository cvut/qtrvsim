#ifndef PROGRAMMEMORY_H
#define PROGRAMMEMORY_H

#include <vector>
#include "programloader.h"
#include "memory.h"
#include "instruction.h"

class ProgramMemory {
public:
    ProgramMemory(ProgramLoader *loader, MemoryAccess *memory);

    Instruction *at(std::uint32_t address); // return instruction isntance for given address

private:
    MemoryAccess *memory;
    Instruction *decode_r(std::uint32_t dt);
    Instruction *decode_j(std::uint32_t dt);
    Instruction *decode_i(std::uint32_t dt);
};

#endif // PROGRAMMEMORY_H
