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

#include "qtmips_machine/core.h"
#include "qtmips_machine/machineconfig.h"
#include "qtmips_machine/memory/backend/memory.h"
#include "qtmips_machine/memory/cache/cache.h"
#include "qtmips_machine/memory/memory_bus.h"
#include "tst_machine.h"

#include <QVector>

using namespace machine;

static void core_regs_data() {
    QTest::addColumn<Instruction>("i");
    QTest::addColumn<Registers>("init");
    QTest::addColumn<Registers>("res");
    // Note that we shouldn't be touching program counter as that is handled
    // automatically and differs if we use pipelining

    // Arithmetic instructions
    {
        Registers regs_init;
        regs_init.write_gp(24, 24);
        regs_init.write_gp(25, 12);
        Registers regs_res(regs_init);
        regs_res.write_gp(26, 36);
        QTest::newRow("ADD")
            << Instruction(0, 24, 25, 26, 0, 32) << regs_init << regs_res;
        QTest::newRow("ADDU")
            << Instruction(0, 24, 25, 26, 0, 33) << regs_init << regs_res;
        QTest::newRow("ADDI")
            << Instruction(8, 24, 26, 12) << regs_init << regs_res;
        QTest::newRow("ADDIU")
            << Instruction(9, 24, 26, 12) << regs_init << regs_res;
        regs_res.write_gp(26, 12);
        QTest::newRow("SUB")
            << Instruction(0, 24, 25, 26, 0, 34) << regs_init << regs_res;
        QTest::newRow("SUBU")
            << Instruction(0, 24, 25, 26, 0, 35) << regs_init << regs_res;
    }
    {
        Registers regs_init;
        regs_init.write_gp(24, 12);
        regs_init.write_gp(25, 24);
        Registers regs_res(regs_init);
        regs_res.write_gp(26, 1);
        QTest::newRow("SLT")
            << Instruction(0, 24, 25, 26, 0, 42) << regs_init << regs_res;
        QTest::newRow("SLTU")
            << Instruction(0, 24, 25, 26, 0, 43) << regs_init << regs_res;
        QTest::newRow("SLTI")
            << Instruction(10, 24, 26, 24) << regs_init << regs_res;
        QTest::newRow("SLTIU")
            << Instruction(11, 24, 26, 24) << regs_init << regs_res;
    }

    // Shift instructions
    {
        Registers regs_init;
        regs_init.write_gp(24, 0xf0);
        regs_init.write_gp(25, 3);
        Registers regs_res(regs_init);
        regs_res.write_gp(26, 0x780);
        QTest::newRow("SLL")
            << Instruction(0, 0, 24, 26, 3, 0) << regs_init << regs_res;
        QTest::newRow("SLLV")
            << Instruction(0, 25, 24, 26, 0, 4) << regs_init << regs_res;
        regs_res.write_gp(26, 0x1e);
        QTest::newRow("SLR")
            << Instruction(0, 0, 24, 26, 3, 2) << regs_init << regs_res;
        QTest::newRow("SLRV")
            << Instruction(0, 25, 24, 26, 0, 6) << regs_init << regs_res;
    }
    {
        Registers regs_init;
        regs_init.write_gp(24, 0x800000f0);
        regs_init.write_gp(25, 3);
        Registers regs_res(regs_init);
        regs_res.write_gp(26, 0xF000001e);
        QTest::newRow("SRA")
            << Instruction(0, 0, 24, 26, 3, 3) << regs_init << regs_res;
        QTest::newRow("SRAV")
            << Instruction(0, 25, 24, 26, 0, 7) << regs_init << regs_res;
    }

    // Logical instructions
    {
        Registers regs_init;
        regs_init.write_gp(24, 0xf0);
        regs_init.write_gp(25, 0xe1);
        Registers regs_res(regs_init);
        regs_res.write_gp(26, 0xe0);
        QTest::newRow("AND")
            << Instruction(0, 24, 25, 26, 0, 36) << regs_init << regs_res;
        QTest::newRow("ANDI")
            << Instruction(12, 24, 26, 0xe1) << regs_init << regs_res;
        regs_res.write_gp(26, 0xf1);
        QTest::newRow("OR")
            << Instruction(0, 24, 25, 26, 0, 37) << regs_init << regs_res;
        QTest::newRow("ORI")
            << Instruction(13, 24, 26, 0xe1) << regs_init << regs_res;
        regs_res.write_gp(26, 0x11);
        QTest::newRow("XOR")
            << Instruction(0, 24, 25, 26, 0, 38) << regs_init << regs_res;
        QTest::newRow("XORI")
            << Instruction(14, 24, 26, 0xe1) << regs_init << regs_res;
        regs_res.write_gp(26, 0xffffff0e);
        QTest::newRow("NOR")
            << Instruction(0, 24, 25, 26, 0, 39) << regs_init << regs_res;
        regs_res.write_gp(26, 0xf00f0000);
        QTest::newRow("LUI")
            << Instruction(15, 0, 26, 0xf00f) << regs_init << regs_res;
    }

    // Move instructions
    {
        Registers regs_init;
        regs_init.write_hi_lo(true, 24);
        regs_init.write_hi_lo(false, 28);
        regs_init.write_gp(24, 55);
        regs_init.write_gp(25, 56);
        regs_init.write_gp(27, 21);
        regs_init.write_gp(28, 22);
        Registers regs_res(regs_init);
        regs_res.write_gp(26, 24);
        QTest::newRow("MFHI")
            << Instruction(0, 0, 0, 26, 0, 16) << regs_init << regs_res;
        regs_res.write_gp(26, 28);
        QTest::newRow("MFLO")
            << Instruction(0, 0, 0, 26, 0, 18) << regs_init << regs_res;
        regs_res.write_gp(26, 0);
        regs_res.write_hi_lo(true, 21);
        QTest::newRow("MTHI")
            << Instruction(0, 27, 0, 0, 0, 17) << regs_init << regs_res;
        regs_res.write_hi_lo(true, 24);
        regs_res.write_hi_lo(false, 22);
        QTest::newRow("MTLO")
            << Instruction(0, 28, 0, 0, 0, 19) << regs_init << regs_res;
        regs_res.write_hi_lo(false, 28);
        QTest::newRow("MOVZ-F")
            << Instruction(0, 24, 24, 25, 0, 10) << regs_init << regs_res;
        QTest::newRow("MOVN-F")
            << Instruction(0, 24, 1, 25, 0, 11) << regs_init << regs_res;
        regs_res.write_gp(25, 55);
        QTest::newRow("MOVZ-T")
            << Instruction(0, 24, 1, 25, 0, 10) << regs_init << regs_res;
        QTest::newRow("MOVN-T")
            << Instruction(0, 24, 24, 25, 0, 11) << regs_init << regs_res;
    }
}

void MachineTests::singlecore_regs_data() {
    core_regs_data();
}

void MachineTests::pipecore_regs_data() {
    core_regs_data();
}

void MachineTests::singlecore_regs() {
    QFETCH(Instruction, i);
    QFETCH(Registers, init);
    QFETCH(Registers, res);

    Memory mem(BIG); // Just memory (it shouldn't be used here except
                     // instruction)
    TrivialBus mem_frontend(&mem);
    memory_write_u32(&mem, res.read_pc().get_raw(), i.data()); // Store single
                                                               // instruction
                                                               // (anything else
                                                               // should be 0 so
                                                               // NOP
                                                               // effectively)
    Memory mem_used(mem); // Create memory copy
    TrivialBus mem_used_frontend(&mem);

    CoreSingle core(&init, &mem_used_frontend, &mem_used_frontend, true);
    core.step(); // Single step should be enought as this is risc without
                 // pipeline
    core.step();

    res.pc_inc();
    res.pc_inc(); // We did single step	so increment program counter accordingly
    QCOMPARE(init, res); // After doing changes from initial state this should
                         // be same state as in case of passed expected result
}

void MachineTests::pipecore_regs() {
    QFETCH(Instruction, i);
    QFETCH(Registers, init);
    QFETCH(Registers, res);

    Memory mem(BIG); // Just memory (it shouldn't be used here except
                     // instruction)
    TrivialBus mem_frontend(&mem);
    memory_write_u32(&mem, res.read_pc().get_raw(), i.data()); // Store single
                                                               // instruction
                                                               // (anything else
                                                               // should be 0 so
                                                               // NOP
                                                               // effectively)

    Memory mem_used(mem);
    TrivialBus mem_used_frontend(&mem_used);

    res.pc_jmp(0x14);

    CorePipelined core(&init, &mem_used_frontend, &mem_used_frontend);
    for (int i = 0; i < 5; i++) {
        core.step(); // Fire steps for five pipelines stages
    }

    // cout << "well:" << init.read_gp(26) << ":" << regs_used.read_gp(26) <<
    // endl;
    QCOMPARE(init, res); // After doing changes from initial state this should
                         // be same state as in case of passed expected result
    QCOMPARE(mem, mem_used); // There should be no change in memory
}

static void core_jmp_data() {
    QTest::addColumn<Instruction>("i");
    QTest::addColumn<Registers>("regs");
    QTest::addColumn<uint64_t>("pc");

    Registers regs;
    regs.write_gp(14, -22);
    regs.write_gp(15, 22);
    regs.write_gp(16, -22);
    regs.write_gp(12, 0x80040000);
    QTest::newRow("B") << Instruction(4, 0, 0, 61) << regs
                       << regs.read_pc().get_raw() + 4 + (61 << 2);
    QTest::newRow("BEQ") << Instruction(4, 14, 16, 61) << regs
                         << regs.read_pc().get_raw() + 4 + (61 << 2);
    QTest::newRow("BEQ-BACK") << Instruction(4, 14, 16, -4) << regs
                              << regs.read_pc().get_raw() + 4 - 16;
    QTest::newRow("BNE") << Instruction(5, 14, 15, 61) << regs
                         << regs.read_pc().get_raw() + 4 + (61 << 2);
    QTest::newRow("BGEZ") << Instruction(1, 15, 1, 61) << regs
                          << regs.read_pc().get_raw() + 4 + (61 << 2);
    QTest::newRow("BGTZ") << Instruction(7, 15, 0, 61) << regs
                          << regs.read_pc().get_raw() + 4 + (61 << 2);
    QTest::newRow("BLEZ") << Instruction(6, 14, 0, 61) << regs
                          << regs.read_pc().get_raw() + 4 + (61 << 2);
    QTest::newRow("BLTZ") << Instruction(1, 14, 0, 61) << regs
                          << regs.read_pc().get_raw() + 4 + (61 << 2);
    QTest::newRow("J") << Instruction(2, Address(24)) << regs
                       << Address(0x80000000).get_raw() + (24 << 2);
    QTest::newRow("JR") << Instruction(0, 12, 0, 0, 0, 8) << regs
                        << Address(0x80040000).get_raw();
}

void MachineTests::singlecore_jmp_data() {
    core_jmp_data();
}

void MachineTests::pipecore_jmp_data() {
    core_jmp_data();
}

void MachineTests::singlecore_jmp() {
    QFETCH(Instruction, i);
    QFETCH(Registers, regs);
    QFETCH(uint64_t, pc);

    Memory mem(BIG);
    TrivialBus mem_frontend(&mem);
    memory_write_u32(&mem, regs.read_pc().get_raw(), i.data());
    Memory mem_used(mem);
    TrivialBus mem_used_frontend(&mem_used);
    Registers regs_used(regs);

    CoreSingle core(&regs_used, &mem_used_frontend, &mem_used_frontend, true);
    core.step();
    QCOMPARE(regs.read_pc() + 4, regs_used.read_pc()); // First execute delay
                                                       // slot
    core.step();
    QCOMPARE(pc, regs_used.read_pc().get_raw()); // Now do jump

    QCOMPARE(mem, mem_used);              // There should be no change in memory
    regs_used.pc_abs_jmp(regs.read_pc()); // Reset program counter before we do
                                          // registers compare
    QCOMPARE(regs, regs_used); // There should be no change in registers now
}

void MachineTests::pipecore_jmp() {
    QFETCH(Instruction, i);
    QFETCH(Registers, regs);
    QFETCH(uint64_t, pc);

    Memory mem(BIG);
    TrivialBus mem_frontend(&mem);
    memory_write_u32(&mem, regs.read_pc().get_raw(), i.data());
    Memory mem_used(mem);
    TrivialBus mem_used_frontend(&mem_used);
    Registers regs_used(regs);

    CorePipelined core(&regs_used, &mem_used_frontend, &mem_used_frontend);
    core.step();
    QCOMPARE(regs.read_pc() + 4, regs_used.read_pc()); // First just fetch
    core.step();
    QCOMPARE(pc, regs_used.read_pc().get_raw()); // Now do jump
    for (int i = 0; i < 3; i++) {
        core.step(); // Follow up with three other steps to complete pipeline to
    }
    // be sure that instruction has no side effects

    QCOMPARE(mem, mem_used);           // There should be no change in memory
    regs.pc_abs_jmp(Address(pc + 12)); // Set reference pc to three more
                                       // instructions later (where regs_used
                                       // should be)
    QCOMPARE(regs, regs_used); // There should be no change in registers now
                               // (except pc)
}

static void core_mem_data() {
    QTest::addColumn<Instruction>("i");
    QTest::addColumn<Registers>("regs_init");
    QTest::addColumn<Registers>("regs_res");
    QTest::addColumn<Memory>("mem_init");
    QTest::addColumn<Memory>("mem_res");

    // Load
    {
        Memory mem(BIG);
        memory_write_u32(&mem, 0x24, 0xA3242526);
        Registers regs;
        regs.write_gp(1, 0x22);
        Registers regs_res(regs);
        regs_res.write_gp(21, 0xFFFFFFA3);
        QTest::newRow("LB")
            << Instruction(32, 1, 21, 0x2) << regs << regs_res << mem << mem;
        regs_res.write_gp(21, 0xFFFFA324);
        QTest::newRow("LH")
            << Instruction(33, 1, 21, 0x2) << regs << regs_res << mem << mem;
        regs_res.write_gp(21, 0xA3242526);
        QTest::newRow("LW")
            << Instruction(35, 1, 21, 0x2) << regs << regs_res << mem << mem;
        regs_res.write_gp(21, 0x000000A3);
        QTest::newRow("LBU")
            << Instruction(36, 1, 21, 0x2) << regs << regs_res << mem << mem;
        regs_res.write_gp(21, 0x0000A324);
        QTest::newRow("LHU")
            << Instruction(37, 1, 21, 0x2) << regs << regs_res << mem << mem;
    }
    // Store
    {
        Registers regs;
        regs.write_gp(1, 0x22);
        regs.write_gp(21, 0x23242526);
        Memory mem(BIG);
        memory_write_u8(&mem, 0x24, 0x26); // Note: store least significant byte
        QTest::newRow("SB") << Instruction(40, 1, 21, 0x2) << regs << regs
                            << Memory(BIG) << mem;
        memory_write_u16(&mem, 0x24, 0x2526);
        QTest::newRow("SH") << Instruction(41, 1, 21, 0x2) << regs << regs
                            << Memory(BIG) << mem;
        memory_write_u32(&mem, 0x24, 0x23242526);
        QTest::newRow("SW") << Instruction(43, 1, 21, 0x2) << regs << regs
                            << Memory(BIG) << mem;
    }
}

void MachineTests::singlecore_mem_data() {
    core_mem_data();
}

void MachineTests::pipecore_mem_data() {
    core_mem_data();
}

void MachineTests::singlecore_mem() {
    QFETCH(Instruction, i);
    QFETCH(Registers, regs_init);
    QFETCH(Registers, regs_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);

    // Write instruction to both memories
    memory_write_u32(&mem_init, regs_init.read_pc().get_raw(), i.data());
    memory_write_u32(&mem_res, regs_init.read_pc().get_raw(), i.data());

    TrivialBus mem_init_frontend(&mem_init);
    CoreSingle core(&regs_init, &mem_init_frontend, &mem_init_frontend, true);
    core.step();
    core.step();

    regs_res.pc_inc();
    regs_res.pc_inc();
    QCOMPARE(regs_init, regs_res);
    QCOMPARE(mem_init, mem_res);
}

void MachineTests::pipecore_mem() {
    QFETCH(Instruction, i);
    QFETCH(Registers, regs_init);
    QFETCH(Registers, regs_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);

    // Write instruction to both memories
    memory_write_u32(&mem_init, regs_init.read_pc().get_raw(), i.data());
    memory_write_u32(&mem_res, regs_init.read_pc().get_raw(), i.data());

    TrivialBus mem_init_frontend(&mem_init);
    CorePipelined core(&regs_init, &mem_init_frontend, &mem_init_frontend);
    for (int i = 0; i < 5; i++) {
        core.step(); // Fire steps for five pipelines stages
    }

    regs_res.pc_jmp(20);
    QCOMPARE(regs_init, regs_res);
    QCOMPARE(mem_init, mem_res);
}

/*======================================================================*/

static void core_alu_forward_data() {
    QTest::addColumn<QVector<uint32_t>>("code");
    QTest::addColumn<Registers>("reg_init");
    QTest::addColumn<Registers>("reg_res");
    // Note that we shouldn't be touching program counter as that is handled
    // automatically and differs if we use pipelining

    // Test forwarding of ALU operands
    {
        QVector<uint32_t> code {
            // objdump -D to src: ^[^ \t]+[ \t]+([^ \t]+)[ \t]+([^ \t].*)$
            0x20020001, // addi    v0,zero,1
            0x20011111, // addi    at,zero,4369
            0x20012222, // addi    at,zero,8738
            0x00221820, // add     v1,at,v0
            0x00222020, // add     a0,at,v0
            0x20020003, // addi    v0,zero,3
            0x20011111, // addi    at,zero,4369
            0x20012222, // addi    at,zero,8738
            0x00412820, // add     a1,v0,at
            0x00413020, // add     a2,v0,at
            0x00000000, // nop
        };
        Registers regs_init;
        regs_init.pc_abs_jmp(0x80020000_addr);
        Registers regs_res(regs_init);
        regs_res.write_gp(1, 0x2222);
        regs_res.write_gp(2, 3);
        regs_res.write_gp(3, 0x2223);
        regs_res.write_gp(4, 0x2223);
        regs_res.write_gp(5, 0x2225);
        regs_res.write_gp(6, 0x2225);
        regs_res.pc_abs_jmp(regs_init.read_pc() + 4 * code.length());
        QTest::newRow("alu_forward_1") << code << regs_init << regs_res;
    }

    // Test forwarding in JR and JALR
    {
        QVector<uint32_t> code {
            // start: = 0x80020000
            0x3c041111, // lui     a0,0x1111
            0x3c052222, // lui     a1,0x2222
            0x0c008012, // jal     80020048 <fnc_add3>
            0x3c063333, // lui     a2,0x3333
            0x00021820, // add     v1,zero,v0
            0x0800800a, // j       80020028 <skip>
            0x20107777, // addi    s0,zero, 0x7777
            0x20128888, // addi    s2,zero, 0x8888
            0x20139999, // addi    s3,zero, 0x9999
            0x2014aaaa, // addi    s4,zero, 0xaaaa
            // skip:
            0x3c088002, // lui     t0,0x8002
            0x25080058, // addiu   t0,t0,88
            0x0100f809, // jalr    t0
            0x2004abcd, // addi    a0,zero, 0xabcd
            0x20040000, // addi    a0,zero,0
            0x20510000, // addi    s1,v0,0
            0x08008018, // j       80020060 <loop>
            0x00000000, // nop
            // fnc_add3:
            0x00851020, // add     v0,a0,a1
            0x00461020, // add     v0,v0,a2
            0x03e00008, // jr      ra
            0x00000000, // nop
            // fnc_short:
            0x03e00008, // jr      ra
            0x20820000, // addi    v0,a0,0
            // loop:
            0x1000ffff, // b       80020060 <loop>
            0x00000000, // nop
        };
        Registers regs_init;
        regs_init.pc_abs_jmp(0x80020000_addr);
        Registers regs_res(regs_init);
        regs_res.write_gp(1, 0x00000000);
        regs_res.write_gp(2, 0xffffabcd);
        regs_res.write_gp(3, 0x66660000);
        regs_res.write_gp(4, 0x00000000);
        regs_res.write_gp(5, 0x22220000);
        regs_res.write_gp(6, 0x33330000);
        regs_res.write_gp(7, 0x00000000);
        regs_res.write_gp(8, 0x80020058);
        regs_res.write_gp(16, 0x00007777);
        regs_res.write_gp(17, 0xffffabcd);
        regs_res.write_gp(31, 0x80020038);
        regs_res.pc_abs_jmp(0x80020060_addr);
        QTest::newRow("j_jal_jalr") << code << regs_init << regs_res;
    }

    // Test multiplication and division
    {
        QVector<uint32_t> code {
            // start:
            0x3c021234, // lui     v0,0x1234
            0x34425678, // ori     v0,v0,0x5678
            0x3c03abcd, // lui     v1,0xabcd
            0x3463ef01, // ori     v1,v1,0xef01
            0x00430018, // mult    v0,v1
            0x00008012, // mflo    s0
            0x00008810, // mfhi    s1
            0x00430019, // multu   v0,v1
            0x00009012, // mflo    s2
            0x00009810, // mfhi    s3
            0x0062001a, // div     zero,v1,v0
            0x0000a012, // mflo    s4
            0x0000a810, // mfhi    s5
            0x0062001b, // divu    zero,v1,v0
            0x0000b012, // mflo    s6
            0x0000b810, // mfhi    s7
            // loop:
            0x1000ffff, // b       80020070 <loop>
            0x00000000, // nop
        };
        Registers regs_init;
        regs_init.pc_abs_jmp(0x80020000_addr);
        Registers regs_res(regs_init);
        uint32_t val_a = 0x12345678;
        uint32_t val_b = 0xabcdef01;
        uint64_t val_u64;
        int64_t val_s64;
        regs_res.write_gp(2, val_a);
        regs_res.write_gp(3, val_b);
        val_s64 = (int64_t)(int32_t)val_a * (int32_t)val_b;
        regs_res.write_gp(16, (uint32_t)(val_s64 & 0xffffffff));
        regs_res.write_gp(17, (uint32_t)(val_s64 >> 32));
        val_u64 = (uint64_t)val_a * val_b;
        regs_res.write_gp(18, (uint32_t)(val_u64 & 0xffffffff));
        regs_res.write_gp(19, (uint32_t)(val_u64 >> 32));
        regs_res.write_gp(20, (uint32_t)((int32_t)val_b / (int32_t)val_a));
        regs_res.write_gp(21, (uint32_t)((int32_t)val_b % (int32_t)val_a));
        regs_res.write_gp(22, val_b / val_a);
        regs_res.write_gp(23, val_b % val_a);
        regs_res.write_hi_lo(false, regs_res.read_gp(22));
        regs_res.write_hi_lo(true, regs_res.read_gp(23));
        regs_res.pc_abs_jmp(regs_init.read_pc() + 4 * code.length() - 4);
        QTest::newRow("mul-div") << code << regs_init << regs_res;
    }

    // branches
    {
        QVector<uint32_t> code {
            // start:
            0x2001ffff, // addi    at,zero,-1
            0x20020001, // addi    v0,zero,1
            0x20030000, // addi    v1,zero,0
            0x20040002, // addi    a0,zero,2
            // test_branch:
            0x20050001, // addi    a1,zero,1
            0x04200004, // bltz    at,80020028 <test_branch+0x18>
            0x00000000, // nop
            0x20050000, // addi    a1,zero,0
            0x20631000, // addi    v1,v1,4096
            0x00611820, // add     v1,v1,at
            0x20050001, // addi    a1,zero,1
            0x18200004, // blez    at,80020040 <test_branch+0x30>
            0x00000000, // nop
            0x20050000, // addi    a1,zero,0
            0x20630100, // addi    v1,v1,256
            0x00611820, // add     v1,v1,at
            0x20050001, // addi    a1,zero,1
            0x04210004, // bgez    at,80020058 <test_branch+0x48>
            0x00000000, // nop
            0x20050000, // addi    a1,zero,0
            0x20632000, // addi    v1,v1,8192
            0x00611820, // add     v1,v1,at
            0x20050001, // addi    a1,zero,1
            0x1c200004, // bgtz    at,80020070 <test_branch+0x60>
            0x00000000, // nop
            0x20050000, // addi    a1,zero,0
            0x20630200, // addi    v1,v1,512
            0x00611820, // add     v1,v1,at
            0x20050001, // addi    a1,zero,1
            0x14220004, // bne     at,v0,80020088 <test_branch+0x78>
            0x00000000, // nop
            0x20050000, // addi    a1,zero,0
            0x20634000, // addi    v1,v1,16384
            0x00611820, // add     v1,v1,at
            0x20050001, // addi    a1,zero,1
            0x10220004, // beq     at,v0,800200a0 <test_branch+0x90>
            0x00000000, // nop
            0x20050000, // addi    a1,zero,0
            0x20630400, // addi    v1,v1,1024
            0x00611820, // add     v1,v1,at
            0x20210001, // addi    at,at,1
            0x1424ffda, // bne     at,a0,80020010 <test_branch>
            0x00000000, // nop
            // loop:
            0x1000ffff, // b       800200ac <loop>
            0x00000000, // nop
        };
        Registers regs_init;
        regs_init.pc_abs_jmp(0x80020000_addr);
        Registers regs_res(regs_init);
        regs_res.write_gp(1, 2);
        regs_res.write_gp(2, 1);
        regs_res.write_gp(3, 0x8d00);
        regs_res.write_gp(4, 2);
        regs_res.write_gp(5, 1);
        regs_res.pc_abs_jmp(regs_init.read_pc() + 4 * code.length());
        QTest::newRow("branch_conditions_test")
            << code << regs_init << regs_res;
    }
}

void MachineTests::singlecore_alu_forward_data() {
    core_alu_forward_data();
}

void MachineTests::pipecore_alu_forward_data() {
    core_alu_forward_data();
}

void MachineTests::pipecorestall_alu_forward_data() {
    core_alu_forward_data();
}

static void run_code_fragment(
    Core &core,
    Registers &reg_init,
    Registers &reg_res,
    Memory &mem_init,
    Memory &mem_res,
    QVector<uint32_t> &code) {
    uint64_t addr = reg_init.read_pc().get_raw();

    foreach (uint32_t i, code) {
        memory_write_u32(&mem_init, addr, i);
        memory_write_u32(&mem_res, addr, i);
        addr += 4;
    }

    for (int k = 10000; k; k--) {
        core.step(); // Single step should be enought as this is risc without
                     // pipeline
        if (reg_init.read_pc() == reg_res.read_pc() && k > 6) { // reached end
                                                                // of
                                                                // the code
                                                                // fragment
            k = 6; // add some cycles to finish processing
        }
    }
    reg_res.pc_abs_jmp(reg_init.read_pc()); // We do not compare result pc
    QCOMPARE(reg_init, reg_res); // After doing changes from initial state this
                                 // should be same state as in case of passed
                                 // expected result
    QCOMPARE(mem_init, mem_res); // There should be no change in memory
}

void MachineTests::singlecore_alu_forward() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    Memory mem_init(BIG);
    TrivialBus mem_init_frontend(&mem_init);
    Memory mem_res(BIG);
    TrivialBus mem_res_frontend(&mem_res);
    CoreSingle core(&reg_init, &mem_init_frontend, &mem_init_frontend, true);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}

void MachineTests::pipecore_alu_forward() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    Memory mem_init(BIG);
    TrivialBus mem_init_frontend(&mem_init);
    Memory mem_res(BIG);
    TrivialBus mem_res_frontend(&mem_res);
    CorePipelined core(
        &reg_init, &mem_init_frontend, &mem_init_frontend,
        MachineConfig::HU_STALL_FORWARD);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}

void MachineTests::pipecorestall_alu_forward() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    Memory mem_init(BIG);
    TrivialBus mem_init_frontend(&mem_init);
    Memory mem_res(BIG);
    TrivialBus mem_res_frontend(&mem_res);
    CorePipelined core(
        &reg_init, &mem_init_frontend, &mem_init_frontend,
        MachineConfig::HU_STALL);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}

/*======================================================================*/

static void core_memory_tests_data() {
    QTest::addColumn<QVector<uint32_t>>("code");
    QTest::addColumn<Registers>("reg_init");
    QTest::addColumn<Registers>("reg_res");
    QTest::addColumn<Memory>("mem_init");
    QTest::addColumn<Memory>("mem_res");

    // Test
    {
        QVector<uint32_t> code {
            // objdump -d to src: ^[^ \t]+[ \t]+([^ \t]+)[ \t]+([^ \t].*)$

            // _start:
            0x20100000, // addi    s0,zero,0
            0x2011003c, // addi    s1,zero,60
            0x00109020, // add     s2,zero,s0
            // main_loop:
            0x1211001d, // beq     s0,s1,80020084 <main_loop_end>
            0x00000000, // nop
            0x3c140000, // lui     s4,0x0
            0x0290a021, // addu    s4,s4,s0
            0x8e941000, // lw      s4,4096(s4)
            0x02009820, // add     s3,s0,zero
            0x02009020, // add     s2,s0,zero
            // inner_loop:
            0x1251000b, // beq     s2,s1,80020058 <inner_loop_end>
            0x00000000, // nop
            0x3c150000, // lui     s5,0x0
            0x02b2a821, // addu    s5,s5,s2
            0x8eb51000, // lw      s5,4096(s5)
            0x0295082a, // slt     at,s4,s5
            0x14200003, // bnez    at,80020050 <not_a_min>
            0x00000000, // nop
            0x22530000, // addi    s3,s2,0
            0x22b40000, // addi    s4,s5,0
            // not_a_min:
            0x0800800a, // j       80020028 <inner_loop>
            0x22520004, // addi    s2,s2,4
            // inner_loop_end:
            0x3c150000, // lui     s5,0x0
            0x02b0a821, // addu    s5,s5,s0
            0x8eb51000, // lw      s5,4096(s5)
            0x3c010000, // lui     at,0x0
            0x00300821, // addu    at,at,s0
            0xac341000, // sw      s4,4096(at)
            0x3c010000, // lui     at,0x0
            0x00330821, // addu    at,at,s3
            0xac351000, // sw      s5,4096(at)
            0x08008003, // j       8002000c <main_loop>
            0x22100004, // addi    s0,s0,4
            // main_loop_end:
            0x20080000, // addi    t0,zero,0
            0xbd090000, // cache   0x9,0(t0)
            // end_loop:
            0x08008023, // j       8002008c <end_loop>
            0x00000000, // nop
        };
        QVector<uint32_t> data_init { 5,  3, 4,  1, 15, 8, 9, 2,
                                      10, 6, 11, 1, 6,  9, 12 };
        QVector<uint32_t> data_res { 1, 1, 2, 3,  4,  5,  6, 6,
                                     8, 9, 9, 10, 11, 12, 15 };
        Registers regs_init;
        regs_init.pc_abs_jmp(0x80020000_addr);
        Registers regs_res(regs_init);
        regs_res.write_gp(1, 0x38);
        regs_res.write_gp(16, 0x3c);
        regs_res.write_gp(17, 0x3c);
        regs_res.write_gp(18, 0x3c);
        regs_res.write_gp(19, 0x38);
        regs_res.write_gp(20, 0xf);
        regs_res.write_gp(21, 0xf);
        regs_res.pc_abs_jmp(regs_init.read_pc() + 4 * code.length() - 4);
        uint32_t addr;
        Memory mem_init(BIG);
        addr = 0x1000;
        foreach (uint32_t i, data_init) {
            memory_write_u32(&mem_init, addr, i);
            addr += 4;
        }
        Memory mem_res(BIG);
        addr = 0x1000;
        foreach (uint32_t i, data_res) {
            memory_write_u32(&mem_res, addr, i);
            addr += 4;
        }
        QTest::newRow("cache_insert_sort")
            << code << regs_init << regs_res << mem_init << mem_res;
    }

    // lwr, lwl, swr, swl
    {
        QVector<uint32_t> code {
            // __start:
            0x3c1c8003, // lui     gp,0x8003
            0x279c90d0, // addiu   gp,gp,-28464
            // main:
            0x3c1aaabb, // lui     k0,0xaabb
            0x375accdd, // ori     k0,k0,0xccdd
            0x23420000, // addi    v0,k0,0
            0x23430000, // addi    v1,k0,0
            0x23440000, // addi    a0,k0,0
            0x23450000, // addi    a1,k0,0
            0x23460000, // addi    a2,k0,0
            0x23470000, // addi    a3,k0,0
            0x23480000, // addi    t0,k0,0
            0x23490000, // addi    t1,k0,0
            0x234a0000, // addi    t2,k0,0
            0x3c1b8002, // lui     k1,0x8002
            0x277b0100, // addiu   k1,k1,0x100
            0x9b620000, // lwr     v0,0(k1)
            0x9b630001, // lwr     v1,1(k1)
            0x9b640002, // lwr     a0,2(k1)
            0x9b650003, // lwr     a1,3(k1)
            0x8b660000, // lwl     a2,0(k1)
            0x8b670001, // lwl     a3,1(k1)
            0x8b680002, // lwl     t0,2(k1)
            0x8b690003, // lwl     t1,3(k1)
            0xbb7a0000, // swr     k0,0(k1)
            0xbb7a0005, // swr     k0,5(k1)
            0xbb7a000a, // swr     k0,10(k1)
            0xbb7a000f, // swr     k0,15(k1)
            0xab7a0014, // swl     k0,20(k1)
            0xab7a0019, // swl     k0,25(k1)
            0xab7a001e, // swl     k0,30(k1)
            0xab7a0023, // swl     k0,35(k1)
            0x8f6a0000, // lw      t2,0(k1)
            0x8f6b0004, // lw      t3,4(k1)
            0x8f6c0008, // lw      t4,8(k1)
            0x8f6d000c, // lw      t5,12(k1)
            0x8f6e0010, // lw      t6,16(k1)
            0x8f6f0014, // lw      t7,20(k1)
            0x8f700018, // lw      s0,24(k1)
            0x8f71001c, // lw      s1,28(k1)
            0x8f720020, // lw      s2,32(k1)
            0x8f730020, // lw      s3,32(k1)
            0xbd090000, // cache   0x9,0(t0)
            // loop:
            0x1000ffff, // b       800200a4 <loop>
            0x00000000, // nop
            // mem:
        };
        Registers regs_init;
        regs_init.pc_abs_jmp(0x80020000_addr);
        Registers regs_res(regs_init);

        regs_res.write_gp(2, 0xaabbcc01);
        regs_res.write_gp(3, 0xaabb0102);
        regs_res.write_gp(4, 0xaa010203);
        regs_res.write_gp(5, 0x01020304);
        regs_res.write_gp(6, 0x01020304);
        regs_res.write_gp(7, 0x020304dd);
        regs_res.write_gp(8, 0x0304ccdd);
        regs_res.write_gp(9, 0x04bbccdd);
        regs_res.write_gp(10, 0xdd020304);
        regs_res.write_gp(11, 0xccdd0708);
        regs_res.write_gp(12, 0xbbccdd0c);
        regs_res.write_gp(13, 0xaabbccdd);
        regs_res.write_gp(14, 0x11121314);
        regs_res.write_gp(15, 0xaabbccdd);
        regs_res.write_gp(16, 0x19aabbcc);
        regs_res.write_gp(17, 0x1d1eaabb);
        regs_res.write_gp(18, 0x212223aa);
        regs_res.write_gp(19, 0x212223aa);

        regs_res.write_gp(26, 0xaabbccdd);
        regs_res.write_gp(27, 0x80020100);
        regs_res.write_gp(28, 0x800290d0);

        uint32_t addr;
        Memory mem_init(BIG);
        addr = 0x80020100;
        QVector<uint32_t> data_init { 0x01020304, 0x05060708, 0x090a0b0c,
                                      0x0d0e0f00, 0x11121314, 0x15161718,
                                      0x191a1b1c, 0x1d1e1f10, 0x21222324,
                                      0x25262728, 0x292a2b2c, 0x2d2e2f20 };
        foreach (uint32_t i, data_init) {
            memory_write_u32(&mem_init, addr, i);
            addr += 4;
        }
        Memory mem_res(BIG);
        addr = 0x80020100;
        QVector<uint32_t> data_res { 0xdd020304, 0xccdd0708, 0xbbccdd0c,
                                     0xaabbccdd, 0x11121314, 0xaabbccdd,
                                     0x19aabbcc, 0x1d1eaabb, 0x212223aa,
                                     0x25262728, 0x292a2b2c, 0x2d2e2f20 };
        foreach (uint32_t i, data_res) {
            memory_write_u32(&mem_res, addr, i);
            addr += 4;
        }

        regs_res.pc_abs_jmp(regs_init.read_pc() + 4 * code.length() - 4);
        QTest::newRow("lwr_lrl_swr_swl")
            << code << regs_init << regs_res << mem_init << mem_res;
    }
}

void MachineTests::singlecore_memory_tests_data() {
    core_memory_tests_data();
}

void MachineTests::pipecore_nc_memory_tests_data() {
    core_memory_tests_data();
}

void MachineTests::pipecore_wt_na_memory_tests_data() {
    core_memory_tests_data();
}

void MachineTests::pipecore_wt_a_memory_tests_data() {
    core_memory_tests_data();
}

void MachineTests::pipecore_wb_memory_tests_data() {
    core_memory_tests_data();
}

void MachineTests::singlecore_memory_tests() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);
    TrivialBus mem_init_frontend(&mem_init);
    TrivialBus mem_res_frontend(&mem_res);
    CoreSingle core(&reg_init, &mem_init_frontend, &mem_init_frontend, true);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}

void MachineTests::pipecore_nc_memory_tests() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);
    TrivialBus mem_init_frontend(&mem_init);
    TrivialBus mem_res_frontend(&mem_res);
    CorePipelined core(
        &reg_init, &mem_init_frontend, &mem_init_frontend,
        MachineConfig::HU_STALL_FORWARD);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}

void MachineTests::pipecore_wt_na_memory_tests() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);
    TrivialBus mem_init_frontend(&mem_init);
    TrivialBus mem_res_frontend(&mem_res);
    CacheConfig cache_conf;
    cache_conf.set_enabled(true);
    cache_conf.set_set_count(2);     // Number of sets
    cache_conf.set_block_size(1);    // Number of blocks
    cache_conf.set_associativity(2); // Degree of associativity
    cache_conf.set_replacement_policy(CacheConfig::RP_LRU);
    cache_conf.set_write_policy(CacheConfig::WP_THROUGH_NOALLOC);

    Cache i_cache(&mem_init_frontend, &cache_conf);
    Cache d_cache(&mem_init_frontend, &cache_conf);
    CorePipelined core(
        &reg_init, &i_cache, &d_cache, MachineConfig::HU_STALL_FORWARD);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}

void MachineTests::pipecore_wt_a_memory_tests() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);
    TrivialBus mem_init_frontend(&mem_init);
    TrivialBus mem_res_frontend(&mem_res);
    CacheConfig cache_conf;
    cache_conf.set_enabled(true);
    cache_conf.set_set_count(2);     // Number of sets
    cache_conf.set_block_size(1);    // Number of blocks
    cache_conf.set_associativity(2); // Degree of associativity
    cache_conf.set_replacement_policy(CacheConfig::RP_LRU);
    cache_conf.set_write_policy(CacheConfig::WP_THROUGH_ALLOC);
    Cache i_cache(&mem_init_frontend, &cache_conf);
    Cache d_cache(&mem_init_frontend, &cache_conf);
    CorePipelined core(
        &reg_init, &i_cache, &d_cache, MachineConfig::HU_STALL_FORWARD);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}

void MachineTests::pipecore_wb_memory_tests() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);
    TrivialBus mem_init_frontend(&mem_init);
    TrivialBus mem_res_frontend(&mem_res);
    CacheConfig cache_conf;
    cache_conf.set_enabled(true);
    cache_conf.set_set_count(4);     // Number of sets
    cache_conf.set_block_size(2);    // Number of blocks
    cache_conf.set_associativity(2); // Degree of associativity
    cache_conf.set_replacement_policy(CacheConfig::RP_LRU);
    cache_conf.set_write_policy(CacheConfig::WP_BACK);
    Cache i_cache(&mem_init_frontend, &cache_conf);
    Cache d_cache(&mem_init_frontend, &cache_conf);
    CorePipelined core(
        &reg_init, &i_cache, &d_cache, MachineConfig::HU_STALL_FORWARD);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}