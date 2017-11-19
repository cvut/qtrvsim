#include "registers.h"
#include "qtmipsexception.h"

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

Registers::Registers(const Registers *orig) : Registers() {
    this->pc = orig->read_pc();
    for (int i = 0; i < 31; i++)
        this->gp[i] = orig->read_gp(i);
    this->lo = orig->read_hi_lo(false);
    this->hi = orig->read_hi_lo(true);
}

std::uint32_t Registers::read_pc() const {
    return this->pc;
}

std::uint32_t Registers::pc_inc() {
    this->pc += 4;
    emit pc_update(this->pc);
    return this->pc;
}

std::uint32_t Registers::pc_jmp(std::int32_t offset) {
    if (offset % 4)
        throw QTMIPS_EXCEPTION(UnalignedJump, "Trying to jump by unaligned offset", QString::number(offset, 16));
    this->pc += offset;
    emit pc_update(this->pc);
    return this->pc;
}

void Registers::pc_abs_jmp(std::uint32_t address) {
    if (address % 4)
        throw QTMIPS_EXCEPTION(UnalignedJump, "Trying to jump to unaligned address", QString::number(address, 16));
    this->pc = address;
}

std::uint32_t Registers::read_gp(std::uint8_t i) const {
    SANITY_ASSERT(i < 32, QString("Trying to read from register ") + QString(i));
    if (!i) // $0 always reads as 0
        return 0;
    return this->gp[i - 1];
}

void Registers::write_gp(std::uint8_t i, std::uint32_t value) {
    SANITY_ASSERT(i < 32, QString("Trying to write to register ") + QString(i));
    if (i == 0) // Skip write to $0
        return;
    this->gp[i - 1] = value;
}

std::uint32_t Registers::read_hi_lo(bool hi) const {
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

bool Registers::operator ==(const Registers &c) const {
    if (read_pc() != c.read_pc())
        return false;
    for (int i = 0; i < 31; i++)
        if (read_gp(i) != c.read_gp(i))
            return false;
    if (read_hi_lo(false) != c.read_hi_lo(false))
        return false;
    if (read_hi_lo(true) != c.read_hi_lo(true))
        return false;
    return true;
}
