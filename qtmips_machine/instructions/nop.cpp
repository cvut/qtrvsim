#include "nop.h"

std::vector<std::string> InstructionNop::to_strs() {
    std::vector<std::string> str;
    str.push_back("nop");
    return str;
}
