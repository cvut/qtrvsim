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
#include "qtmipsexception.h"

using namespace machine;

// TODO should this be configurable?
//////////////////////////////////////////////////////////////////////////////
/// Program counter initial value
#define PC_INIT 0x80020000
#define SP_INIT 0xbfffff00
//////////////////////////////////////////////////////////////////////////////

Registers::Registers() : QObject() {
    reset();
}

Registers::Registers(const Registers &orig) : QObject() {
    this->pc = orig.read_pc();
    for (std::uint8_t i = 0; i < 31; i++)
        this->gp[i] = orig.read_gp(i + 1);
    this->lo = orig.read_hi_lo(false);
    this->hi = orig.read_hi_lo(true);
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
    emit pc_update(this->pc);
}

void Registers::pc_abs_jmp_28(std::uint32_t address) {
    this->pc_abs_jmp((pc & 0xF0000000) | (address & 0x0FFFFFFF));
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
    emit gp_update(i, value);
    this->gp[i - 1] = value;
}

std::uint32_t Registers::read_hi_lo(bool is_hi) const {
    if (is_hi)
        return hi;
    else
        return lo;
}

void Registers::write_hi_lo(bool is_hi, std::uint32_t value) {
    if (is_hi)
        hi = value;
    else
        lo = value;
    emit hi_lo_update(is_hi, value);
}

bool Registers::operator==(const Registers &c) const {
    if (read_pc() != c.read_pc())
        return false;
    for (std::uint8_t i = 0; i < 31; i++)
        if (read_gp(i) != c.read_gp(i))
            return false;
    if (read_hi_lo(false) != c.read_hi_lo(false))
        return false;
    if (read_hi_lo(true) != c.read_hi_lo(true))
        return false;
    return true;
}

bool Registers::operator!=(const Registers &c) const {
    return ! this->operator==(c);
}

void Registers::reset() {
    pc_abs_jmp(PC_INIT); // Initialize to beginning program section
    for (int i = 1; i < 32; i++)
        write_gp(i, 0);
    write_gp(29, SP_INIT); // initialize to safe RAM area - corresponds to Linux
    write_hi_lo(false, 0);
    write_hi_lo(true, 0);
}
