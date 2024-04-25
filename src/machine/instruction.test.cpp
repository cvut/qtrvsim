#include "instruction.test.h"

#include "instruction.h"

using namespace machine;

// Test that we are correctly encoding instructions in constructor
void TestInstruction::instruction() {
    QCOMPARE(Instruction(0x0), Instruction());
}

// Test that we are correctly decoding instruction fields
void TestInstruction::instruction_access() {
    Instruction i(0xffffffff);

    QCOMPARE(i.data(), (uint32_t)0xffffffff);
    QCOMPARE(i.opcode(), (uint8_t)0x7f);
    QCOMPARE(i.rs(), (uint8_t)0x1f);
    QCOMPARE(i.rt(), (uint8_t)0x1f);
    QCOMPARE(i.rd(), (uint8_t)0x1f);
    QCOMPARE(i.shamt(), (uint8_t)0x1f);
    QCOMPARE(i.funct(), (uint16_t)0x3ff);
    QCOMPARE(i.immediate(), (int32_t)0);
    QCOMPARE(i.address().get_raw(), (uint64_t)0x3ffffff);
}

static struct { uint32_t code; QString str; } code_to_string[] = {
    {0xffffffff, "unknown"},
    {0x0, "unknown"},
    {0b00000000000000000000000000010011, "nop"},
    {0b00000000000000001000000010010011, "addi x1, x1, 0"},
    {0b01111111111111111000111110010011, "addi x31, x31, 2047"},
    {0b11111111111100001000000010010011, "addi x1, x1, -1"},
    {0b10000000000000001000000010010011, "addi x1, x1, -2048"},
    #include<./instruction.test.data.h>
};

void TestInstruction::instruction_to_str() {
    size_t array_length = sizeof(code_to_string) / sizeof(code_to_string[0]);
    for (size_t i = 0; i < array_length; ++i) {
        QCOMPARE(Instruction(code_to_string[i].code).to_str(), code_to_string[i].str);
    }
}

void TestInstruction::instruction_code_from_str() {
    size_t array_length = sizeof(code_to_string) / sizeof(code_to_string[0]);
    for (size_t i = 0; i < array_length; ++i) {
        if (code_to_string[i].str == "unknown") { continue; }
        uint32_t code = 0;
        Instruction::code_from_string(&code, code_to_string[i].str.length(), code_to_string[i].str, machine::Address(0x0));
        QCOMPARE(code, code_to_string[i].code);
    }
}

QTEST_APPLESS_MAIN(TestInstruction)
