#include "tst_machine.h"
#include "instruction.h"

using namespace machine;

// Test that we are correctly encoding instructions in constructor
void MachineTests::instruction() {
    QCOMPARE(Instruction(0x0), Instruction());
    QCOMPARE(Instruction(0x4432146), Instruction(1, 2, 3, 4, 5, 6));
    QCOMPARE(Instruction(0x4430004), Instruction(1, 2, 3, 4));
    QCOMPARE(Instruction(0x4000002), Instruction(1, 2));
}

// Test that we are correctly decoding instruction fields
void MachineTests::instruction_access() {
    Instruction i(0xffffffff);

    QCOMPARE(i.data(), (std::uint32_t) 0xffffffff);
    QCOMPARE(i.opcode(), (std::uint8_t) 0x3f);
    QCOMPARE(i.rs(), (std::uint8_t) 0x1f);
    QCOMPARE(i.rt(), (std::uint8_t) 0x1f);
    QCOMPARE(i.rd(), (std::uint8_t) 0x1f);
    QCOMPARE(i.shamt(), (std::uint8_t) 0x1f);
    QCOMPARE(i.funct(), (std::uint8_t) 0x3f);
    QCOMPARE(i.immediate(), (std::uint16_t) 0xffff);
    QCOMPARE(i.address(), (std::uint32_t) 0x3ffffff);
}

// TODO test to_str
