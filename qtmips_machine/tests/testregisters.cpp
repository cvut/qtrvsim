#include "tst_machine.h"
#include <qtmipsexception.h>
#include <registers.h>

void MachineTests::registers_gp0() {
    Registers r;
    QCOMPARE(r.read_gp(0), (unsigned)0);
    r.write_gp(0, 0xff);
    QCOMPARE(r.read_gp(0), (unsigned)0);
}

void MachineTests::registers_rw_gp() {
    Registers r;
    for (int i = 1; i < 32; i++) {
        r.write_gp(i, 0xf00 + i);
        QCOMPARE(r.read_gp(i), (unsigned)(0xf00 + i));
    }
}

void MachineTests::registers_rw_hi_lo() {
    Registers r;
    r.write_hi_lo(false, 0xee);
    r.write_hi_lo(true, 0xaa);
    QCOMPARE(r.read_hi_lo(false), (unsigned)0xee);
    QCOMPARE(r.read_hi_lo(true), (unsigned)0xaa);
}

void MachineTests::registers_pc() {
    Registers r;
    QCOMPARE(r.read_pc(), (unsigned)0x80020000); // Check initial pc address
    QCOMPARE(r.pc_inc(), (unsigned)0x80020004);
    QCOMPARE(r.pc_inc(), (unsigned)0x80020008);
    QCOMPARE(r.pc_jmp(-0x8), (unsigned)0x80020000);
    QCOMPARE(r.pc_jmp(0xC), (unsigned)0x8002000C);
    r.pc_abs_jmp(0x80020100);
    QCOMPARE(r.read_pc(), (unsigned)0x80020100);
    QVERIFY_EXCEPTION_THROWN(r.pc_jmp(0x1), QtMipsExceptionUnalignedJump);
    QVERIFY_EXCEPTION_THROWN(r.pc_abs_jmp(0x80020101), QtMipsExceptionUnalignedJump);
}

void MachineTests::registers_compare() {
    Registers r1, r2;
    QCOMPARE(r1, r2);
    // General purpose register
    r1.write_gp(1, 24);
    QVERIFY(r1 != r2);
    r2.write_gp(1, 24);
    QCOMPARE(r1, r2);
    // Program counter
    r1.pc_inc();
    QVERIFY(r1 != r2);
    r2.pc_inc();
    QCOMPARE(r1, r2);
    // LO/HI (testing just one as they have common codepath)
    r1.write_hi_lo(false, 18);
    QVERIFY(r1 != r2);
    r2.write_hi_lo(false, 18);
    QCOMPARE(r1, r2);
    // Now let's try copy (and verify only with gp this time)
    Registers r3(r1);
    QCOMPARE(r3, r1);
    r3.write_gp(12, 19);
    QVERIFY(r1 != r3);
    r1.write_gp(12, 19);
    QCOMPARE(r3, r1);
}
