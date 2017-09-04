#include "tst_machine.h"
#include "programmemory.h"
#include <qstring.h>
#include <qvector.h>
#include <iostream>

QVector<QString> str_inst_r(const char *inst, const char *rs, const char *rd, const char *rt, const char *sa) {
    QVector<QString> ret;
    ret << inst;
    if (rs)
        ret << rs;
    if (rd)
        ret << rd;
    if (rt)
        ret << rt;
    if (sa)
        ret << sa;
    return ret;
}

QVector<QString> str_inst_i(const char *inst, const char *rs, const char *rt, const char *immediate) {
    QVector<QString> ret;
    ret << inst;
    if (rs)
        ret << rs;
    if (rt)
        ret << rt;
    ret << immediate;
    return ret;
}

QVector<QString> str_inst_j(const char *inst, const char *address) {
    QVector<QString> ret;
    ret << inst;
    ret << address;
    return ret;
}

#define I(II) ((std::uint32_t) II)
#define I_R(OP, RS, RD, RT, SA, F) ((std::uint32_t)(((OP) << 26) | ((RS) << 21) | ((RT) << 16) | ((RD) << 11) | ((SA) << 6) | (F)))
#define I_I(OP, RS, RT, IMM) ((std::uint32_t)(((OP) << 26) | ((RS) << 21) | ((RT) << 16) | (IMM)))
#define I_J(OP, ADDR) ((std::uint32_t)(((OP) << 26) | (ADDR)))

void MachineTests::program_memory_data() {
    QTest::addColumn<std::uint32_t>("bin");
    QTest::addColumn<QVector<QString>>("str");

    /* NOP */
    QTest::newRow("NOP") << I(0) << str_inst_r("nop", nullptr, nullptr, nullptr, nullptr);
    /* ALU */
    QTest::newRow("ADD") << I_R(0, 12, 14, 13, 0, 32) << str_inst_r("add", "12", "13", "14", nullptr);
    QTest::newRow("ADDU") << I_R(0, 12, 14, 13, 0, 33) << str_inst_r("addu", "12", "13", "14", nullptr);
    QTest::newRow("SUB") << I_R(0, 12, 14, 13, 0, 34) << str_inst_r("sub", "12", "13", "14", nullptr);
    QTest::newRow("SUBU") << I_R(0, 12, 14, 13, 0, 35) << str_inst_r("subu", "12", "13", "14", nullptr);
    QTest::newRow("AND") << I_R(0, 12, 14, 13, 0, 36) << str_inst_r("and", "12", "13", "14", nullptr);
    QTest::newRow("OR") << I_R(0, 12, 14, 13, 0, 37) << str_inst_r("or", "12", "13", "14", nullptr);
    QTest::newRow("XOR") << I_R(0, 12, 14, 13, 0, 38) << str_inst_r("xor", "12", "13", "14", nullptr);
    QTest::newRow("NOR") << I_R(0, 12, 14, 13, 0, 39) << str_inst_r("nor", "12", "13", "14", nullptr);
    // TODO missing
    QTest::newRow("SLT") << I_R(0, 12, 14, 13, 0, 42) << str_inst_r("slt", "12", "13", "14", nullptr);
    QTest::newRow("SLTU") << I_R(0, 12, 14, 13, 0, 43) << str_inst_r("sltu", "12", "13", "14", nullptr);
    QTest::newRow("ADDI") << I_I(8, 12, 13, 42) << str_inst_i("addi", "12", "13", "2a");
    QTest::newRow("ADDIU") << I_I(9, 12, 13, 42) << str_inst_i("addiu", "12", "13", "2a");
    QTest::newRow("SLTI") << I_I(10, 12, 13, 42) << str_inst_i("slti", "12", "13", "2a");
    QTest::newRow("SLTIU") << I_I(11, 12, 13, 42) << str_inst_i("sltiu", "12", "13", "2a");
    QTest::newRow("ANDI") << I_I(12, 12, 13, 42) << str_inst_i("andi", "12", "13", "2a");
    QTest::newRow("ORI") << I_I(13, 12, 13, 42) << str_inst_i("ori", "12", "13", "2a");
    QTest::newRow("XORI") << I_I(14, 12, 13, 42) << str_inst_i("xori", "12", "13", "2a");
    QTest::newRow("LUI") << I_I(15, 12, 13, 42) << str_inst_i("lui", "12", "13", "2a");
    /* Load and store */
    QTest::newRow("LB") << I_I(32, 12, 13, 42) << str_inst_i("lb", "12", "13", "2a");
    QTest::newRow("LH") << I_I(33, 12, 13, 42) << str_inst_i("lh", "12", "13", "2a");
    QTest::newRow("LWL") << I_I(34, 12, 13, 42) << str_inst_i("lwl", "12", "13", "2a");
    QTest::newRow("LW") << I_I(35, 12, 13, 42) << str_inst_i("lw", "12", "13", "2a");
    QTest::newRow("LBU") << I_I(36, 12, 13, 42) << str_inst_i("lbu", "12", "13", "2a");
    QTest::newRow("LHU") << I_I(37, 12, 13, 42) << str_inst_i("lhu", "12", "13", "2a");
    QTest::newRow("LWR") << I_I(38, 12, 13, 42) << str_inst_i("lwr", "12", "13", "2a");
    // TODO missing
    QTest::newRow("SB") << I_I(40, 12, 13, 42) << str_inst_i("sb", "12", "13", "2a");
    QTest::newRow("SH") << I_I(41, 12, 13, 42) << str_inst_i("sh", "12", "13", "2a");
    QTest::newRow("SWL") << I_I(42, 12, 13, 42) << str_inst_i("swl", "12", "13", "2a");
    QTest::newRow("SW") << I_I(43, 12, 13, 42) << str_inst_i("sw", "12", "13", "2a");
    // TODO missing
    QTest::newRow("SWR") << I_I(46, 12, 13, 42) << str_inst_i("swr", "12", "13", "2a");
    /* Shifts */
    QTest::newRow("SLL") << I_R(0, 0, 13, 14, 12, 0) << str_inst_r("sll", nullptr, "13", "14", "12");
    // TODO missing
    QTest::newRow("SRL") << I_R(0, 0, 13, 14, 12, 2) << str_inst_r("srl", nullptr, "13", "14", "12");
    QTest::newRow("SRA") << I_R(0, 0, 13, 14, 12, 3) << str_inst_r("sra", nullptr, "13", "14", "12");
    QTest::newRow("SLLV") << I_R(0, 12, 13, 14, 0, 4) << str_inst_r("sllv", "12", "13", "14", nullptr);
    // TODO missing
    QTest::newRow("SRLV") << I_R(0, 12, 13, 14, 0, 6) << str_inst_r("srlv", "12", "13", "14", nullptr);
    QTest::newRow("SRAV") << I_R(0, 12, 13, 14, 0, 7) << str_inst_r("srav", "12", "13", "14", nullptr);
    /* Jump and branch */
    //QTest::newRow("JR") << I_R(0, 12, 13, 14, 0, 8) << str_inst_r("jr", "12", "13", "14", nullptr);
    //QTest::newRow("JALR") << I_R(0, 12, 13, 14, 0, 9) << str_inst_r("jalr", "12", "13", "14", nullptr);
    //QTest::newRow("BLTZ") << I_I(1, 12, 0, 42) << str_inst_i("bltz", "12", nullptr, "2a");
    //QTest::newRow("BGEZ") << I_I(1, 12, 1, 42) << str_inst_i("bltz", "12", nullptr, "2a");
    // TODO missing
    //QTest::newRow("BLTZAL") << I_I(1, 12, 16, 42) << str_inst_i("bltzal", "12", nullptr, "2a");
    //QTest::newRow("BGEZAL") << I_I(1, 12, 17, 42) << str_inst_i("bgezal", "12", nullptr, "2a");
    // TODO missing?
    //QTest::newRow("J") << I_J(1, 4242) << str_inst_j("j", "1092");
    //QTest::newRow("JAL") << I_J(2, 4242) << str_inst_j("jal", "1092");

    // TODO other instructions
}

void MachineTests::program_memory() {
    Memory m;
    ProgramMemory pm(&m);

    QFETCH(std::uint32_t, bin);
    QFETCH(QVector<QString>, str);

    m.write_word(0x00, bin);
    /*
    Instruction *i = pm.at(0x00);
    QVector<QString> s = i->to_strs();
    for (int i = 0; i < str.size(); i++) {
        std::cout << s[i].toStdString() << " ";
    }
    std::cout << std::endl;
    std::cout.flush();
    */
    QCOMPARE(pm.at(0x00)->to_strs(), str);
}
