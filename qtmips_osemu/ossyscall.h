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

#ifndef OSSYCALL_H
#define OSSYCALL_H

#include <QObject>
#include <qtmipsexception.h>
#include <machineconfig.h>
#include <registers.h>
#include <memory.h>
#include <core.h>
#include <instruction.h>
#include <alu.h>

namespace osemu {

class OsSyscallExceptionHandler : public machine::ExceptionHandler {
    Q_OBJECT
public:
    bool handle_exception(machine::Core *core, machine::Registers *regs,
                          machine::ExceptionCause excause, std::uint32_t inst_addr,
                          std::uint32_t next_addr, std::uint32_t jump_branch_pc,
                          bool in_delay_slot, std::uint32_t mem_ref_addr);
    int syscall_default_handler(std::uint32_t &result, machine::Core *core,
                   std::uint32_t syscall_num,
                   std::uint32_t a1, std::uint32_t a2, std::uint32_t a3,
                   std::uint32_t a4, std::uint32_t a5, std::uint32_t a6,
                   std::uint32_t a7, std::uint32_t a8);

};

}

#endif // OSSYCALL_H
