#include "registers.h"

#include "memory/address.h"
#include "simulator_exception.h"

using namespace machine;

// TODO should this be configurable?
//////////////////////////////////////////////////////////////////////////////
/// Program counter initial value
#define PC_INIT 0x00000200_addr
#define SP_INIT 0xbfffff00_addr
//////////////////////////////////////////////////////////////////////////////

Registers::Registers() : QObject() {
    reset();
}

Registers::Registers(const Registers &orig) : QObject() {
    this->pc = orig.read_pc();
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
    if (reg == 0) {
        return { 0 }; // $0 always reads as 0
    }

    RegisterValue value = this->gp.at(reg);
    emit gp_read(reg, value);
    return value;
}

void Registers::write_gp(RegisterId reg, RegisterValue value) {
    if (reg == 0) {
        return; // Skip write to $0
    }

    this->gp.at(reg) = value;
    emit gp_update(reg, value);
}

bool Registers::operator==(const Registers &c) const {
    if (read_pc() != c.read_pc()) { return false; }
    if (this->gp != c.gp) { return false; }
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
    write_gp(2_reg, SP_INIT.get_raw()); // initialize to safe RAM area -
                                         // corresponds to Linux
}
