#include "instruction.test.h"

#include "instruction.h"

using namespace machine;

// Test that we are correctly encoding instructions in constructor
void TestInstruction::instruction() {
    QCOMPARE(Instruction(0x0), Instruction());
    //    QCOMPARE(Instruction(0x4432146), Instruction(1, 2, 3, 4, 5, 6));
    //    QCOMPARE(Instruction(0x4430004), Instruction(1, 2, 3, 4));
    //    QCOMPARE(Instruction(0x4000002), Instruction(1, 2_addr));
}

// Test that we are correctly decoding instruction fields
void TestInstruction::instruction_access() {
    Instruction i(0xffffffff);

    QCOMPARE(i.data(), (uint32_t)0xffffffff);
    QCOMPARE(i.opcode(), (uint8_t)0x3f);
    QCOMPARE(i.rs(), (uint8_t)0x1f);
    QCOMPARE(i.rt(), (uint8_t)0x1f);
    QCOMPARE(i.rd(), (uint8_t)0x1f);
    QCOMPARE(i.shamt(), (uint8_t)0x1f);
    QCOMPARE(i.funct(), (uint16_t)0x3f);
    QCOMPARE(i.immediate(), (int32_t)0xffff);
    QCOMPARE(i.address().get_raw(), (uint64_t)0x3ffffff);
}

// TODO test to_str

QTEST_APPLESS_MAIN(TestInstruction)
