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

static const struct {
    uint32_t code;
    QString str;
    QString alias_str = nullptr;
} code_to_string[] = {
    { 0xffffffff, "unknown" },
    { 0x0, "unknown" },
    { 0b00000000000000000000000000010011, "nop" },
    { 0b00000000000000001000000010010011, "addi x1, x1, 0" },
    { 0b01111111111111111000111110010011, "addi x31, x31, 2047" },
    { 0b11111111111100001000000010010011, "addi x1, x1, -1" },
    { 0b10000000000000001000000010010011, "addi x1, x1, -2048" },
#include <./instruction.test.data.def>
};

struct Pair {
    uint32_t code;
    QString str;
};
static const struct {
    QString string_data;
    std::vector<Pair> pairs;
} pesude_code_to_string[] = {
    { "nop", { { 0b00000000000000000000000000010011, "nop" } } },
    { "la x1, 0xffffefff",
      { { 0xfffff097, "auipc x1, 0xfffff" }, { 0xfff08093, "addi x1, x1, -1" } } },
    { "li x1, 0xffffefff",
      { { 0xfffff0b7, "lui x1, 0xfffff" }, { 0xfff08093, "addi x1, x1, -1" } } },
    { "call 0xfffeffff",
      { { 0xffff0317, "auipc x6, 0xffff0" }, { 0xfff300e7, "jalr x1, -1(x6)" } } },
    { "tail 0xfffeffff",
      { { 0xffff0317, "auipc x6, 0xffff0" }, { 0xfff30067, "jalr x0, -1(x6)" } } },
    { "sext.b x1, x2", { { 0x11093, "slli x1, x2, 0x0" }, { 0x4000d093, "srai x1, x1, 0x0" } } },
    { "sext.h x1, x2", { { 0x11093, "slli x1, x2, 0x0" }, { 0x4000d093, "srai x1, x1, 0x0" } } },
    { "zext.h x1, x2", { { 0x11093, "slli x1, x2, 0x0" }, { 0xd093, "srli x1, x1, 0x0" } } },
    { "zext.w x1, x2", { { 0x11093, "slli x1, x2, 0x0" }, { 0xd093, "srli x1, x1, 0x0" } } },
};

void TestInstruction::instruction_to_str() {
    for (size_t i = 0; i < sizeof(code_to_string) / sizeof(code_to_string[0]); i++) {
        QCOMPARE(Instruction(code_to_string[i].code).to_str(), code_to_string[i].str);
    }

    for (size_t i = 0; i < sizeof(pesude_code_to_string) / sizeof(pesude_code_to_string[0]); i++) {
        for (size_t j = 0; j < pesude_code_to_string[i].pairs.size(); j++) {
            Pair pair = pesude_code_to_string[i].pairs[j];
            QCOMPARE(Instruction(pair.code).to_str(), pair.str);
        }
    }
}

void TestInstruction::instruction_code_from_str() {
    for (size_t i = 0; i < sizeof(code_to_string) / sizeof(code_to_string[0]); i++) {
        if (code_to_string[i].str == "unknown") { continue; }
        QString test_string_data = code_to_string[i].alias_str == nullptr
                                       ? code_to_string[i].str
                                       : code_to_string[i].alias_str;
        uint32_t code = 0;
        try {
            Instruction::code_from_string(
                &code, code_to_string[i].str.length(), test_string_data, Address(0x0));
            QCOMPARE(code, code_to_string[i].code);
        } catch (const Instruction::ParseError &e) {
            TokenizedInstruction inst
                = TokenizedInstruction::from_line(test_string_data, Address(0x0), nullptr, 0);
            QFAIL(qPrintable(e.message));
        } catch (...) { QFAIL("code_from_string throw unexpected exception"); }
    }
}

QTEST_APPLESS_MAIN(TestInstruction)
