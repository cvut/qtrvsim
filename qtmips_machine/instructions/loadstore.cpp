#include "loadstore.h"

InstructionLoad::InstructionLoad(enum InstructionLoadStoreT type, std::uint8_t rs, std::uint8_t rt, std::uint16_t offset)
    : InstructionI(rs, rt, offset) {
    this->type = type;
}

std::vector<std::string> InstructionLoad::to_strs() {
    std::vector<std::string> str = this->InstructionI::to_strs();
    switch (this->type) {
    case ILST_B:
        str[0] = "lb";
        break;
    case ILST_HW:
        str[0] = "lh";
        break;
    case ILST_WL:
        str[0] = "lwl";
        break;
    case ILST_W:
        str[0] = "lw";
        break;
    case ILST_BU:
        str[0] = "lbu";
        break;
    case ILST_HU:
        str[0] = "lhu";
        break;
    case ILST_WR:
        str[0] = "lwr";
        break;
    default:
        // TODO different exception
        throw std::exception();
    }
    return str;
}

InstructionStore::InstructionStore(enum InstructionLoadStoreT type, std::uint8_t rs, std::uint8_t rt, std::uint16_t offset)
    : InstructionI(rs, rt, offset) {
    this->type = type;
}

std::vector<std::string> InstructionStore::to_strs() {
    std::vector<std::string> str = this->InstructionI::to_strs();
    switch (this->type) {
    case ILST_B:
        str[0] = "sb";
        break;
    case ILST_HW:
        str[0] = "sh";
        break;
    case ILST_WL:
        str[0] = "swl";
        break;
    case ILST_W:
        str[0] = "sw";
        break;
    case ILST_WR:
        str[0] = "swr";
        break;
    default:
        // TODO different exception
        throw std::exception();
    }
    return str;
}
