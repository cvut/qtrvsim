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

#ifndef TRACER_H
#define TRACER_H

#include <QObject>
#include "qtmipsmachine.h"

class Tracer : public QObject {
    Q_OBJECT
public:
    Tracer(machine::QtMipsMachine *machine);

    // Trace instructions in different stages/sections
    void fetch();
    void decode();
    void execute();
    void memory();
    void writeback();
    // Trace registers
    void reg_pc();
    void reg_gp(std::uint8_t i);
    void reg_lo();
    void reg_hi();

private slots:
    void instruction_fetch(const machine::Instruction &inst, uint32_t inst_addr, machine::ExceptionCause excause, bool valid);
    void instruction_decode(const machine::Instruction &inst, uint32_t inst_addr, machine::ExceptionCause excause, bool valid);
    void instruction_execute(const machine::Instruction &inst, uint32_t inst_addr, machine::ExceptionCause excause, bool valid);
    void instruction_memory(const machine::Instruction &inst, uint32_t inst_addr, machine::ExceptionCause excause, bool valid);
    void instruction_writeback(const machine::Instruction &inst, uint32_t inst_addr, machine::ExceptionCause excause, bool valid);

    void regs_pc_update(std::uint32_t val);
    void regs_gp_update(std::uint8_t i, std::uint32_t val);
    void regs_hi_lo_update(bool hi, std::uint32_t val);

private:
    machine::QtMipsMachine *machine;

    bool gp_regs[32];
    bool r_hi, r_lo;

    bool con_fetch, con_decode, con_execute, con_memory, con_writeback,
         con_regs_pc, con_regs_gp, con_regs_hi_lo;
};

#endif // TRACER_H
