#include "utils.h"
#include <sstream>

#define TO_STR_HEX do { std::stringstream ss; ss << std::hex << v; return std::string(ss.str()); } while (false)

std::string to_string_hex(int v) {
    TO_STR_HEX;
}

std::string to_string_hex(unsigned v) {
    TO_STR_HEX;
}

std::string to_string_hex(long v) {
    TO_STR_HEX;
}

std::string to_string_hex(unsigned long v) {
    TO_STR_HEX;
}

std::string to_string_hex(long long v) {
    TO_STR_HEX;
}

std::string to_string_hex(unsigned long long v) {
    TO_STR_HEX;
}

#undef TO_STR_HEX
