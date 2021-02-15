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

#ifndef TST_MACHINE_H
#define TST_MACHINE_H

#include <QtTest/QTest>
#include <QtTest>

class MachineTests : public QObject {
    Q_OBJECT
private Q_SLOTS:
    // Registers
    void registers_gp0();
    void registers_rw_gp();
    void registers_rw_hi_lo();
    void registers_pc();
    void registers_compare();
    // Memory
    void memory();
    void memory_data();
    void memory_section();
    void memory_section_data();
    void memory_endian();
    void memory_compare();
    void memory_write_ctl();
    void memory_write_ctl_data();
    void memory_read_ctl();
    void memory_read_ctl_data();
    // Program loader
    void program_loader();
    // Instruction
    void instruction();
    void instruction_access();
    // Alu
    void alu();
    void alu_data();
    void alu_trap_overflow();
    void alu_trap_overflow_data();
    void alu_except();
    void alu_except_data();
    // Cache
    // Core depends on cache, therefore it is tested first.
    void cache_data();
    void cache();
    // Core
    void singlecore_regs();
    void singlecore_regs_data();
    void pipecore_regs();
    void pipecore_regs_data();
    void singlecore_jmp();
    void singlecore_jmp_data();
    void pipecore_jmp();
    void pipecore_jmp_data();
    void singlecore_mem();
    void singlecore_mem_data();
    void pipecore_mem();
    void pipecore_mem_data();
    void singlecore_alu_forward();
    void singlecore_alu_forward_data();
    void pipecore_alu_forward();
    void pipecore_alu_forward_data();
    void pipecorestall_alu_forward();
    void pipecorestall_alu_forward_data();
    void singlecore_memory_tests_data();
    void pipecore_nc_memory_tests_data();
    void pipecore_wt_na_memory_tests_data();
    void pipecore_wt_a_memory_tests_data();
    void pipecore_wb_memory_tests_data();
    void singlecore_memory_tests();
    void pipecore_nc_memory_tests();
    void pipecore_wt_na_memory_tests();
    void pipecore_wt_a_memory_tests();
    void pipecore_wb_memory_tests();
};

#endif // TST_MACHINE_H
