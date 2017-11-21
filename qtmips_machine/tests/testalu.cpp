#include "tst_machine.h"
#include "alu.h"
#include "qtmipsexception.h"

void MachineTests::alu_data() {
    QTest::addColumn<AluOp>("op");
    QTest::addColumn<std::uint32_t>("s");
    QTest::addColumn<std::uint32_t>("t");
    QTest::addColumn<std::uint8_t>("sa");
    QTest::addColumn<std::uint32_t>("res");

    // TODO SLL-SRAV
    QTest::newRow("ADD") << ALU_OP_ADD \
                         << (std::uint32_t)24 \
                         << (std::uint32_t)66 \
                         << (std::uint8_t)0 \
                         << (std::uint32_t)90;
    QTest::newRow("ADDU") << ALU_OP_ADDU \
                         << (std::uint32_t)24 \
                         << (std::uint32_t)66 \
                         << (std::uint8_t)0 \
                         << (std::uint32_t)90;
    QTest::newRow("SUB") << ALU_OP_SUB \
                         << (std::uint32_t)66 \
                         << (std::uint32_t)24 \
                         << (std::uint8_t)0 \
                         << (std::uint32_t)42;
    QTest::newRow("SUBU") << ALU_OP_SUBU \
                         << (std::uint32_t)24 \
                         << (std::uint32_t)66 \
                         << (std::uint8_t)0 \
                         << (std::uint32_t)-42;
    QTest::newRow("AND") << ALU_OP_AND \
                         << (std::uint32_t)0xA81 \
                         << (std::uint32_t)0x603 \
                         << (std::uint8_t)0 \
                         << (std::uint32_t)0x201;
    QTest::newRow("OR") << ALU_OP_OR \
                         << (std::uint32_t)0xA81 \
                         << (std::uint32_t)0x603 \
                         << (std::uint8_t)0 \
                         << (std::uint32_t)0xE83;
    QTest::newRow("XOR") << ALU_OP_XOR \
                         << (std::uint32_t)0xA81 \
                         << (std::uint32_t)0x603 \
                         << (std::uint8_t)0 \
                         << (std::uint32_t)0xC82;
    QTest::newRow("NOR") << ALU_OP_NOR \
                         << (std::uint32_t)0xA81 \
                         << (std::uint32_t)0x603 \
                         << (std::uint8_t)0 \
                         << (std::uint32_t)0xFFFFF17C;
    // TODO SLT-SLTU
}

void MachineTests::alu() {
    QFETCH(AluOp, op);
    QFETCH(std::uint32_t, s);
    QFETCH(std::uint32_t, t);
    QFETCH(std::uint8_t, sa);
    QFETCH(std::uint32_t, res);

    QCOMPARE(alu_operate(op, s , t, sa), res);
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
                         << (std::uint32_t)3 \
                         << (std::uint32_t)4;
    // Just test that we can throw unsupported ALU operation
    QTest::newRow("?") << (std::uint8_t)ALU_OP_LAST \
                         << (std::uint32_t)0 \
                         << (std::uint32_t)0;
}

void MachineTests::alu_except() {
    QFETCH(std::uint8_t, op);
    QFETCH(std::uint32_t, s);
    QFETCH(std::uint32_t, t);

    // Only runtime exception is expected as any other exception is a bug
    QVERIFY_EXCEPTION_THROWN(alu_operate((enum AluOp)op, s , t, 0), QtMipsExceptionRuntime);
}
