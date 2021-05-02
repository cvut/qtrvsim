#include "alu.h"

#include "alu.test.h"
#include "common/polyfills/mulh64.h"

#include <array>
#include <tuple>

using namespace machine;

void TestAlu::test_alu64_operate_data() {
    QTest::addColumn<AluOp>("op");
    QTest::addColumn<bool>("modified");
    QTest::addColumn<int64_t>("operand_a");
    QTest::addColumn<int64_t>("operand_b");
    QTest::addColumn<int64_t>("result");

    QTest::addRow("ADD") << AluOp::ADD << false << int64_t(0xFFFFFFFFFFFFFFFFULL) << int64_t(1)
                         << int64_t(0);
    QTest::addRow("ADD") << AluOp::ADD << false << int64_t(123) << int64_t(123000)
                         << int64_t(123123);
    QTest::addRow("SUB") << AluOp::ADD << true << int64_t(123123) << int64_t(123000)
                         << int64_t(123);
    QTest::addRow("SLT") << AluOp::SLT << false << int64_t(123123) << int64_t(123000) << int64_t(0);
    QTest::addRow("SLT") << AluOp::SLT << false << int64_t(-123123) << int64_t(123000)
                         << int64_t(1);
    QTest::addRow("SLTU") << AluOp::SLTU << false << int64_t(-123123) << int64_t(123000)
                          << int64_t(0);
    QTest::addRow("SLTU") << AluOp::SLTU << false << int64_t(123123) << int64_t(123000000)
                          << int64_t(1);

    QTest::addRow("XOR") << AluOp::XOR << false << int64_t(0xFFFFFFFF00000000ULL)
                         << int64_t(0xFFFFFFFFFFFFFFFFULL) << int64_t(0x00000000FFFFFFFFULL);
    QTest::addRow("OR") << AluOp::OR << false << int64_t(0xFFFFFFFF00000000ULL)
                        << int64_t(0xFFFFFFFFFFFFFFFFULL) << int64_t(0xFFFFFFFFFFFFFFFFULL);
    QTest::addRow("AND") << AluOp::AND << false << int64_t(0xFFFFFFFF00000000ULL)
                         << int64_t(0xFFFFFFFFFFFFFFFFULL) << int64_t(0xFFFFFFFF00000000ULL);
    QTest::addRow("SRL") << AluOp::SR << false << int64_t(0xFFFFFFFF00000000ULL)
                         << int64_t(0xFFFFFFFF00000010ULL) << int64_t(0x0000FFFFFFFF0000ULL);
    QTest::addRow("SRA") << AluOp::SR << true << int64_t(0xFFFFFFFF00000000ULL)
                         << int64_t(0xFFFFFFFF00000010ULL) << int64_t(0xFFFFFFFFFFFF0000ULL);
}

void TestAlu::test_alu64_operate() {
    QFETCH(AluOp, op);
    QFETCH(bool, modified);
    QFETCH(int64_t, operand_a);
    QFETCH(int64_t, operand_b);
    QFETCH(int64_t, result);

    // Test unit itself.
    QCOMPARE(alu64_operate(op, modified, operand_a, operand_b), result);
    // Test that combined wrapper does not break anything.
    QCOMPARE(
        alu_combined_operate(
            { .alu_op = op }, AluComponent::ALU, false, modified, operand_a, operand_b),
        RegisterValue(result));
}

void TestAlu::test_alu32_operate_data() {
    QTest::addColumn<AluOp>("op");
    QTest::addColumn<bool>("modified");
    QTest::addColumn<int32_t>("operand_a");
    QTest::addColumn<int32_t>("operand_b");
    QTest::addColumn<int32_t>("result");

    QTest::addRow("ADD") << AluOp::ADD << false << int32_t(0xFFFFFFFF) << int32_t(1) << int32_t(0);
    QTest::addRow("ADD") << AluOp::ADD << false << int32_t(123) << int32_t(123000)
                         << int32_t(123123);
    QTest::addRow("SUB") << AluOp::ADD << true << int32_t(123123) << int32_t(123000)
                         << int32_t(123);
    QTest::addRow("SLT") << AluOp::SLT << false << int32_t(123123) << int32_t(123000) << int32_t(0);
    QTest::addRow("SLT") << AluOp::SLT << false << int32_t(-123123) << int32_t(123000)
                         << int32_t(1);
    QTest::addRow("SLTU") << AluOp::SLTU << false << int32_t(-123123) << int32_t(123000)
                          << int32_t(0);
    QTest::addRow("SLTU") << AluOp::SLTU << false << int32_t(123123) << int32_t(123000000)
                          << int32_t(1);
    QTest::addRow("XOR") << AluOp::XOR << false << int32_t(0xFFFF0000) << int32_t(0xFFFFFFFF)
                         << int32_t(0x0000FFFF);
    QTest::addRow("OR") << AluOp::OR << false << int32_t(0xFFFF0000) << int32_t(0xFFFFFFFF)
                        << int32_t(0xFFFFFFFF);
    QTest::addRow("AND") << AluOp::AND << false << int32_t(0xFFFF0000) << int32_t(0xFFFFFFFF)
                         << int32_t(0xFFFF0000);
    QTest::addRow("SRL") << AluOp::SR << false << int32_t(0xFFFF0000) << int32_t(0xFFFF0010)
                         << int32_t(0x0000FFFF);
    QTest::addRow("SRA") << AluOp::SR << true << int32_t(0xFFFF0000) << int32_t(0xFFFF0010)
                         << int32_t(0xFFFFFFFF);
}

void TestAlu::test_alu32_operate() {
    QFETCH(AluOp, op);
    QFETCH(bool, modified);
    QFETCH(int32_t, operand_a);
    QFETCH(int32_t, operand_b);
    QFETCH(int32_t, result);

    // Test unit itself.
    QCOMPARE(alu32_operate(op, modified, operand_a, operand_b), result);
    // Test that combined wrapper does not break anything.
    QCOMPARE(
        alu_combined_operate(
            { .alu_op = op }, AluComponent::ALU, true, modified, operand_a, operand_b),
        RegisterValue(result));
}

// TODO evaluate the results and inline as literals.
constexpr std::array<std::tuple<int64_t, int64_t>, 6> inputs = { {
    { 1, 1 },
    { 2, 1 },
    { 2, 2 },
    { 123456789, 666 },
    { 123456789, -7777 },
    { -1, 8888 },
} };

void TestAlu::test_mul64_operate_data() {
    QTest::addColumn<MulOp>("op");
    QTest::addColumn<int64_t>("operand_a");
    QTest::addColumn<int64_t>("operand_b");
    QTest::addColumn<int64_t>("result");

    for (auto input : inputs) {
        int64_t a = std::get<0>(input);
        int64_t b = std::get<1>(input);

        QTest::addRow("MUL") << MulOp::MUL << a << b << a * b;
        QTest::addRow("MULH") << MulOp::MULH << a << b << int64_t(mulh64(a, b));
        QTest::addRow("MULHU") << MulOp::MULHU << a << b << int64_t(mulhu64(a, b));
        QTest::addRow("MULHSU") << MulOp::MULHSU << a << b << int64_t(mulhsu64(a, b));
        QTest::addRow("DIV") << MulOp::DIV << a << b << a / b;
        QTest::addRow("DIVU") << MulOp::DIVU << a << b << int64_t(uint64_t(a) / uint64_t(b));
        QTest::addRow("REM") << MulOp::REM << a << b << (a % b);
        QTest::addRow("REMU") << MulOp::REMU << a << b << int64_t(uint64_t(a) % uint64_t(b));
    }

    // Defined edge cases for division

    QTest::addRow("division by zero signed")
        << MulOp::DIV << int64_t(42) << int64_t(0) << int64_t(-1);
    QTest::addRow("division by zero unsigned")
        << MulOp::DIVU << int64_t(128) << int64_t(0) << int64_t(UINT64_MAX);
    QTest::addRow("division by zero reminder signed")
        << MulOp::REM << int64_t(666) << int64_t(0) << int64_t(666);
    QTest::addRow("division by zero reminder unsigned")
        << MulOp::REMU << int64_t(777) << int64_t(0) << int64_t(777);
    QTest::addRow("zero division by zero signed")
        << MulOp::DIV << int64_t(0) << int64_t(0) << int64_t(-1);
    QTest::addRow("zero division by zero unsigned")
        << MulOp::DIVU << int64_t(0) << int64_t(0) << int64_t(UINT64_MAX);
    QTest::addRow("zero division by zero reminder signed")
        << MulOp::REM << int64_t(0) << int64_t(0) << (int64_t)0;
    QTest::addRow("zero division by zero reminder unsigned")
        << MulOp::REMU << int64_t(0) << int64_t(0) << (int64_t)0;
    QTest::addRow("division overflow")
        << MulOp::DIV << int64_t(INT64_MIN) << int64_t(-1) << INT64_MIN;
    QTest::addRow("division reminder overflow")
        << MulOp::REM << int64_t(INT64_MAX) << int64_t(-1) << int64_t(0);
}

void TestAlu::test_mul64_operate() {
    QFETCH(MulOp, op);
    QFETCH(int64_t, operand_a);
    QFETCH(int64_t, operand_b);
    QFETCH(int64_t, result);

    // Test unit itself.
    QCOMPARE(mul64_operate(op, operand_a, operand_b), result);
    // Test that combined wrapper does not break anything.
    QCOMPARE(
        alu_combined_operate(
            (AluCombinedOp) { .mul_op = op }, AluComponent::MUL, false, false, operand_a,
            operand_b),
        RegisterValue(result));
}

/**
 * Helper function for upper bits of 32 bit multiplication.
 * Sign extension is handled on caller side.
 */
constexpr int32_t mulh32(uint64_t a, uint64_t b) {
    return (a * b) >> 32;
}

void TestAlu::test_mul32_operate_data() {
    QTest::addColumn<MulOp>("op");
    QTest::addColumn<int32_t>("operand_a");
    QTest::addColumn<int32_t>("operand_b");
    QTest::addColumn<int32_t>("result");

    for (auto input : inputs) {
        int32_t a = std::get<0>(input);
        int32_t b = std::get<1>(input);

        QTest::addRow("MUL") << MulOp::MUL << a << b << a * b;
        QTest::addRow("MULH") << MulOp::MULH << a << b << int32_t(mulh32(a, b));
        QTest::addRow("MULHU") << MulOp::MULHU << a << b
                               << int32_t(mulh32(uint32_t(a), uint32_t(b)));
        QTest::addRow("MULHSU") << MulOp::MULHSU << a << b << int32_t(mulh32(a, uint32_t(b)));
        QTest::addRow("DIV") << MulOp::DIV << a << b << a / b;
        QTest::addRow("DIVU") << MulOp::DIVU << a << b << int32_t(uint32_t(a) / uint32_t(b));
        QTest::addRow("REM") << MulOp::REM << a << b << (a % b);
        QTest::addRow("REMU") << MulOp::REMU << a << b << int32_t(uint32_t(a) % uint32_t(b));
    }

    // Defined edge cases for division

    QTest::addRow("division by zero signed")
        << MulOp::DIV << int32_t(42) << int32_t(0) << int32_t(-1);
    QTest::addRow("division by zero unsigned")
        << MulOp::DIVU << int32_t(128) << int32_t(0) << int32_t(UINT32_MAX);
    QTest::addRow("division by zero reminder signed")
        << MulOp::REM << int32_t(666) << int32_t(0) << int32_t(666);
    QTest::addRow("division by zero reminder unsigned")
        << MulOp::REMU << int32_t(777) << int32_t(0) << int32_t(777);
    QTest::addRow("zero division by zero signed")
        << MulOp::DIV << int32_t(0) << int32_t(0) << int32_t(-1);
    QTest::addRow("zero division by zero unsigned")
        << MulOp::DIVU << int32_t(0) << int32_t(0) << int32_t(UINT32_MAX);
    QTest::addRow("zero division by zero reminder signed")
        << MulOp::REM << int32_t(0) << int32_t(0) << (int32_t)0;
    QTest::addRow("zero division by zero reminder unsigned")
        << MulOp::REMU << int32_t(0) << int32_t(0) << (int32_t)0;
    QTest::addRow("division overflow")
        << MulOp::DIV << int32_t(INT32_MIN) << int32_t(-1) << INT32_MIN;
    QTest::addRow("division reminder overflow")
        << MulOp::REM << int32_t(INT32_MAX) << int32_t(-1) << int32_t(0);
}
void TestAlu::test_mul32_operate() {
    QFETCH(MulOp, op);
    QFETCH(int32_t, operand_a);
    QFETCH(int32_t, operand_b);
    QFETCH(int32_t, result);

    // Test unit itself.
    QCOMPARE(mul32_operate(op, operand_a, operand_b), result);
    // Test that combined wrapper does not break anything.
    QCOMPARE(
        alu_combined_operate(
            (AluCombinedOp) { .mul_op = op }, AluComponent::MUL, true, false, operand_a, operand_b),
        RegisterValue(result));
}

QTEST_APPLESS_MAIN(TestAlu)