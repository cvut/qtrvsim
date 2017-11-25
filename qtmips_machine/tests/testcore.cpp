#include "tst_machine.h"
#include "core.h"

static void core_regs_data() {
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
    QTest::newRow("ADDI") << Instruction(8, 24, 26, 12) \
                         << regs_init \
                         << regs_res;
    QTest::newRow("ADDIU") << Instruction(9, 24, 26, 12) \
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
    QTest::newRow("SLTI") << Instruction(10, 24, 26, 24) \
                         << regs_init \
                         << regs_res;
    QTest::newRow("SLTIU") << Instruction(11, 24, 26, 24) \
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

    // Logical instructions
    {
    Registers regs_init;
    regs_init.write_gp(24, 0xf0);
    regs_init.write_gp(25, 0xe1);
    Registers regs_res(regs_init);
    regs_res.write_gp(26, 0xe0);
    QTest::newRow("AND") << Instruction(0, 24, 25, 26, 0, 36) \
                         << regs_init \
                         << regs_res;
    QTest::newRow("ANDI") << Instruction(12, 24, 26, 0xe1) \
                         << regs_init \
                         << regs_res;
    regs_res.write_gp(26, 0xf1);
    QTest::newRow("OR") << Instruction(0, 24, 25, 26, 0, 37) \
                         << regs_init \
                         << regs_res;
    QTest::newRow("ORI") << Instruction(13, 24, 26, 0xe1) \
                         << regs_init \
                         << regs_res;
    regs_res.write_gp(26, 0x11);
    QTest::newRow("XOR") << Instruction(0, 24, 25, 26, 0, 38) \
                         << regs_init \
                         << regs_res;
    QTest::newRow("XORI") << Instruction(14, 24, 26, 0xe1) \
                         << regs_init \
                         << regs_res;
    regs_res.write_gp(26, 0xffffff0e);
    QTest::newRow("NOR") << Instruction(0, 24, 25, 26, 0, 39) \
                         << regs_init \
                         << regs_res;
    }
}

void MachineTests::singlecore_regs_data() {
    core_regs_data();
}

void MachineTests::pipecore_regs_data() {
    core_regs_data();
}

#include <iostream>
using namespace std;

void MachineTests::singlecore_regs() {
    QFETCH(Instruction, i);
    QFETCH(Registers, init);
    QFETCH(Registers, res);

    Memory mem; // Just memory (it shouldn't be used here except instruction)
    mem.write_word(res.read_pc(), i.data()); // Store single instruction (anything else should be 0 so NOP effectively)
    Memory mem_used(mem); // Create memory copy

    CoreSingle core(&init, &mem_used);
    core.step(); // Single step should be enought as this is risc without pipeline

    res.pc_inc(); // We did single step	so increment program counter accordingly
    QCOMPARE(init, res); // After doing changes from initial state this should be same state as in case of passed expected result
    QCOMPARE(mem, mem_used); // There should be no change in memory
}

void MachineTests::pipecore_regs() {
    QFETCH(Instruction, i);
    QFETCH(Registers, init);
    QFETCH(Registers, res);

    Memory mem; // Just memory (it shouldn't be used here except instruction)
    mem.write_word(res.read_pc(), i.data()); // Store single instruction (anything else should be 0 so NOP effectively)

    Memory mem_used(mem);
    Registers regs_used(init);

    res.pc_jmp(0x14);

    CorePipelined core(&regs_used, &mem_used);
    for (int i = 0; i < 4; i++) {
        core.step(); // Fire steps for five pipelines stages
        init.pc_inc();
        QCOMPARE(init, regs_used); // Untill writeback there should be no change in registers
    }
    core.step();

    //cout << "well:" << init.read_gp(26) << ":" << regs_used.read_gp(26) << endl;
    QCOMPARE(regs_used, res); // After doing changes from initial state this should be same state as in case of passed expected result
    QCOMPARE(mem, mem_used); // There should be no change in memory
}

void MachineTests::core_mem_data() {

}

void MachineTests::core_mem() {

}
