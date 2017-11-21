#include "tst_machine.h"
#include "core.h"

void MachineTests::core_regs_data() {
    QTest::addColumn<Instruction>("i");
    QTest::addColumn<Registers>("init");
    QTest::addColumn<Registers>("res");

    // Test arithmetic instructions
    {
    Registers regs_init;
    regs_init.write_gp(24, 12);
    regs_init.write_gp(25, 24);
    Registers regs_res(regs_init);
    regs_res.write_gp(26, 36);
    QTest::newRow("ADD") << Instruction(0, 24, 25, 26, 0, 32) \
                         << regs_init \
                         << regs_res;
    }
    // TODO test other operations
}

void MachineTests::core_regs() {
    QTest::addColumn<Instruction>("i");
    QTest::addColumn<Registers>("init");
    QTest::addColumn<Registers>("res");
    QFETCH(Instruction, i);
    QFETCH(Registers, init);
    QFETCH(Registers, res);

    Memory mem; // Just memory (it shouldn't be used here except instruction)
    mem.write_word(res.read_pc(), i.data()); // Store single instruction (anything else should be 0 so NOP effectively)

    // Test on non-piplined
    Memory mem_single(mem); // Create memory copy
    CoreSingle core_single(&init, &mem_single);
    core_single.step(); // Single step should be enought as this is risc without pipeline
    //QCOMPARE(init, res); // After doing changes from initial state this should be same state as in case of passed expected result
    QCOMPARE(mem, mem_single); // There should be no change in memory

    // TODO on pipelined core
}

void MachineTests::core_mem_data() {

}

void MachineTests::core_mem() {

}
