#include "tst_machine.h"
#include "instructions/arithmetic.h"
#include "instructions/jumpbranch.h"
#include "instructions/loadstore.h"
#include "instructions/nop.h"
#include "instructions/shift.h"
#include <iostream>

void MachineTests::instruction_arithmetic_data() {
    QTest::addColumn<size_t>("type");
    QTest::addColumn<std::uint32_t>("res");

    QTest::newRow("ADD") << (size_t)IAT_ADD << (unsigned)749;
    QTest::newRow("ADDU") << (size_t)IAT_ADDU << (unsigned)749;
    QTest::newRow("SUB") << (size_t)IAT_SUB << (unsigned)-665;
    QTest::newRow("SUBU") << (size_t)IAT_SUBU << (unsigned)-665;
    QTest::newRow("AND") << (size_t)IAT_AND << (unsigned)2;
    QTest::newRow("OR") << (size_t)IAT_OR << (unsigned)747;
    QTest::newRow("XOR") << (size_t)IAT_XOR << (unsigned)745;
    // TODO others
}

void MachineTests::instruction_arithmetic() {
    Registers regs;

    QFETCH(size_t, type);
    QFETCH(std::uint32_t, res);

    // TODO meaby one more dataset?

    regs.write_gp(12, 42);
    regs.write_gp(8, 707);
    regs.write_gp(5, 0);

    Instruction *i = new InstructionArithmetic((enum InstructionArithmeticT)type, 12, 8, 5);

    i->decode(&regs);
    i->execute();
    i->memory(nullptr); // We should not work with memory so segfault here is basically a test
    i->write_back(&regs);

    QCOMPARE(regs.read_gp(5), (std::uint32_t)res);
}

void MachineTests::instruction_arithmetic_immediate_data() {
    QTest::addColumn<size_t>("type");
    QTest::addColumn<std::uint32_t>("res");

    QTest::newRow("ADDI") << (size_t)IAT_ADDI << (unsigned)749;
    QTest::newRow("ADDIU") << (size_t)IAT_ADDIU << (unsigned)749;
    QTest::newRow("ANDI") << (size_t)IAT_ANDI << (unsigned)-665;
    QTest::newRow("ORI") << (size_t)IAT_ORI << (unsigned)-665;
    QTest::newRow("XORI") << (size_t)IAT_XORI << (unsigned)2;
    // TODO others
}

void MachineTests::instruction_arithmetic_immediate() {
    Registers regs;

    QFETCH(size_t, type);
    QFETCH(std::uint32_t, res);

    // TODO meaby one more dataset?

    regs.write_gp(9, 42);
    regs.write_gp(3, 0);

    Instruction *i = new InstructionArithmeticImmediate((enum InstructionArithmeticImmediateT)type, 9, 3, 707);

    i->decode(&regs);
    i->execute();
    i->memory(nullptr); // We should not work with memory so segfault here is basically a test
    i->write_back(&regs);

    QCOMPARE(regs.read_gp(3), (std::uint32_t)res);
}
