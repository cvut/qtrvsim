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

#include "tst_machine.h"
#include "alu.h"
#include "qtmipsexception.h"

using namespace machine;

void MachineTests::alu_data() {
    QTest::addColumn<AluOp>("op");
    QTest::addColumn<std::uint32_t>("s");
    QTest::addColumn<std::uint32_t>("t");
    QTest::addColumn<std::uint8_t>("sa");
    QTest::addColumn<Registers>("regs_init");
    QTest::addColumn<Registers>("regs_res");
    QTest::addColumn<std::uint32_t>("res");

    QTest::newRow("SLL") << ALU_OP_SLL \
                         << (std::uint32_t)0 \
                         << (std::uint32_t)0x80000001 \
                         << (std::uint8_t)3 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)0x8;
    QTest::newRow("SRL") << ALU_OP_SRL \
                         << (std::uint32_t)0 \
                         << (std::uint32_t)0x80000008 \
                         << (std::uint8_t)3 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)0x10000001;
    QTest::newRow("SRA") << ALU_OP_SRA \
                         << (std::uint32_t)0 \
                         << (std::uint32_t)0x80000008 \
                         << (std::uint8_t)3 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)0xF0000001;
    QTest::newRow("SLLV") << ALU_OP_SLLV \
                         << (std::uint32_t)3 \
                         << (std::uint32_t)0x80000001 \
                         << (std::uint8_t)0 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)0x8;
    QTest::newRow("SRLV") << ALU_OP_SRLV \
                         << (std::uint32_t)3 \
                         << (std::uint32_t)0x80000008 \
                         << (std::uint8_t)0 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)0x10000001;
    QTest::newRow("SRAV") << ALU_OP_SRAV \
                         << (std::uint32_t)3 \
                         << (std::uint32_t)0x80000008 \
                         << (std::uint8_t)0 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)0xF0000001;
    // JR and JALR should have no effect and we test that in core (it really doesn't make sense to test it here)
    QTest::newRow("MOVZ") << ALU_OP_MOVZ \
                         << (std::uint32_t)22 \
                         << (std::uint32_t)0 \
                         << (std::uint8_t)0 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)22;
    QTest::newRow("MOVN") << ALU_OP_MOVN \
                         << (std::uint32_t)22 \
                         << (std::uint32_t)1 \
                         << (std::uint8_t)0 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)22;
    {
    Registers init;
    init.write_hi_lo(true, 42);
    init.write_hi_lo(false, 24);
    Registers res(init);
    QTest::newRow("MFHI") << ALU_OP_MFHI \
                         << (std::uint32_t)0 \
                         << (std::uint32_t)0 \
                         << (std::uint8_t)0 \
                         << init \
                         << res \
                         << (std::uint32_t)42;
    QTest::newRow("MFLO") << ALU_OP_MFLO \
                         << (std::uint32_t)0 \
                         << (std::uint32_t)0 \
                         << (std::uint8_t)0 \
                         << init \
                         << res \
                         << (std::uint32_t)24;
    res.write_hi_lo(true, 43);
    QTest::newRow("MTHI") << ALU_OP_MTHI \
                         << (std::uint32_t)43 \
                         << (std::uint32_t)0 \
                         << (std::uint8_t)0 \
                         << init \
                         << res \
                         << (std::uint32_t)0;
    res.write_hi_lo(true, 42);
    res.write_hi_lo(false, 23);
    QTest::newRow("MTLO") << ALU_OP_MTLO \
                         << (std::uint32_t)23 \
                         << (std::uint32_t)0 \
                         << (std::uint8_t)0 \
                         << init \
                         << res \
                         << (std::uint32_t)0;
    }
    QTest::newRow("ADD") << ALU_OP_ADD \
                         << (std::uint32_t)24 \
                         << (std::uint32_t)66 \
                         << (std::uint8_t)0 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)90;
    QTest::newRow("ADDU") << ALU_OP_ADDU \
                         << (std::uint32_t)24 \
                         << (std::uint32_t)66 \
                         << (std::uint8_t)0 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)90;
    QTest::newRow("SUB") << ALU_OP_SUB \
                         << (std::uint32_t)66 \
                         << (std::uint32_t)24 \
                         << (std::uint8_t)0 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)42;
    QTest::newRow("SUBU") << ALU_OP_SUBU \
                         << (std::uint32_t)24 \
                         << (std::uint32_t)66 \
                         << (std::uint8_t)0 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)-42;
    QTest::newRow("AND") << ALU_OP_AND \
                         << (std::uint32_t)0xA81 \
                         << (std::uint32_t)0x603 \
                         << (std::uint8_t)0 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)0x201;
    QTest::newRow("OR") << ALU_OP_OR \
                         << (std::uint32_t)0xA81 \
                         << (std::uint32_t)0x603 \
                         << (std::uint8_t)0 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)0xE83;
    QTest::newRow("XOR") << ALU_OP_XOR \
                         << (std::uint32_t)0xA81 \
                         << (std::uint32_t)0x603 \
                         << (std::uint8_t)0 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)0xC82;
    QTest::newRow("NOR") << ALU_OP_NOR \
                         << (std::uint32_t)0xA81 \
                         << (std::uint32_t)0x603 \
                         << (std::uint8_t)0 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)0xFFFFF17C;
    QTest::newRow("SLT") << ALU_OP_SLT \
                         << (std::uint32_t)-31 \
                         << (std::uint32_t)24 \
                         << (std::uint8_t)0 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)1;
    QTest::newRow("SLTU") << ALU_OP_SLTU \
                         << (std::uint32_t)24 \
                         << (std::uint32_t)32 \
                         << (std::uint8_t)0 \
                         << Registers() \
                         << Registers() \
                         << (std::uint32_t)1;
}

void MachineTests::alu() {
    bool discard;
    QFETCH(AluOp, op);
    QFETCH(std::uint32_t, s);
    QFETCH(std::uint32_t, t);
    QFETCH(std::uint8_t, sa);
    QFETCH(Registers, regs_init);
    QFETCH(Registers, regs_res);
    QFETCH(std::uint32_t, res);

    QCOMPARE(alu_operate(op, s , t, sa, 0, &regs_init, discard), res);
    QCOMPARE(regs_res, regs_init);
}

void MachineTests::alu_except_data() {
    QTest::addColumn<std::uint8_t>("op");
    QTest::addColumn<std::uint32_t>("s");
    QTest::addColumn<std::uint32_t>("t");
    // Note no sa as shift unstruction has no exceptions

    QTest::newRow("ADD") << (std::uint8_t)ALU_OP_ADD \
                         << (std::uint32_t)0x8fffffff \
                         << (std::uint32_t)0x90000000;
    QTest::newRow("SUB") << (std::uint8_t)ALU_OP_SUB \
                         << (std::uint32_t)0x80000003 \
                         << (std::uint32_t)4;
    // Just test that we can throw unsupported ALU operation
    QTest::newRow("?") << (std::uint8_t)ALU_OP_LAST \
                         << (std::uint32_t)0 \
                         << (std::uint32_t)0;
}

void MachineTests::alu_except() {
    bool discard;
    QFETCH(std::uint8_t, op);
    QFETCH(std::uint32_t, s);
    QFETCH(std::uint32_t, t);
    Registers regs;

    // Only runtime exception is expected as any other exception is a bug
    QVERIFY_EXCEPTION_THROWN(alu_operate((enum AluOp)op, s , t, 0, 0, &regs, discard), QtMipsExceptionRuntime);
}
