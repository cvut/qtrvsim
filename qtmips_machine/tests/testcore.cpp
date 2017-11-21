#include "tst_machine.h"
#include "core.h"

void MachineTests::core_regs_data() {
    QTest::addColumn<Instruction>("i");
    QTest::addColumn<Registers>("init");
    QTest::addColumn<Registers>("res");
    // Note that we shouldn't be touching program counter as that is handled automatically and differs if we use pipelining

    // Arithmetic instructions
    {
    Registers regs_init;
    regs_init.write_gp(24, 24);
    regs_init.write_gp(25, 12);
    Registers regs_res(regs_init);
    regs_res.write_gp(26, 36);
    QTest::newRow("ADD") << Instruction(0, 24, 25, 26, 0, 32) \
                         << regs_init \
                         << regs_res;
    QTest::newRow("ADDU") << Instruction(0, 24, 25, 26, 0, 33) \
                         << regs_init \
                         << regs_res;
    regs_res.write_gp(26, 12);
    QTest::newRow("SUB") << Instruction(0, 24, 25, 26, 0, 34) \
                         << regs_init \
                         << regs_res;
    QTest::newRow("SUBU") << Instruction(0, 24, 25, 26, 0, 35) \
                         << regs_init \
                         << regs_res;
    }
    {
    Registers regs_init;
    regs_init.write_gp(24, 12);
    regs_init.write_gp(25, 24);
    Registers regs_res(regs_init);
    regs_res.write_gp(26, 1);
    QTest::newRow("SLT") << Instruction(0, 24, 25, 26, 0, 42) \
                         << regs_init \
                         << regs_res;
    QTest::newRow("SLTU") << Instruction(0, 24, 25, 26, 0, 43) \
                         << regs_init \
                         << regs_res;
    }

    // Shift instructions
    {
    Registers regs_init;
    regs_init.write_gp(24, 0xf0);
    regs_init.write_gp(25, 3);
    Registers regs_res(regs_init);
    regs_res.write_gp(26, 0x780);
    QTest::newRow("SLL") << Instruction(0, 0, 24, 26, 3, 0) \
                         << regs_init \
                         << regs_res;
    QTest::newRow("SLLV") << Instruction(0, 25, 24, 26, 0, 4) \
                         << regs_init \
                         << regs_res;
    regs_res.write_gp(26, 0x1e);
    QTest::newRow("SLR") << Instruction(0, 0, 24, 26, 3, 2) \
                         << regs_init \
                         << regs_res;
    QTest::newRow("SLRV") << Instruction(0, 25, 24, 26, 0, 6) \
                         << regs_init \
                         << regs_res;
    }
    {
    Registers regs_init;
    regs_init.write_gp(24, 0x800000f0);
    regs_init.write_gp(25, 3);
    Registers regs_res(regs_init);
    regs_res.write_gp(26, 0x8000001e);
    QTest::newRow("SRA") << Instruction(0, 0, 24, 26, 3, 3) \
                         << regs_init \
                         << regs_res;
    QTest::newRow("SRAV") << Instruction(0, 25, 24, 26, 0, 7) \
                         << regs_init \
                         << regs_res;
    }
    // TODO test other operations
}

/*
#include <iostream>
using namespace std;
*/

void MachineTests::core_regs() {
    QFETCH(Instruction, i);
    QFETCH(Registers, init);
    QFETCH(Registers, res);

    Memory mem; // Just memory (it shouldn't be used here except instruction)
    mem.write_word(res.read_pc(), i.data()); // Store single instruction (anything else should be 0 so NOP effectively)

    // Test on non-piplined
    res.pc_inc(); // We did single step	so increment program counter accordingly
    Memory mem_single(mem); // Create memory copy
    Registers regs_single(init); // Create registers copy
    CoreSingle core_single(&regs_single, &mem_single);
    core_single.step(); // Single step should be enought as this is risc without pipeline
    //cout << "well:" << hex << regs_single.read_gp(26) << endl;
    QCOMPARE(regs_single, res); // After doing changes from initial state this should be same state as in case of passed expected result
    QCOMPARE(mem, mem_single); // There should be no change in memory

    // TODO on pipelined core
}

void MachineTests::core_mem_data() {

}

void MachineTests::core_mem() {

}
