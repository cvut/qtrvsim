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

#ifndef REGISTERS_H
#define REGISTERS_H

#include <QObject>
#include <cstdint>

namespace machine {

class Registers : public QObject {
    Q_OBJECT
public:
    Registers();
    Registers(const Registers&);

    std::uint32_t read_pc() const; // Return current value of program counter
    std::uint32_t pc_inc(); // Increment program counter by four bytes
    std::uint32_t pc_jmp(std::int32_t offset); // Relative jump from current location in program counter
    void pc_abs_jmp(std::uint32_t address); // Absolute jump in program counter (write to pc)
    void pc_abs_jmp_28(std::uint32_t address); // Absolute jump in current 256MB section (basically J implementation)

    std::uint32_t read_gp(std::uint8_t i) const; // Read general-purpose register
    void write_gp(std::uint8_t i, std::uint32_t value); // Write general-purpose register
    std::uint32_t read_hi_lo(bool hi) const; // true - read HI / false - read LO
    void write_hi_lo(bool hi, std::uint32_t value);

    bool operator ==(const Registers &c) const;
    bool operator !=(const Registers &c) const;

    void reset(); // Reset all values to zero (except pc)

signals:
    void pc_update(std::uint32_t val);
    void gp_update(std::uint8_t i, std::uint32_t val);
    void hi_lo_update(bool hi, std::uint32_t val);
    void gp_read(std::uint8_t i, std::uint32_t val) const;
    void hi_lo_read(bool hi, std::uint32_t val) const;

private:
    std::uint32_t gp[31]; // general-purpose registers ($0 is intentionally skipped)
    std::uint32_t hi, lo;
    std::uint32_t pc; // program counter
};

}

Q_DECLARE_METATYPE(machine::Registers)

#endif // REGISTERS_H
