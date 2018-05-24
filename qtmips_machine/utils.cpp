#include "utils.h"

std::uint32_t sign_extend(std::uint16_t v) {
    return ((v & 0x8000) ? 0xFFFF0000 : 0) | v;
}
