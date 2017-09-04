#include "programmemory.h"
#include <sstream>
#include "qtmipsexception.h"
#include "instructions/arithmetic.h"
#include "instructions/loadstore.h"
#include "instructions/nop.h"
#include "instructions/shift.h"
#include <iostream>

ProgramMemory::ProgramMemory(MemoryAccess *memory) {
    this->memory = memory;
}

void ProgramMemory::load(ProgramLoader *loader) {
    // Load program to memory (just dump it byte by byte, decode is done on demand)
    for (unsigned i = 0; i < loader->get_nsec(); i++) {
        std::uint32_t base_address = loader->get_address(i);
        QVector<std::uint8_t> data = loader->get_data(i);
        for (auto it = data.begin(); it < data.end(); it++) {
            memory->write_byte(base_address + i, *it);
        }
    }
}

#define MASKSUB(VAR, LEN, OFFSET) (((VAR) >> (OFFSET)) & ((1 << (LEN)) - 1))

Instruction *ProgramMemory::at(std::uint32_t address) {
    if (address % 4)
        // TODO different exception (unaligned address)
        throw std::exception();
    // Read instruction from memory
    std::uint32_t dt = this->memory->read_word(address);

    // Decode instruction
    if (dt == 0)
        return new InstructionNop();
    std::uint8_t opcode = dt >> 26; // upper 6 bits
    if (opcode == 0) { // Arithmetic and shift instructions
        return this->decode_r(dt);
    } else if (opcode == 2 || opcode == 3) { // Jump instructions
        return decode_j(dt, opcode);
    } else {
        return decode_i(dt, opcode);
    }
}

#define I_UNKNOWN(DATA) throw QTMIPS_EXCEPTION(UnsupportedInstruction, "Unknown instruction, can't decode", QString::number(DATA, 16))
#define I_UNSUPPORTED(INST) throw QTMIPS_EXCEPTION(UnsupportedInstruction, "Decoded unsupported unstruction", #INST)

Instruction *ProgramMemory::decode_r(std::uint32_t dt) {
        std::uint8_t func = MASKSUB(dt, 6, 0);

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
        } else if (func > 31 && func < 44) {
            enum InstructionArithmeticT type;
            switch (func) {
            case 32:
                type = IAT_ADD;
                break;
            case 33:
                type = IAT_ADDU;
                break;
            case 34:
                type = IAT_SUB;
                break;
            case 35:
                type = IAT_SUBU;
                break;
            case 36:
                type = IAT_AND;
                break;
            case 37:
                type = IAT_OR;
                break;
            case 38:
                type = IAT_XOR;
                break;
            case 39:
                type = IAT_NOR;
                break;
            case 42:
                type = IAT_SLT;
                break;
            case 43:
                type = IAT_SLTU;
                break;
            default:
                I_UNKNOWN(dt);
            }
            return new InstructionArithmetic(type, rs, rd, rt);
        } else { // TODO filter rest
            I_UNKNOWN(dt);
        }
}

Instruction *ProgramMemory::decode_j(std::uint32_t dt, std::uint8_t opcode) {
    std::uint32_t address = MASKSUB(dt, 26, 0);
    // TODO
    I_UNKNOWN(dt);
}

Instruction *ProgramMemory::decode_i(std::uint32_t dt, std::uint8_t opcode) {
    // InstructionI
    std::uint8_t rs, rt;
    rs = MASKSUB(dt, 5, 21);
    rt = MASKSUB(dt, 5, 16);
    std::uint16_t immediate = MASKSUB(dt, 16, 0);

    if (opcode > 7 && opcode < 16) {
        enum InstructionArithmeticImmediateT type;
        switch (opcode) {
        case 8:
            type = IAT_ADDI;
            break;
        case 9:
            type = IAT_ADDIU;
            break;
        case 10:
            type = IAT_SLTI;
            break;
        case 11:
            type = IAT_SLTIU;
            break;
        case 12:
            type = IAT_ANDI;
            break;
        case 13:
            type = IAT_ORI;
            break;
        case 14:
            type = IAT_XORI;
            break;
        case 15:
            type = IAT_LUI;
            break;
        default:
            I_UNKNOWN(dt);
        }
        return new InstructionArithmeticImmediate(type, rs, rt, immediate);
    } else if (opcode > 31 && opcode < 47) {
        enum InstructionLoadStoreT type;
        bool isload = false;
        switch (opcode) {
        case 32:
            type = ILST_B;
            isload = true;
            break;
        case 33:
            type = ILST_HW;
            isload = true;
            break;
        case 34:
            type = ILST_WL;
            isload = true;
            break;
        case 35:
            type = ILST_W;
            isload = true;
            break;
        case 36:
            type = ILST_BU;
            isload = true;
            break;
        case 37:
            type = ILST_HU;
            isload = true;
            break;
        case 38:
            type = ILST_WR;
            isload = true;
            break;
        case 40:
            type = ILST_B;
            break;
        case 41:
            type = ILST_HW;
            break;
        case 42:
            type = ILST_WL;
            break;
        case 43:
            type = ILST_W;
            break;
        case 46:
            type = ILST_WR;
            break;
        default:
            I_UNKNOWN(dt);
        }
        if (isload)
            return new InstructionLoad(type, rs, rt, immediate);
        else
            return new InstructionStore(type, rs, rt, immediate);
    } else
        I_UNKNOWN(dt);
}
