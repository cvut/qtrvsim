#include "tst_machine.h"
#include "programmemory.h"
#include <qstring.h>
#include <qvector.h>

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

#define I(II) ((std::uint32_t) II)

void MachineTests::program_memory_data() {
    QTest::addColumn<std::uint32_t>("bin");
    QTest::addColumn<QVector<QString>>("str");

    // TODO correct instruction
    QTest::newRow("NOP") << I(0x000000) << str_inst_r("nop", nullptr, nullptr, nullptr, nullptr);
    //QTest::newRow("SLL") << I(0x000000) << str_inst_r("sll", "", "", nullptr, nullptr);
    // TODO other instructions
}

void MachineTests::program_memory() {
    Memory m;
    ProgramMemory pm(&m);

   QFETCH(std::uint32_t, bin);
   QFETCH(QVector<QString>, str);

   m.write_word(0x00, bin);
   QCOMPARE(pm.at(0x00)->to_strs(), str);
}
