// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

#include "registers.h"

#include "memory/address.h"
#include "qtmipsexception.h"

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

Address Registers::pc_inc() {
    this->pc += 4;
    emit pc_update(this->pc);
    return this->pc;
}

Address Registers::pc_jmp(int32_t offset) {
    if (offset % 4) {
        throw QTMIPS_EXCEPTION(
            UnalignedJump, "Trying to jump by unaligned offset",
            QString::number(offset, 16));
    }
    this->pc += offset;
    emit pc_update(this->pc);
    return this->pc;
}

void Registers::pc_abs_jmp(machine::Address address) {
    if (address.get_raw() % 4) {
        throw QTMIPS_EXCEPTION(
            UnalignedJump, "Trying to jump to unaligned address",
            QString::number(address.get_raw(), 16));
    }
    this->pc = address;
    emit pc_update(this->pc);
}

void Registers::pc_abs_jmp_28(Address address) {
    this->pc_abs_jmp((pc & 0xF0000000) | (address & 0x0FFFFFFF).get_raw());
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
    pc_abs_jmp(PC_INIT); // Initialize to beginning program section
    for (int i = 1; i < 32; i++) {
        write_gp(i, 0);
    }
    write_gp(29_reg, SP_INIT.get_raw()); // initialize to safe RAM area -
                                         // corresponds to Linux
    write_hi_lo(false, 0);
    write_hi_lo(true, 0);
}
