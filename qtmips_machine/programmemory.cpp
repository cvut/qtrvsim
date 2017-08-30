#include "programmemory.h"
#include <sstream>
#include "qtmipsexception.h"
#include "instructions/arithmetic.h"
#include "instructions/loadstore.h"
#include "instructions/nop.h"
#include "instructions/shift.h"
#include "utils.h"

ProgramMemory::ProgramMemory(ProgramLoader *loader, MemoryAccess *memory) {
    this->memory = memory;
    // Load program to memory (just dump it byte by byte, decode is done on demand)
    for (int i = 0; i < loader->get_nsec(); i++) {
        std::uint32_t base_address = loader->get_address(i);
        std::vector<std::uint8_t> data = loader->get_data(i);
        for (auto it = data.begin(); it < data.end(); it++) {
            memory->write_byte(base_address + i, *it);
        }
    }
}

#define MASKSUB(VAR, LEN, OFFSET) ((VAR & ~((1 << (LEN+OFFSET+1)) - 1)) >> OFFSET)

Instruction *ProgramMemory::at(std::uint32_t address) {
    if (address % 4)
        // TODO different exception (unaligned address)
        throw std::exception();
    // Read instruction from memory
    std::uint32_t dt = this->memory->read_word(address);

    // Decode instruction
    Instruction *inst;
    std::uint8_t opcode = dt >> 26; // upper 6 bits
    if (opcode == 0) { // Arithmetic and shift instructions
        return this->decode_r(dt);
    } else if (opcode == 2 || opcode == 3) { // Jump instructions
        return decode_j(dt);
    } else {
        return decode_i(dt);
    }
}

// TODO implement
#define I_UNKNOWN(DATA) throw QTMIPS_EXCEPTION(UnsupportedInstruction, "Unknown instruction, can't decode", to_string_hex(DATA))
#define I_UNSUPPORTED(INST) throw QTMIPS_EXCEPTION(UnsupportedInstruction, "Decoded unsupported unstruction", #INST)

Instruction *ProgramMemory::decode_r(std::uint32_t dt) {
        std::uint8_t func = MASKSUB(dt, 6, 0);
        if (!func)
            return new InstructionNop();

        std::uint8_t rs, rt, rd, sa;
        rs = MASKSUB(dt, 5, 21);
        rt = MASKSUB(dt, 5, 16);
        rd = MASKSUB(dt, 5, 11);
        sa = MASKSUB(dt, 5, 6);

        if (func < 8) { // Shift instructions
            bool variable = false;
            enum InstructionShiftT t;
            switch (func) {
            case 0:
                t = IST_LL;
                break;
            case 2:
                t = IST_RL;
                break;
            case 3:
                t = IST_RA;
                break;
            case 4:
                t = IST_LL;
                variable = true;
                break;
            case 6:
                t = IST_RL;
                variable = true;
                break;
            case 7:
                t = IST_RA;
                variable = true;
                break;
            default:
                I_UNKNOWN(dt);
            }
            if (variable)
                return new InstructionShiftVariable(t, rs, rt, rd);
            else
                return new InstructionShift(t, rt, rd, sa);
        } else if (func < 10) { // Jump instructions
            // TODO
            I_UNKNOWN(dt);
        } else { // TODO filter rest
            I_UNKNOWN(dt);
        }
}

Instruction *ProgramMemory::decode_j(std::uint32_t dt) {
    std::uint32_t address = MASKSUB(dt, 26, 0);
    // TODO
    I_UNKNOWN(dt);
}

Instruction *ProgramMemory::decode_i(std::uint32_t dt) {
    // InstructionI
    std::uint8_t rs, rt;
    rs = MASKSUB(dt, 5, 21);
    rt = MASKSUB(dt, 5, 16);
    std::uint16_t immediate = MASKSUB(dt, 16, 0);
    // TODO
    I_UNKNOWN(dt);
}
