#include "tst_machine.h"
#include "core.h"

using namespace machine;

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

    // Move instructions
    {
    Registers regs_init;
    regs_init.write_hi_lo(true, 24);
    regs_init.write_hi_lo(false, 28);
    regs_init.write_gp(24, 55);
    regs_init.write_gp(25, 56);
    regs_init.write_gp(27, 21);
    regs_init.write_gp(28, 22);
    Registers regs_res(regs_init);
    regs_res.write_gp(26, 24);
    QTest::newRow("MFHI") << Instruction(0, 0, 0, 26, 0, 16) \
                         << regs_init \
                         << regs_res;
    regs_res.write_gp(26, 28);
    QTest::newRow("MFLO") << Instruction(0, 0, 0, 26, 0, 18) \
                         << regs_init \
                         << regs_res;
    regs_res.write_gp(26, 0);
    regs_res.write_hi_lo(true, 21);
    QTest::newRow("MTHI") << Instruction(0, 27, 0, 0, 0, 17) \
                         << regs_init \
                         << regs_res;
    regs_res.write_hi_lo(true, 24);
    regs_res.write_hi_lo(false, 22);
    QTest::newRow("MTLO") << Instruction(0, 28, 0, 0, 0, 19) \
                         << regs_init \
                         << regs_res;
    regs_res.write_hi_lo(false, 28);
    QTest::newRow("MOVZ-F") << Instruction(0, 24, 24, 25, 0, 10) \
                         << regs_init \
                         << regs_res;
    QTest::newRow("MOVN-F") << Instruction(0, 24, 1, 25, 0, 11) \
                         << regs_init \
                         << regs_res;
    regs_res.write_gp(25, 55);
    QTest::newRow("MOVZ-T") << Instruction(0, 24, 1, 25, 0, 10) \
                         << regs_init \
                         << regs_res;
    QTest::newRow("MOVN-T") << Instruction(0, 24, 24, 25, 0, 11) \
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

void MachineTests::singlecore_regs() {
    QFETCH(Instruction, i);
    QFETCH(Registers, init);
    QFETCH(Registers, res);

    Memory mem; // Just memory (it shouldn't be used here except instruction)
    mem.write_word(res.read_pc(), i.data()); // Store single instruction (anything else should be 0 so NOP effectively)
    Memory mem_used(mem); // Create memory copy

    CoreSingle core(&init, &mem_used, true);
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

    res.pc_jmp(0x14);

    CorePipelined core(&init, &mem_used);
    for (int i = 0; i < 5; i++)
        core.step(); // Fire steps for five pipelines stages

    //cout << "well:" << init.read_gp(26) << ":" << regs_used.read_gp(26) << endl;
    QCOMPARE(init, res); // After doing changes from initial state this should be same state as in case of passed expected result
    QCOMPARE(mem, mem_used); // There should be no change in memory
}

static void core_jmp_data() {
    QTest::addColumn<Instruction>("i");
    QTest::addColumn<Registers>("regs");
    QTest::addColumn<std::uint32_t>("pc");

    Registers regs;
    regs.write_gp(14, -22);
    regs.write_gp(15, 22);
    regs.write_gp(16, -22);
    regs.write_gp(12, 0x80040000);
    QTest::newRow("B") << Instruction(4, 0, 0, 61) \
                         << regs \
                         << regs.read_pc() + 4 + (61 << 2);
    QTest::newRow("BEQ") << Instruction(4, 14, 16, 61) \
                         << regs \
                         << regs.read_pc() + 4 + (61 << 2);
    QTest::newRow("BNE") << Instruction(5, 14, 15, 61) \
                         << regs \
                         << regs.read_pc() + 4 + (61 << 2);
    QTest::newRow("BGEZ") << Instruction(1, 15, 1, 61) \
                         << regs \
                         << regs.read_pc() + 4 + (61 << 2);
    QTest::newRow("BGTZ") << Instruction(7, 15, 0, 61) \
                         << regs \
                         << regs.read_pc() + 4 + (61 << 2);
    QTest::newRow("BLEZ") << Instruction(6, 14, 0, 61) \
                         << regs \
                         << regs.read_pc() + 4 + (61 << 2);
    QTest::newRow("BLTZ") << Instruction(1, 14, 0, 61) \
                         << regs \
                         << regs.read_pc() + 4 + (61 << 2);
    QTest::newRow("J") << Instruction(2, 24) \
                         << regs \
                         << 0x80000000 + (24 << 2);
    QTest::newRow("JR") << Instruction(0, 12, 0, 0, 0, 8) \
                         << regs \
                         << 0x80040000;
}

void MachineTests::singlecore_jmp_data() {
    core_jmp_data();
}

void MachineTests::pipecore_jmp_data() {
    core_jmp_data();
}

void MachineTests::singlecore_jmp() {
    QFETCH(Instruction, i);
    QFETCH(Registers, regs);
    QFETCH(std::uint32_t, pc);

    Memory mem;
    mem.write_word(regs.read_pc(), i.data());
    Memory mem_used(mem);
    Registers regs_used(regs);

    CoreSingle core(&regs_used, &mem_used, true);
    core.step();
    QCOMPARE(regs.read_pc() + 4, regs_used.read_pc()); // First execute delay slot
    core.step();
    QCOMPARE(pc, regs_used.read_pc()); // Now do jump

    QCOMPARE(mem, mem_used); // There should be no change in memory
    regs_used.pc_abs_jmp(regs.read_pc()); // Reset program counter before we do registers compare
    QCOMPARE(regs, regs_used); // There should be no change in registers now
}

void MachineTests::pipecore_jmp() {
    QFETCH(Instruction, i);
    QFETCH(Registers, regs);
    QFETCH(std::uint32_t, pc);

    Memory mem;
    mem.write_word(regs.read_pc(), i.data());
    Memory mem_used(mem);
    Registers regs_used(regs);

    CorePipelined core(&regs_used, &mem_used);
    core.step();
    QCOMPARE(regs.read_pc() + 4, regs_used.read_pc()); // First just fetch
    core.step();
    QCOMPARE(pc, regs_used.read_pc()); // Now do jump
    for (int i = 0; i < 3; i++)
        core.step(); // Follow up with three other steps to complete pipeline to be sure that instruction has no side effects

    QCOMPARE(mem, mem_used); // There should be no change in memory
    regs.pc_abs_jmp(pc + 12); // Set reference pc to three more instructions later (where regs_used should be)
    QCOMPARE(regs, regs_used); // There should be no change in registers now (except pc)
}

static void core_mem_data() {
    QTest::addColumn<Instruction>("i");
    QTest::addColumn<Registers>("regs_init");
    QTest::addColumn<Registers>("regs_res");
    QTest::addColumn<Memory>("mem_init");
    QTest::addColumn<Memory>("mem_res");

    // Load
    {
    Memory mem;
    mem.write_word(0x24, 0xA3242526);
    Registers regs;
    regs.write_gp(1, 0x22);
    Registers regs_res(regs);
    regs_res.write_gp(21, 0x80000023);
    QTest::newRow("LB") << Instruction(32, 1, 21, 0x2) \
                         << regs \
                         << regs_res \
                         << mem \
                         << mem;
    regs_res.write_gp(21, 0x80002324);
    QTest::newRow("LH") << Instruction(33, 1, 21, 0x2) \
                         << regs \
                         << regs_res \
                         << mem \
                         << mem;
    regs_res.write_gp(21, 0xA3242526);
    QTest::newRow("LW") << Instruction(35, 1, 21, 0x2) \
                         << regs \
                         << regs_res \
                         << mem \
                         << mem;
    regs_res.write_gp(21, 0x000000A3);
    QTest::newRow("LBU") << Instruction(36, 1, 21, 0x2) \
                         << regs \
                         << regs_res \
                         << mem \
                         << mem;
    regs_res.write_gp(21, 0x0000A324);
    QTest::newRow("LHU") << Instruction(37, 1, 21, 0x2) \
                         << regs \
                         << regs_res \
                         << mem \
                         << mem;
    }
    // Store
    {
    Registers regs;
    regs.write_gp(1, 0x22);
    regs.write_gp(21, 0x23242526);
    Memory mem;
    mem.write_byte(0x24, 0x26); // Note: store least significant byte
    QTest::newRow("SB") << Instruction(40, 1, 21, 0x2) \
                         << regs \
                         << regs \
                         << Memory() \
                         << mem;
    mem.write_hword(0x24, 0x2526);
    QTest::newRow("SH") << Instruction(41, 1, 21, 0x2) \
                         << regs \
                         << regs \
                         << Memory() \
                         << mem;
    mem.write_word(0x24, 0x23242526);
    QTest::newRow("SH") << Instruction(43, 1, 21, 0x2) \
                         << regs \
                         << regs \
                         << Memory() \
                         << mem;
    }
}

void MachineTests::singlecore_mem_data() {
    core_mem_data();
}

void MachineTests::pipecore_mem_data() {
    core_mem_data();
}

void MachineTests::singlecore_mem() {
    QFETCH(Instruction, i);
    QFETCH(Registers, regs_init);
    QFETCH(Registers, regs_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);

    // Write instruction to both memories
    mem_init.write_word(regs_init.read_pc(), i.data());
    mem_res.write_word(regs_init.read_pc(), i.data());

    CoreSingle core(&regs_init, &mem_init, true);
    core.step();

    regs_res.pc_inc();
    QCOMPARE(regs_init, regs_res);
    QCOMPARE(mem_init, mem_res);
}

void MachineTests::pipecore_mem() {
    QFETCH(Instruction, i);
    QFETCH(Registers, regs_init);
    QFETCH(Registers, regs_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);

    // Write instruction to both memories
    mem_init.write_word(regs_init.read_pc(), i.data());
    mem_res.write_word(regs_init.read_pc(), i.data());

    CorePipelined core(&regs_init, &mem_init);
    for (int i = 0; i < 5; i++)
        core.step(); // Fire steps for five pipelines stages

    regs_res.pc_jmp(20);
    QCOMPARE(regs_init, regs_res);
    QCOMPARE(mem_init, mem_res);
}
