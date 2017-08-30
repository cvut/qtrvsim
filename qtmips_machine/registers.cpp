#include "registers.h"
#include "qtmipsexception.h"
#include "utils.h"

// TODO should this be configurable?
//////////////////////////////////////////////////////////////////////////////
/// Program counter initial value
#define PC_INIT 0x80020000
//////////////////////////////////////////////////////////////////////////////

Registers::Registers() {
    this->pc = PC_INIT; // Initialize to beginning program section
    for (int i = 0; i < 31; i++)
        this->gp[i] = 0;
    this->hi = this->lo = 0;
}

std::uint32_t Registers::read_pc() {
    return this->pc;
}

std::uint32_t Registers::pc_inc() {
    this->pc += 4;
    return this->pc;
}

std::uint32_t Registers::pc_jmp(std::int32_t offset) {
    if (offset % 4)
        throw QTMIPS_EXCEPTION(UnalignedJump, "Trying to jump by unaligned offset", to_string_hex(offset));
    this->pc += offset;
    return this->pc;
}

void Registers::pc_abs_jmp(std::uint32_t address) {
    if (address % 4)
        throw QTMIPS_EXCEPTION(UnalignedJump, "Trying to jump to unaligned address", to_string_hex(address));
    this->pc = address;
}

std::uint32_t Registers::read_gp(std::uint8_t i) {
    SANITY_ASSERT(i < 32, std::string("Trying to read from register ") + std::to_string(i));
    if (!i) // $0 always reads as 0
        return 0;
    return this->gp[i - 1];
}

void Registers::write_gp(std::uint8_t i, std::uint32_t value) {
    SANITY_ASSERT(i < 32, std::string("Trying to write to register ") + std::to_string(i));
    if (i == 0) // Skip write to $0
        return;
    this->gp[i - 1] = value;
}

std::uint32_t Registers::read_hi_lo(bool hi) {
    if (hi)
        return this->hi;
    else
        return this->lo;
}

void Registers::write_hi_lo(bool hi, std::uint32_t value) {
    if (hi)
        this->hi = value;
    else
        this->lo = value;
}
