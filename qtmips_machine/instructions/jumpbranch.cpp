#include "jumpbranch.h"

InstructionJump::InstructionJump(bool link, std::uint32_t address)
    : InstructionJ(address) {
    this->link = link;
}

std::vector<std::string> InstructionJump::to_strs() {
    std::vector<std::string> str = this->InstructionJ::to_strs();
    if (link)
        str[0] = "j";
    else
        str[0] = "jal";
    return str;
}

InstructionJumpRegister::InstructionJumpRegister(bool link, std::uint8_t rs)
    : InstructionR(rs, 0, 0, 0) {
    this->link = link;
}

std::vector<std::string> InstructionJumpRegister::to_strs() {
    std::vector<std::string> str = this->InstructionR::to_strs();
    str.erase(str.begin() + 2, str.end()); // Drop every field after rs
    if (link)
        str[0] = "j";
    else
        str[0] = "jal";
    return str;
}
