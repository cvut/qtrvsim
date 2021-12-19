#include "registers.test.h"

#include "machine/registers.h"

using namespace machine;

void TestRegisters::registers_gp0() {
    Registers r;
    QCOMPARE(r.read_gp(0), RegisterValue(0));
    r.write_gp(0, 0xff);
    QCOMPARE(r.read_gp(0), RegisterValue(0));
}

void TestRegisters::registers_rw_gp() {
    Registers r;
    for (int i = 1; i < 32; i++) {
        r.write_gp(i, 0xf00 + i);
        QCOMPARE(r.read_gp(i), RegisterValue(0xf00 + i));
    }
}

void TestRegisters::registers_compare() {
    Registers r1, r2;
    QCOMPARE(r1, r2);
    // General purpose register
    r1.write_gp(1, 24);
    QVERIFY(r1 != r2);
    r2.write_gp(1, 24);
    QCOMPARE(r1, r2);
    // Program counter
    r1.write_pc(r1.read_pc() + 4);
    QVERIFY(r1 != r2);
    r2.write_pc(r2.read_pc() + 4);
    QCOMPARE(r1, r2);
    // Now let's try copy (and verify only with gp this time)
    Registers r3(r1);
    QCOMPARE(r3, r1);
    r3.write_gp(12, 19);
    QVERIFY(r1 != r3);
    r1.write_gp(12, 19);
    QCOMPARE(r3, r1);
}

QTEST_APPLESS_MAIN(TestRegisters)