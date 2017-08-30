#include "instruction.h"
#include <sstream>
#include <iostream>
#include "utils.h"

InstructionR::InstructionR(std::uint8_t rs, std::uint8_t rd, std::uint8_t rt, std::uint8_t sa) {
    this->rs = rs;
    this->rd = rd;
    this->rt = rt;
    this->sa = sa;
}

// TODO for registers output as register ($0)!

std::vector<std::string> InstructionR::to_strs() {
    std::vector<std::string> str;
    // Instruction name
    str.push_back("unknown"); // unknown instruction, should be replaced by child

    // Source register
    str.push_back(to_string_hex((unsigned)this->rs));
    // Target register
    str.push_back(to_string_hex((unsigned)this->rt));
    // Destination register
    str.push_back(to_string_hex((unsigned)this->rd));
    // Shift amount
    str.push_back(to_string_hex((unsigned)this->sa));

    return str;
}

InstructionI::InstructionI(std::uint8_t rs, std::uint8_t rt, std::uint16_t immediate) {
    this->rs = rs;
    this->rt = rt;
    this->immediate = immediate;
}

std::vector<std::string> InstructionI::to_strs() {
    std::vector<std::string> str;
    // Instruction name
    str.push_back("unknown"); // unknown instruction, should be replaced by child

    // Source register
    str.push_back(to_string_hex((unsigned)this->rs));
    // Target register
    str.push_back(to_string_hex((unsigned)this->rt));
    // Immediate value
    str.push_back(to_string_hex((unsigned)this->immediate));

    return str;
}

InstructionJ::InstructionJ(std::uint32_t address) {
    this->address = address;
}

std::vector<std::string> InstructionJ::to_strs() {
    std::vector<std::string> str;
    // Instruction name
    str.push_back("unknown"); // unknown instruction, should be replaced by child

    std::stringstream ss;
    // Source register
    str.push_back(to_string_hex((unsigned)this->address));

    return str;
}
