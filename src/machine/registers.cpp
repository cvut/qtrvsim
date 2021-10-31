#include "registers.h"

#include "memory/address.h"
#include "simulator_exception.h"

using namespace machine;

// TODO should this be configurable?
//////////////////////////////////////////////////////////////////////////////
/// Program counter initial value
#define PC_INIT 0x80020000_addr
#define SP_INIT 0xbfffff00_addr
//////////////////////////////////////////////////////////////////////////////

Registers::Registers() : QObject() {
    reset();
}

Registers::Registers(const Registers &orig) : QObject() {
    this->pc = orig.read_pc();
    this->lo = orig.read_hi_lo(false);
    this->hi = orig.read_hi_lo(true);
    this->gp = orig.gp;
}

Address Registers::read_pc() const {
    return this->pc;
}

void Registers::write_pc(machine::Address address) {
    if (address.get_raw() % 4) {
        throw SIMULATOR_EXCEPTION(
            UnalignedJump, "Trying to jump to unaligned address",
            QString::number(address.get_raw(), 16));
    }
    this->pc = address;
    emit pc_update(this->pc);
}

RegisterValue Registers::read_gp(RegisterId reg) const {
    if (reg.data == 0) {
        return { 0 }; // $0 always reads as 0
    }

    RegisterValue value = this->gp.at(reg.data);
    emit gp_read(reg, value.as_u32());
    return value;
}

void Registers::write_gp(RegisterId reg, RegisterValue value) {
    if (reg.data == 0) {
        return; // Skip write to $0
    }

    this->gp.at(reg.data) = value;
    emit gp_update(reg, value.as_u32());
}

RegisterValue Registers::read_hi_lo(bool is_hi) const {
    RegisterValue value = is_hi ? hi : lo;
    emit hi_lo_read(is_hi, value.as_u32());
    return value;
}

void Registers::write_hi_lo(bool is_hi, RegisterValue value) {
    if (is_hi) {
        hi = value;
    } else {
        lo = value;
    }
    emit hi_lo_update(is_hi, value.as_u32());
}

bool Registers::operator==(const Registers &c) const {
    if (read_pc() != c.read_pc()) {
        return false;
    }
    if (this->gp != c.gp) {
        return false;
    }
    if (read_hi_lo(false).as_u32() != c.read_hi_lo(false).as_u32()) {
        return false;
    }
    if (read_hi_lo(true).as_u32() != c.read_hi_lo(true).as_u32()) {
        return false;
    }
    return true;
}

bool Registers::operator!=(const Registers &c) const {
    return !this->operator==(c);
}

void Registers::reset() {
    write_pc(PC_INIT); // Initialize to beginning program section
    for (int i = 1; i < 32; i++) {
        write_gp(i, 0);
    }
    write_gp(29_reg, SP_INIT.get_raw()); // initialize to safe RAM area -
                                         // corresponds to Linux
    write_hi_lo(false, 0);
    write_hi_lo(true, 0);
}
