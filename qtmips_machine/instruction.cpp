#include "instruction.h"
#include <sstream>
#include <iostream>

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

    std::stringstream ss;
    // Source register
    ss << std::hex << (unsigned) this->rs;
    str.push_back(ss.str());
    ss.str("");
    // Target register
    ss << std::hex << (unsigned) this->rt;
    str.push_back(ss.str());
    ss.str("");
    // Destination register
    ss << std::hex << (unsigned) this->rd;
    str.push_back(ss.str());
    ss.str("");
    // Shift amount
    ss << std::hex << (unsigned) this->sa;
    str.push_back(ss.str());

    return str;
}

InstructionI::InstructionI(std::uint8_t rs, std::uint8_t rt, std::uint16_t immediate) {
    this->rs = rs;
    this->rt = rt;
    this->immediage = immediate;
}

std::vector<std::string> InstructionI::to_strs() {
    std::vector<std::string> str;
    // Instruction name
    str.push_back("unknown"); // unknown instruction, should be replaced by child

    std::stringstream ss;
    // Source register
    ss << std::hex << (unsigned) this->rs;
    str.push_back(ss.str());
    ss.str("");
    // Destination register
    ss << std::hex << (unsigned) this->rt;
    str.push_back(ss.str());
    ss.str("");
    // Immediate value
    ss << std::hex << (unsigned) this->immediage;
    str.push_back(ss.str());

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
    ss << std::hex << (unsigned) this->address;
    str.push_back(ss.str());

    return str;
}
