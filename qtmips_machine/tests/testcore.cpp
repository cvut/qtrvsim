#include "tst_machine.h"
#include "core.h"

void MachineTests::core_regs_data() {
    /*
    QTest::addColumn<Instruction>("i");
    QTest::addColumn<Registers>("init");
    QTest::addColumn<Registers>("res");

    // Test arithmetic instructions
    {
    Registers regs_init();
    regs_init.write_gp(24, 12);
    regs_init.write_gp(25, 24);
    Registers regs_res(&regs_init);
    regs_res.write_gp(26, 36);
    QTest::newRow("ADD") << Instruction(0, 24, 25, 26, 0, 32) \
                         << regs_init \
                         << regs_res;
    }
    */
    // TODO test other operations
}

void MachineTests::core_regs() {

}

void MachineTests::core_mem_data() {

}

void MachineTests::core_mem() {

}
