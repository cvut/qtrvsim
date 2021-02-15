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
    Registers(const Registers &);

    uint32_t read_pc() const;        // Return current value of program counter
    uint32_t pc_inc();               // Increment program counter by four bytes
    uint32_t pc_jmp(int32_t offset); // Relative jump from current
                                     // location in program counter
    void pc_abs_jmp(uint32_t address);    // Absolute jump in program counter
                                          // (write to pc)
    void pc_abs_jmp_28(uint32_t address); // Absolute jump in current 256MB
                                          // section (basically J
                                          // implementation)

    uint32_t read_gp(uint8_t i) const;        // Read general-purpose
                                              // register
    void write_gp(uint8_t i, uint32_t value); // Write general-purpose
                                              // register
    uint32_t read_hi_lo(bool hi) const; // true - read HI / false - read LO
    void write_hi_lo(bool hi, uint32_t value);

    bool operator==(const Registers &c) const;
    bool operator!=(const Registers &c) const;

    void reset(); // Reset all values to zero (except pc)

signals:
    void pc_update(uint32_t val);
    void gp_update(uint8_t i, uint32_t val);
    void hi_lo_update(bool hi, uint32_t val);
    void gp_read(uint8_t i, uint32_t val) const;
    void hi_lo_read(bool hi, uint32_t val) const;

private:
    uint32_t gp[31] = { 0 }; // general-purpose registers ($0 is
                             // intentionally skipped)
    uint32_t hi {}, lo {};
    uint32_t pc {}; // program counter
};

} // namespace machine

Q_DECLARE_METATYPE(machine::Registers)

#endif // REGISTERS_H
