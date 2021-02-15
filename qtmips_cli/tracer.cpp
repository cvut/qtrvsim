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

#include "tracer.h"
#include <iostream>
#include <qtmipsexception.h>

using namespace std;
using namespace machine;

Tracer::Tracer(QtMipsMachine *machine) {
    this->machine = machine;
    for (unsigned i = 0; i < 32; i++)
        gp_regs[i] = false;
    r_hi = false;
    r_lo = false;

    con_regs_pc = false;
    con_regs_gp = false;
    con_regs_hi_lo = false;
}

#define CON(VAR, FROM, SIG, SLT)                                               \
  do {                                                                         \
    if (!VAR) {                                                                \
      connect(FROM, SIG, this, SLT);                                           \
      VAR = true;                                                              \
    }                                                                          \
  } while (false)

void Tracer::fetch() {
  CON(con_fetch, machine->core(), &Core::instruction_fetched,
      &Tracer::instruction_fetch);
}

void Tracer::decode() {
  CON(con_decode, machine->core(), &Core::instruction_decoded,
      &Tracer::instruction_decode);
}

void Tracer::execute() {
  CON(con_execute, machine->core(), &Core::instruction_executed,
      &Tracer::instruction_execute);
}

void Tracer::memory() {
  CON(con_memory, machine->core(), &Core::instruction_memory,
      &Tracer::instruction_memory);
}

void Tracer::writeback() {
  CON(con_writeback, machine->core(), &Core::instruction_writeback,
      &Tracer::instruction_writeback);
}

void Tracer::reg_pc() {
  CON(con_regs_pc, machine->registers(), &Registers::pc_update,
      &Tracer::regs_pc_update);
}

void Tracer::reg_gp(std::uint8_t i) {
  SANITY_ASSERT(i <= 32, "Trying to trace invalid gp.");
  CON(con_regs_gp, machine->registers(), &Registers::gp_update,
      &Tracer::regs_gp_update);
  gp_regs[i] = true;
}

void Tracer::reg_lo() {
  CON(con_regs_hi_lo, machine->registers(), &Registers::hi_lo_update,
      &Tracer::regs_hi_lo_update);
  r_lo = true;
}

void Tracer::reg_hi() {
  CON(con_regs_hi_lo, machine->registers(), &Registers::hi_lo_update,
      &Tracer::regs_hi_lo_update);
  r_hi = true;
}

void Tracer::instruction_fetch(const Instruction &inst, std::uint32_t inst_addr, ExceptionCause excause, bool valid) {
    cout << "Fetch: " << (excause != EXCAUSE_NONE? "!": "") << (valid? inst.to_str(inst_addr).toStdString() : "Idle") << endl;
}

void Tracer::instruction_decode(const machine::Instruction &inst, uint32_t inst_addr, ExceptionCause excause, bool valid) {
    cout << "Decode: " << (excause != EXCAUSE_NONE? "!": "") << (valid? inst.to_str(inst_addr).toStdString() : "Idle") << endl;
}

void Tracer::instruction_execute(const machine::Instruction &inst, uint32_t inst_addr, ExceptionCause excause, bool valid) {
    cout << "Execute: " << (excause != EXCAUSE_NONE? "!": "") << (valid? inst.to_str(inst_addr).toStdString() : "Idle") << endl;
}

void Tracer::instruction_memory(const machine::Instruction &inst, uint32_t inst_addr, ExceptionCause excause, bool valid) {
    cout << "Memory: " << (excause != EXCAUSE_NONE? "!": "") << (valid? inst.to_str(inst_addr).toStdString() : "Idle") << endl;
}

void Tracer::instruction_writeback(const machine::Instruction &inst, uint32_t inst_addr, ExceptionCause excause, bool valid) {
    cout << "Writeback: " << (excause != EXCAUSE_NONE? "!": "") << (valid? inst.to_str(inst_addr).toStdString() : "Idle") << endl;
}

void Tracer::regs_pc_update(std::uint32_t val) {
    cout << "PC:" << hex << val << endl;
}

void Tracer::regs_gp_update(std::uint8_t i, std::uint32_t val) {
    if (gp_regs[i])
        cout << "GP" << dec << (unsigned)i << ":" << hex << val << endl;
}

void Tracer::regs_hi_lo_update(bool hi, std::uint32_t val) {
    if (hi && r_hi)
        cout << "HI:" << hex << val << endl;
    else if (!hi && r_lo)
        cout << "LO:" << hex << val << endl;
}
