#include "core.test.h"

#include "machine/core.h"
#include "machine/machineconfig.h"
#include "machine/memory/backend/memory.h"
#include "machine/memory/cache/cache.h"
#include "machine/memory/memory_bus.h"
#include "machine/predictor.h"

#include <QVector>

using std::vector;

using namespace machine;

Q_DECLARE_METATYPE(Xlen) // NOLINT(performance-no-int-to-ptr)

/**
 * Compiles program with no relocations into memory
 *
 * @param memory    memory to save program to
 * @param init_pc   address, where the compiled program will start
 * @param instructions vector of instructions
 * @return          number of instruction compiled (number of steps, that can be executed)
 */
size_t compile_simple_program(
    FrontendMemory &memory,
    Address init_pc,
    const std::vector<QString> &instructions) {
    Address pc = init_pc;
    uint32_t code[2];
    for (auto &instruction : instructions) {
        size_t size = Instruction::code_from_string(code, 8, instruction, pc);
        for (size_t i = 0; i < size; i += 4, pc += 4) {
            memory.write_u32(pc, code[i]);
        }
    }
    return (pc - init_pc) / 4;
}

template<typename Core>
void test_program_with_single_result() {
    QFETCH(vector<QString>, instructions);
    QFETCH(Registers, registers);
    QFETCH(RegisterValue, x10_result);
    QFETCH(Xlen, xlen);

    Memory memory_backend(BIG);
    TrivialBus memory(&memory_backend);
    BranchPredictor predictor {};
    CSR::ControlState controlst {};
    Core core(&registers, &predictor, &memory, &memory, &controlst, Xlen::_32, config_isa_word_default);

    size_t instruction_count = compile_simple_program(memory, 0x200_addr, instructions);
    if (typeid(Core) == typeid(CorePipelined)) { instruction_count += 3; } // finish pipeline
    for (size_t i = 0; i < instruction_count; i++) {
        core.step();
    }
    QCOMPARE(registers.read_gp(10).as_xlen(xlen), x10_result.as_xlen(xlen));
}

static void core_regs_data() {
    QSKIP("Switched RV.");
    QTest::addColumn<Instruction>("i");
    QTest::addColumn<Registers>("init");
    QTest::addColumn<Registers>("res");
    // Note that we shouldn't be touching program counter as that is handled
    // automatically and differs if we use pipelining

    // Arithmetic instructions
    {
        Registers regs_init;
        regs_init.write_gp(24, 24);
        regs_init.write_gp(25, 12);
        Registers regs_res(regs_init);
        regs_res.write_gp(26, 36);
        //        QTest::newRow("ADD") << Instruction(0, 24, 25, 26, 0, 32) << regs_init <<
        //        regs_res; QTest::newRow("ADDU") << Instruction(0, 24, 25, 26, 0, 33) <<
        //        regs_init
        //        << regs_res; QTest::newRow("ADDI") << Instruction(8, 24, 26, 12) << regs_init
        //        << regs_res; QTest::newRow("ADDIU") << Instruction(9, 24, 26, 12) << regs_init
        //        << regs_res;
        regs_res.write_gp(26, 12);
        //        QTest::newRow("SUB") << Instruction(0, 24, 25, 26, 0, 34) << regs_init <<
        //        regs_res; QTest::newRow("SUBU") << Instruction(0, 24, 25, 26, 0, 35) <<
        //        regs_init
        //        << regs_res;
    }
    {
        Registers regs_init;
        regs_init.write_gp(24, 12);
        regs_init.write_gp(25, 24);
        Registers regs_res(regs_init);
        regs_res.write_gp(26, 1);
        //        QTest::newRow("SLT") << Instruction(0, 24, 25, 26, 0, 42) << regs_init <<
        //        regs_res; QTest::newRow("SLTU") << Instruction(0, 24, 25, 26, 0, 43) <<
        //        regs_init
        //        << regs_res; QTest::newRow("SLTI") << Instruction(10, 24, 26, 24) << regs_init
        //        << regs_res; QTest::newRow("SLTIU") << Instruction(11, 24, 26, 24) <<
        //        regs_init << regs_res;
    }

    // Shift instructions
    {
        Registers regs_init;
        regs_init.write_gp(24, 0xf0);
        regs_init.write_gp(25, 3);
        Registers regs_res(regs_init);
        regs_res.write_gp(26, 0x780);
        //        QTest::newRow("SLL") << Instruction(0, 0, 24, 26, 3, 0) << regs_init <<
        //        regs_res; QTest::newRow("SLLV") << Instruction(0, 25, 24, 26, 0, 4) <<
        //        regs_init << regs_res;
        regs_res.write_gp(26, 0x1e);
        //        QTest::newRow("SLR") << Instruction(0, 0, 24, 26, 3, 2) << regs_init <<
        //        regs_res; QTest::newRow("SLRV") << Instruction(0, 25, 24, 26, 0, 6) <<
        //        regs_init << regs_res;
    }
    {
        Registers regs_init;
        regs_init.write_gp(24, 0x800000f0);
        regs_init.write_gp(25, 3);
        Registers regs_res(regs_init);
        // Cast is needed to correctly work with any internal size of register.
        regs_res.write_gp(26, (int32_t)0xF000001e);
        //        QTest::newRow("SRA") << Instruction(0, 0, 24, 26, 3, 3) << regs_init <<
        //        regs_res; QTest::newRow("SRAV") << Instruction(0, 25, 24, 26, 0, 7) <<
        //        regs_init << regs_res;
    }

    // Logical instructions
    {
        Registers regs_init;
        regs_init.write_gp(24, 0xf0);
        regs_init.write_gp(25, 0xe1);
        Registers regs_res(regs_init);
        regs_res.write_gp(26, 0xe0);
        //        QTest::newRow("AND") << Instruction(0, 24, 25, 26, 0, 36) << regs_init <<
        //        regs_res; QTest::newRow("ANDI") << Instruction(12, 24, 26, 0xe1) << regs_init
        //        << regs_res;
        regs_res.write_gp(26, 0xf1);
        //        QTest::newRow("OR") << Instruction(0, 24, 25, 26, 0, 37) << regs_init <<
        //        regs_res; QTest::newRow("ORI") << Instruction(13, 24, 26, 0xe1) << regs_init
        //        << regs_res;
        regs_res.write_gp(26, 0x11);
        //        QTest::newRow("XOR") << Instruction(0, 24, 25, 26, 0, 38) << regs_init <<
        //        regs_res; QTest::newRow("XORI") << Instruction(14, 24, 26, 0xe1) << regs_init
        //        << regs_res;
        regs_res.write_gp(26, 0xffffff0e);
        //        QTest::newRow("NOR") << Instruction(0, 24, 25, 26, 0, 39) << regs_init <<
        //        regs_res; regs_res.write_gp(26, 0xf00f0000); QTest::newRow("LUI") <<
        //        Instruction(15, 0, 26, 0xf00f) << regs_init << regs_res;
    }

    // Move instructions
    {
        Registers regs_init;
        Registers regs_res(regs_init);
        regs_res.write_gp(26, 24);
        //        QTest::newRow("MFHI") << Instruction(0, 0, 0, 26, 0, 16) << regs_init <<
        //        regs_res;
        regs_res.write_gp(26, 28);
        //        QTest::newRow("MFLO") << Instruction(0, 0, 0, 26, 0, 18) << regs_init <<
        //        regs_res;
        regs_res.write_gp(26, 0);
        //        QTest::newRow("MTHI") << Instruction(0, 27, 0, 0, 0, 17) << regs_init <<
        //        regs_res; QTest::newRow("MTLO") << Instruction(0, 28, 0, 0, 0, 19) <<
        //        regs_init << regs_res; QTest::newRow("MOVZ-F") << Instruction(0, 24, 24, 25,
        //        0, 10) << regs_init << regs_res; QTest::newRow("MOVN-F") << Instruction(0, 24,
        //        1, 25, 0, 11) << regs_init
        //        << regs_res;
        regs_res.write_gp(25, 55);
        //        QTest::newRow("MOVZ-T") << Instruction(0, 24, 1, 25, 0, 10) << regs_init <<
        //        regs_res; QTest::newRow("MOVN-T") << Instruction(0, 24, 24, 25, 0, 11) <<
        //        regs_init << regs_res;
    }
}

void TestCore::singlecore_regs_data() {
    QSKIP("Switched ALU to RV.");
    core_regs_data();
}

void TestCore::pipecore_regs_data() {
    QSKIP("Switched ALU to RV.");
    core_regs_data();
}

void TestCore::singlecore_regs() {
    QSKIP("Switched ALU to RV.");
    QFETCH(Instruction, i);
    QFETCH(Registers, init);
    QFETCH(Registers, res);

    Memory mem(BIG); // Just memory (it shouldn't be used here except
                     // instruction)
    TrivialBus mem_frontend(&mem);
    memory_write_u32(&mem, res.read_pc().get_raw(), i.data()); // Store single
                                                               // instruction
                                                               // (anything else
                                                               // should be 0 so
                                                               // NOP
                                                               // effectively)
    Memory mem_used(mem);                                      // Create memory copy
    TrivialBus mem_used_frontend(&mem);

    //    CoreSingle core(&init, &mem_used_frontend, &mem_used_frontend, true, nullptr,
    //    Xlen::_32); core.step(); // Single step should be enought as this is risc without
    // pipeline
    //    core.step();

    //    res.pc_inc();
    //    res.pc_inc();        // We did single step	so increment program counter accordingly
    QCOMPARE(init, res); // After doing changes from initial state this should
                         // be same state as in case of passed expected result
}

void TestCore::pipecore_regs() {
    QSKIP("Switched ALU to RV.");
    QFETCH(Instruction, i);
    QFETCH(Registers, init);
    QFETCH(Registers, res);

    Memory mem(BIG); // Just memory (it shouldn't be used here except
                     // instruction)
    TrivialBus mem_frontend(&mem);
    memory_write_u32(&mem, res.read_pc().get_raw(), i.data()); // Store single
                                                               // instruction
                                                               // (anything else
                                                               // should be 0 so
                                                               // NOP
                                                               // effectively)

    Memory mem_used(mem);
    TrivialBus mem_used_frontend(&mem_used);

    res.write_pc(0x14_addr);

    //    CorePipelined core(
    //        &init, &mem_used_frontend, &mem_used_frontend, nullptr, MachineConfig::HU_NONE, 0,
    //        nullptr, Xlen::_32);
    //    for (int i = 0; i < 5; i++) {
    //        core.step(); // Fire steps for five pipelines stages
    //    }
    //
    //    cout << "well:" << init.read_gp(26) << ":" << regs_used.read_gp(26) << endl;
    //    QCOMPARE(init, res); // After doing changes from initial state this should
    //    be same state as in case of passed expected result QCOMPARE(mem, mem_used); // There
    //    should be
    // no change in
    // memory
}

static void core_jmp_data() {
    QSKIP("Switched RV.");
    QTest::addColumn<Instruction>("i");
    QTest::addColumn<Registers>("regs");
    QTest::addColumn<uint64_t>("pc");

    Registers regs;
    regs.write_gp(14, -22);
    regs.write_gp(15, 22);
    regs.write_gp(16, -22);
    regs.write_gp(12, 0x80040000);
    //    QTest::newRow("B") << Instruction(4, 0, 0, 61) << regs
    //                       << regs.read_pc().get_raw() + 4 + (61 << 2);
    //    QTest::newRow("BEQ") << Instruction(4, 14, 16, 61) << regs
    //                         << regs.read_pc().get_raw() + 4 + (61 << 2);
    //    QTest::newRow("BEQ-BACK") << Instruction(4, 14, 16, -4) << regs
    //                              << regs.read_pc().get_raw() + 4 - 16;
    //    QTest::newRow("BNE") << Instruction(5, 14, 15, 61) << regs
    //                         << regs.read_pc().get_raw() + 4 + (61 << 2);
    //    QTest::newRow("BGEZ") << Instruction(1, 15, 1, 61) << regs
    //                          << regs.read_pc().get_raw() + 4 + (61 << 2);
    //    QTest::newRow("BGTZ") << Instruction(7, 15, 0, 61) << regs
    //                          << regs.read_pc().get_raw() + 4 + (61 << 2);
    //    QTest::newRow("BLEZ") << Instruction(6, 14, 0, 61) << regs
    //                          << regs.read_pc().get_raw() + 4 + (61 << 2);
    //    QTest::newRow("BLTZ") << Instruction(1, 14, 0, 61) << regs
    //                          << regs.read_pc().get_raw() + 4 + (61 << 2);
    //    QTest::newRow("J") << Instruction(2, Address(24)) << regs
    //                       << Address(0x80000000).get_raw() + (24 << 2);
    //    QTest::newRow("JR") << Instruction(0, 12, 0, 0, 0, 8) << regs <<
    //    Address(0x80040000).get_raw();
}

void TestCore::singlecore_jmp_data() {
    QSKIP("Switched ALU to RV.");
    core_jmp_data();
}

void TestCore::pipecore_jmp_data() {
    QSKIP("Switched ALU to RV.");
    core_jmp_data();
}

void TestCore::singlecore_jmp() {
    QSKIP("Switched ALU to RV.");
    QFETCH(Instruction, i);
    QFETCH(Registers, regs);
    // QFETCH(uint64_t, pc);

    Memory mem(BIG);
    TrivialBus mem_frontend(&mem);
    memory_write_u32(&mem, regs.read_pc().get_raw(), i.data());
    Memory mem_used(mem);
    TrivialBus mem_used_frontend(&mem_used);
    Registers regs_used(regs);

    //    CoreSingle core(
    //        &regs_used, &mem_used_frontend, &mem_used_frontend, true, nullptr, Xlen::_32);
    //    core.step();
    //    QCOMPARE(regs.read_pc() + 4, regs_used.read_pc()); // First execute delay
    //                                                            slot
    //    core.step();
    //    QCOMPARE(pc, regs_used.read_pc().get_raw()); // Now do jump
    //
    //    QCOMPARE(mem, mem_used);              // There should be no change in memory
    //    regs_used.pc_abs_jmp(regs.read_pc()); // Reset program counter before we do
    //                                               registers compare
    //    QCOMPARE(regs, regs_used); // There should be no change in registers now
}

void TestCore::pipecore_jmp() {
    QSKIP("Switched ALU to RV.");
    QFETCH(Instruction, i);
    QFETCH(Registers, regs);
    // QFETCH(uint64_t, pc);

    Memory mem(BIG);
    TrivialBus mem_frontend(&mem);
    memory_write_u32(&mem, regs.read_pc().get_raw(), i.data());
    Memory mem_used(mem);
    TrivialBus mem_used_frontend(&mem_used);
    Registers regs_used(regs);

    //    CorePipelined core(
    //        &regs_used, &mem_used_frontend, &mem_used_frontend, nullptr,
    //        MachineConfig::HU_NONE, 0, nullptr, Xlen::_32);
    //    core.step();
    //    QCOMPARE(regs.read_pc() + 4, regs_used.read_pc()); // First just fetch
    //    core.step();
    //    QCOMPARE(pc, regs_used.read_pc().get_raw()); // Now do jump
    //    for (int i = 0; i < 3; i++) {
    //        core.step(); // Follow up with three other steps to complete pipeline to
    //    }
    //     be sure that instruction has no side effects
    //
    //    QCOMPARE(mem, mem_used);           // There should be no change in memory
    //    regs.pc_abs_jmp(Address(pc + 12)); // Set reference pc to three more
    //                                        instructions later (where regs_used
    //                                        should be)
    //    QCOMPARE(regs, regs_used);         // There should be no change in registers now
    // (except pc)
}

static void core_mem_data() {
    QTest::addColumn<Instruction>("i");
    QTest::addColumn<Registers>("regs_init");
    QTest::addColumn<Registers>("regs_res");
    QTest::addColumn<Memory>("mem_init");
    QTest::addColumn<Memory>("mem_res");

    // Load
    {
        Memory mem(BIG);
        memory_write_u32(&mem, 0x24, 0xA3242526);
        Registers regs;
        regs.write_gp(1, 0x22);
        Registers regs_res(regs);
        // Cast to get proper sign extension.
        regs_res.write_gp(21, (int32_t)0xFFFFFFA3);
        //        QTest::newRow("LB") << Instruction(32, 1, 21, 0x2) << regs << regs_res << mem
        //        << mem;
        //         Cast to get proper sign extension.
        //        regs_res.write_gp(21, (int32_t)0xFFFFA324);
        //        QTest::newRow("LH") << Instruction(33, 1, 21, 0x2) << regs << regs_res << mem
        //        << mem; regs_res.write_gp(21, 0xA3242526); QTest::newRow("LW") <<
        //        Instruction(35, 1, 21, 0x2) << regs << regs_res << mem << mem;
        //        regs_res.write_gp(21, 0x000000A3); QTest::newRow("LBU") << Instruction(36, 1,
        //        21, 0x2) << regs << regs_res << mem << mem; regs_res.write_gp(21, 0x0000A324);
        //        QTest::newRow("LHU") << Instruction(37, 1, 21, 0x2) << regs << regs_res << mem
        //        << mem;
    }
    // Store
    {
        Registers regs;
        regs.write_gp(1, 0x22);
        regs.write_gp(21, 0x23242526);
        Memory mem(BIG);
        memory_write_u8(&mem, 0x24, 0x26); // Note: store least significant byte
        //        QTest::newRow("SB") << Instruction(40, 1, 21, 0x2) << regs << regs <<
        //        Memory(BIG)
        //        << mem; memory_write_u16(&mem, 0x24, 0x2526); QTest::newRow("SH") <<
        //        Instruction(41, 1, 21, 0x2) << regs << regs << Memory(BIG) << mem;
        //        memory_write_u32(&mem, 0x24, 0x23242526);
        //        QTest::newRow("SW") << Instruction(43, 1, 21, 0x2) << regs << regs <<
        //        Memory(BIG)
        //        << mem;
    }
}

void TestCore::singlecore_mem_data() {
    QSKIP("Switched ALU to RV.");
    core_mem_data();
}

void TestCore::pipecore_mem_data() {
    QSKIP("Switched ALU to RV.");
    core_mem_data();
}

void TestCore::singlecore_mem() {
    QSKIP("Switched ALU to RV.");
    QFETCH(Instruction, i);
    QFETCH(Registers, regs_init);
    QFETCH(Registers, regs_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);

    // Write instruction to both memories
    memory_write_u32(&mem_init, regs_init.read_pc().get_raw(), i.data());
    memory_write_u32(&mem_res, regs_init.read_pc().get_raw(), i.data());

    TrivialBus mem_init_frontend(&mem_init);
    //    CoreSingle core(
    //        &regs_init, &mem_init_frontend, &mem_init_frontend, true, nullptr, Xlen::_32);
    //    core.step();
    //    core.step();
    //
    //    regs_res.pc_inc();
    //    regs_res.pc_inc();
    //    QCOMPARE(regs_init, regs_res);
    //    QCOMPARE(mem_init, mem_res);
}

void TestCore::pipecore_mem() {
    QSKIP("Switched ALU to RV.");
    QFETCH(Instruction, i);
    QFETCH(Registers, regs_init);
    QFETCH(Registers, regs_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);

    // Write instruction to both memories
    memory_write_u32(&mem_init, regs_init.read_pc().get_raw(), i.data());
    memory_write_u32(&mem_res, regs_init.read_pc().get_raw(), i.data());

    TrivialBus mem_init_frontend(&mem_init);
    //    CorePipelined core(
    //        &regs_init, &mem_init_frontend, &mem_init_frontend, nullptr,
    //        MachineConfig::HU_NONE, 0, nullptr, Xlen::_32);
    //    for (int i = 0; i < 5; i++) {
    //        core.step(); // Fire steps for five pipelines stages
    //    }
    //
    //    regs_res.pc_jmp(20);
    //    QCOMPARE(regs_init, regs_res);
    //    QCOMPARE(mem_init, mem_res);
}

/*======================================================================*/

static void core_alu_forward_data() {
    QTest::addColumn<QVector<uint32_t>>("code");
    QTest::addColumn<Registers>("reg_init");
    QTest::addColumn<Registers>("reg_res");
    // Note that we shouldn't be touching program counter as that is handled
    // automatically and differs if we use pipelining

    // Test forwarding of ALU operands
    {
        QVector<uint32_t> code {
            // objdump --disassembler-options=no-aliases,numeric -d test-hazards
            // sed -n -e 's/^[ \t]*[^ \t]\+:[ \t]\+\([0-9a-f]\+\)[ \t]\+\([^ \t].*\)$/0x\1, \/\/ \2/p'
            0x00100113, // addi     x2,x0,1
            0x11100093, // addi     x1,x0,273
            0x22200093, // addi     x1,x0,546
            0x002081b3, // add      x3,x1,x2
            0x00208233, // add      x4,x1,x2
            0x00300113, // addi     x2,x0,3
            0x11100093, // addi     x1,x0,273
            0x22200093, // addi     x1,x0,546
            0x001102b3, // add      x5,x2,x1
            0x00110333, // add      x6,x2,x1
            0x00000013, // addi     x0,x0,0
            0x00000063, // beq      x0,x0,10080 <loop>
        };
        Registers regs_init;
                regs_init.write_pc(0x200_addr);
                Registers regs_res(regs_init);
                regs_res.write_gp(1, 0x222);
                regs_res.write_gp(2, 3);
                regs_res.write_gp(3, 0x223);
                regs_res.write_gp(4, 0x223);
                regs_res.write_gp(5, 0x225);
                regs_res.write_gp(6, 0x225);
                regs_res.write_pc(regs_init.read_pc() + 4 * code.length() - 4);
                QTest::newRow("alu_forward_1") << code << regs_init << regs_res;
    }

    // Test forwarding in JR and JALR
    {
        QVector<uint32_t> code {
            // start:
            0x01111537, // lui      x10,0x1111
            0x022225b7, // lui      x11,0x2222
            0x03333637, // lui      x12,0x3333
            0x034000ef, // jal      x1,240 <fnc_add3>
            0x00a00733, // add      x14,x0,x10
            0x0140006f, // jal      x0,228 <skip>
            0x44400413, // addi     x8,x0,1092
            0x55500913, // addi     x18,x0,1365
            0x66600993, // addi     x19,x0,1638
            0x77700a13, // addi     x20,x0,1911
            // skip:
            0x00000297, // auipc    x5,0x0
            0x02828293, // addi     x5,x5,40 # 250 <fnc_short>
            0x000280e7, // jalr     x1,0(x5)
            0x2cd00513, // addi     x10,x0,717
            0x00050493, // addi     x9,x10,0 # 1111000 <__global_pointer$+0x1108400>
            0x01c0006f, // jal      x0,258 <test_end>
            // fnc_add:
            0x00b50533, // add      x10,x10,x11
            0x00c50533, // add      x10,x10,x12
            0x00008067, // jalr     x0,0(x1)
            0x7f100693, // addi     x13,x0,2033
            // fnc_short
            0x00008067, // jalr     x0,0(x1)
            0x7f200693, // addi     x13,x0,2034
            // test_end:
            0x00000013, // addi     x0,x0,0
            0x00000013, // addi     x0,x0,0
            // loop:
            0x00000063, // beq      x0,x0,260 <loop>
        };
        Registers regs_init;
        regs_init.write_pc(0x200_addr);
        Registers regs_res(regs_init);
        regs_res.write_gp( 1, 0x00000234);
        regs_res.write_gp( 5, 0x00000250);
        regs_res.write_gp( 9, 0x000002cd);
        regs_res.write_gp(10, 0x000002cd);
        regs_res.write_gp(11, 0x02222000);
        regs_res.write_gp(12, 0x03333000);
        regs_res.write_gp(14, 0x06666000);
        regs_res.write_pc(regs_init.read_pc() + 4 * code.length() - 4);
        QTest::newRow("j_jal_jalr") << code << regs_init << regs_res;
    }

    // Test multiplication and division
    {
        QVector<uint32_t> code {
            0x12345137, // lui      x2,0x12345
            0x67810113, // addi     x2,x2,1656
            0xabcdf1b7, // lui      x3,0xabcdf
            0xf0118193, // addi     x3,x3,-255
            0x02310833, // mul      x16,x2,x3
            0x023118b3, // mulh     x17,x2,x3
            0x02310933, // mul      x18,x2,x3
            0x023139b3, // mulhu    x19,x2,x3
            0x0221ca33, // div      x20,x3,x2
            0x0221eab3, // rem      x21,x3,x2
            0x0221db33, // divu     x22,x3,x2
            0x0221fbb3, // remu     x23,x3,x2
            0x0000006f, // jal      x0,230 <loop>
        };
        Registers regs_init;
        regs_init.write_pc(0x80020000_addr);
        Registers regs_res(regs_init);
        uint32_t val_a = 0x12345678;
        uint32_t val_b = 0xabcdef01;
        uint64_t val_u64;
        int64_t val_s64;
        regs_res.write_gp(2, (int32_t)val_a);
        regs_res.write_gp(3, (int32_t)val_b);
        val_s64 = (int64_t)(int32_t)val_a * (int32_t)val_b;
        regs_res.write_gp(16, (int32_t)(val_s64 & 0xffffffff));
        regs_res.write_gp(17, (int32_t)(val_s64 >> 32));
        val_u64 = (uint64_t)val_a * val_b;
        regs_res.write_gp(18, (int32_t)(val_u64 & 0xffffffff));
        regs_res.write_gp(19, (int32_t)(val_u64 >> 32));
        regs_res.write_gp(20, (int32_t)((int32_t)val_b / (int32_t)val_a));
        regs_res.write_gp(21, (int32_t)((int32_t)val_b % (int32_t)val_a));
        regs_res.write_gp(22, val_b / val_a);
        regs_res.write_gp(23, val_b % val_a);
        regs_res.write_pc(regs_init.read_pc() + 4 * code.length() - 4);
        QTest::newRow("mul-div") << code << regs_init << regs_res;
    }

    // branches
    {
        QVector<uint32_t> code {
            0xfff00093, // addi     x1,x0,-1
            0x00100113, // addi     x2,x0,1
            0x00000193, // addi     x3,x0,0
            0x00200213, // addi     x4,x0,2
            0x00100293, // addi     x5,x0,1
            0x0000ca63, // blt      x1,x0,228 <test_branch+0x18>
            0x00000013, // addi     x0,x0,0
            0x00000293, // addi     x5,x0,0
            0x10018193, // addi     x3,x3,256
            0x001181b3, // add      x3,x3,x1
            0x00100293, // addi     x5,x0,1
            0x00105a63, // bge      x0,x1,240 <test_branch+0x30>
            0x00000013, // addi     x0,x0,0
            0x00000293, // addi     x5,x0,0
            0x01018193, // addi     x3,x3,16
            0x001181b3, // add      x3,x3,x1
            0x00100293, // addi     x5,x0,1
            0x0000da63, // bge      x1,x0,258 <test_branch+0x48>
            0x00000013, // addi     x0,x0,0
            0x00000293, // addi     x5,x0,0
            0x20018193, // addi     x3,x3,512
            0x001181b3, // add      x3,x3,x1
            0x00100293, // addi     x5,x0,1
            0x00104a63, // blt      x0,x1,270 <test_branch+0x60>
            0x00000013, // addi     x0,x0,0
            0x00000293, // addi     x5,x0,0
            0x02018193, // addi     x3,x3,32
            0x001181b3, // add      x3,x3,x1
            0x00100293, // addi     x5,x0,1
            0x00209a63, // bne      x1,x2,288 <test_branch+0x78>
            0x00000013, // addi     x0,x0,0
            0x00000293, // addi     x5,x0,0
            0x40018193, // addi     x3,x3,1024
            0x001181b3, // add      x3,x3,x1
            0x00100293, // addi     x5,x0,1
            0x00208a63, // beq      x1,x2,2a0 <test_branch+0x90>
            0x00000013, // addi     x0,x0,0
            0x00000293, // addi     x5,x0,0
            0x04018193, // addi     x3,x3,64
            0x001181b3, // add      x3,x3,x1
            0x00108093, // addi     x1,x1,1
            0xf64096e3, // bne      x1,x4,210 <test_branch>
            0x00000013, // addi     x0,x0,0
            0x00000013, // addi     x0,x0,0
            0x00000013, // addi     x0,x0,0
            0x00000063, // beq      x0,x0,2b4 <loop>
        };
        Registers regs_init;
        regs_init.write_pc(0x200_addr);
        Registers regs_res(regs_init);
        regs_res.write_gp(1, 2);
        regs_res.write_gp(2, 1);
        regs_res.write_gp(3, 0x8d0);
        regs_res.write_gp(4, 2);
        regs_res.write_gp(5, 1);
        regs_res.write_pc(regs_init.read_pc() + 4 * code.length());
        QTest::newRow("branch_conditions_test") << code << regs_init << regs_res;
    }
}

void TestCore::singlecore_alu_forward_data() {
    core_alu_forward_data();
}

void TestCore::pipecore_alu_forward_data() {
    core_alu_forward_data();
}

void TestCore::pipecorestall_alu_forward_data() {
    core_alu_forward_data();
}

static void run_code_fragment(
    Core &core,
    Registers &reg_init,
    Registers &reg_res,
    Memory &mem_init,
    Memory &mem_res,
    QVector<uint32_t> &code) {

    uint64_t addr = reg_init.read_pc().get_raw();

    foreach (uint32_t i, code) {
        memory_write_u32(&mem_init, addr, i);
        memory_write_u32(&mem_res, addr, i);
        addr += 4;
    }

    for (int k = 10000; k; k--) {
        core.step(); // Single step should be enought as this is risc without
                     // pipeline
        if (reg_init.read_pc() == reg_res.read_pc() && k > 6) { // reached end
                                                                // of
                                                                // the code
                                                                // fragment
            k = 6; // add some cycles to finish processing
        }
    }
    reg_res.write_pc(reg_init.read_pc()); // We do not compare result pc
    QCOMPARE(reg_init, reg_res);          // After doing changes from initial state this
                                          // should be same state as in case of passed
                                          // expected result
    QCOMPARE(mem_init, mem_res);          // There should be no change in memory
}

void TestCore::singlecore_alu_forward() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    Memory mem_init(LITTLE);
    TrivialBus mem_init_frontend(&mem_init);
    Memory mem_res(LITTLE);
    TrivialBus mem_res_frontend(&mem_res);

    BranchPredictor predictor {};
    CSR::ControlState controlst {};

    CoreSingle core(&reg_init, &predictor, &mem_init_frontend, &mem_init_frontend, &controlst, Xlen::_32, config_isa_word_default);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}

void TestCore::pipecore_alu_forward() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    Memory mem_init(LITTLE);
    TrivialBus mem_init_frontend(&mem_init);
    Memory mem_res(LITTLE);
    TrivialBus mem_res_frontend(&mem_res);

    BranchPredictor predictor {};
    CSR::ControlState controlst {};

    CorePipelined core(&reg_init, &predictor, &mem_init_frontend, &mem_init_frontend, &controlst,
                       Xlen::_32, config_isa_word_default, MachineConfig::HazardUnit::HU_STALL_FORWARD);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}

void TestCore::pipecorestall_alu_forward() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    Memory mem_init(LITTLE);
    TrivialBus mem_init_frontend(&mem_init);
    Memory mem_res(LITTLE);
    TrivialBus mem_res_frontend(&mem_res);

    BranchPredictor predictor {};
    CSR::ControlState controlst {};

    CorePipelined core(&reg_init, &predictor, &mem_init_frontend, &mem_init_frontend, &controlst,
                       Xlen::_32, config_isa_word_default, MachineConfig::HazardUnit::HU_STALL);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}

/*======================================================================*/

static void core_memory_tests_data() {
    QTest::addColumn<QVector<uint32_t>>("code");
    QTest::addColumn<Registers>("reg_init");
    QTest::addColumn<Registers>("reg_res");
    QTest::addColumn<Memory>("mem_init");
    QTest::addColumn<Memory>("mem_res");

    // Test
    {
        QVector<uint32_t> code {
            // objdump --disassembler-options=no-aliases,numeric -d test-hazards
            // sed -n -e 's/^[ \t]*[^ \t]\+:[ \t]\+\([0-9a-f]\+\)[ \t]\+\([^ \t].*\)$/0x\1, \/\/ \2/p'

            // _start:
            0x40000513, // addi     x10,x0,1024
            0x00000413, // addi     x8,x0,0
            0x03c00493, // addi     x9,x0,60
            0x00800933, // add      x18,x0,x8
            // main_cycle:
            0x04940a63, // beq      x8,x9,264 <main_cycle_end>
            0x008502b3, // add      x5,x10,x8
            0x0002aa03, // lw       x20,0(x5)
            0x000409b3, // add      x19,x8,x0
            0x00040933, // add      x18,x8,x0
            // inner_cycle:
            0x02990263, // beq      x18,x9,248 <inner_cycle_end>
            0x012502b3, // add      x5,x10,x18
            0x0002aa83, // lw       x21,0(x5)
            0x015a22b3, // slt      x5,x20,x21
            0x00029663, // bne      x5,x0,240 <not_minimum>
            0x00090993, // addi     x19,x18,0
            0x000a8a13, // addi     x20,x21,0
            // not_minimum:
            0x00490913, // addi     x18,x18,4
            0xfe1ff06f, // jal      x0,224 <inner_cycle>
            // inner_cycle_end:
            0x008502b3, // add      x5,x10,x8
            0x0002aa83, // lw       x21,0(x5)
            0x0142a023, // sw       x20,0(x5)
            0x013502b3, // add      x5,x10,x19
            0x0152a023, // sw       x21,0(x5)
            0x00440413, // addi     x8,x8,4
            0xfb1ff06f, // jal      x0,210 <main_cycle>
            // main_cycle_end:
            0x0ff0000f, // fence    iorw,iorw
            0x00000013, // addi     x0,x0,0  // the mai_cycle loop end has
            0x00000013, // addi     x0,x0,0  // to be separated because else fetch
            0x00000013, // addi     x0,x0,0  // reaches stop address prematurely
            0x00000063, // beq      x0,x0,10080 <loop>
        };
        QVector<uint32_t> data_init { 5, 3, 4, 1, 15, 8, 9, 2, 10, 6, 11, 1, 6, 9, 12 };
        QVector<uint32_t> data_res { 1, 1, 2, 3, 4, 5, 6, 6, 8, 9, 9, 10, 11, 12, 15 };
        Registers regs_init;
        regs_init.write_pc(0x200_addr);
        Registers regs_res(regs_init);
        regs_res.write_gp( 5, 0x438);
        regs_res.write_gp( 8,  0x3c);
        regs_res.write_gp( 9,  0x3c);
        regs_res.write_gp(10, 0x400);
        regs_res.write_gp(18,  0x3c);
        regs_res.write_gp(19,  0x38);
        regs_res.write_gp(20,   0xf);
        regs_res.write_gp(21,   0xf);
        regs_res.write_pc(regs_init.read_pc() + 4 * code.length() - 4);
        uint32_t addr;
        Memory mem_init(LITTLE);
        addr = 0x400;
        foreach (uint32_t i, data_init) {
            memory_write_u32(&mem_init, addr, i);
            addr += 4;
        }
        Memory mem_res(LITTLE);
        addr = 0x400;
        foreach (uint32_t i, data_res) {
            memory_write_u32(&mem_res, addr, i);
            addr += 4;
        }
        QTest::newRow("cache_insert_sort") << code << regs_init << regs_res << mem_init << mem_res;
    }

    // unaligned lw a lw and sw
    {
        QVector<uint32_t> code {
            0xaabbdd37, // lui      x26,0xaabbd
            0xcddd0d13, // addi     x26,x26,-803
            0x000d0113, // addi     x2,x26,0
            0x000d0193, // addi     x3,x26,0
            0x000d0213, // addi     x4,x26,0
            0x000d0293, // addi     x5,x26,0
            0x000d0313, // addi     x6,x26,0
            0x000d0393, // addi     x7,x26,0
            0x000d0413, // addi     x8,x26,0
            0x000d0493, // addi     x9,x26,0
            0x000d0513, // addi     x10,x26,0
            0x80020db7, // lui      x27,0x80020
            0x100d8d93, // addi     x27,x27,256
            0x000da103, // lw       x2,0(x27)
            0x001da183, // lw       x3,1(x27)
            0x002da203, // lw       x4,2(x27)
            0x003da283, // lw       x5,3(x27)
            0x01ada023, // sw       x26,0(x27)
            0x01ada2a3, // sw       x26,5(x27)
            0x01ada523, // sw       x26,10(x27)
            0x01ada7a3, // sw       x26,15(x27)
            0x000da503, // lw       x10,0(x27)
            0x004da583, // lw       x11,4(x27)
            0x008da603, // lw       x12,8(x27)
            0x00cda683, // lw       x13,12(x27)
            0x010da703, // lw       x14,16(x27)
            0x014da783, // lw       x15,20(x27)
            0x018da803, // lw       x16,24(x27)
            0x01cda883, // lw       x17,28(x27)
            0x020da903, // lw       x18,32(x27)
            0x020da983, // lw       x19,32(x27)
            0x0ff0000f, // fence    iorw,iorw
            0x00000013, // addi     x0,x0,0
            0x00000013, // addi     x0,x0,0
            0xfe000ce3, // beq      x0,x0,27c <loop>
        };
        Registers regs_init;
        regs_init.write_pc(0x200_addr);
        Registers regs_res(regs_init);

        regs_res.write_gp(2, (int32_t)0x04030201);
        regs_res.write_gp(3, (int32_t)0x05040302);
        regs_res.write_gp(4, (int32_t)0x06050403);
        regs_res.write_gp(5, (int32_t)0x07060504);
        regs_res.write_gp(6, (int32_t)0xaabbccdd);
        regs_res.write_gp(7, (int32_t)0xaabbccdd);
        regs_res.write_gp(8, (int32_t)0xaabbccdd);
        regs_res.write_gp(9, (int32_t)0xaabbccdd);
        regs_res.write_gp(10, (int32_t)0xaabbccdd);
        regs_res.write_gp(11, (int32_t)0xbbccdd05);
        regs_res.write_gp(12, (int32_t)0xccdd0aaa);
        regs_res.write_gp(13, (int32_t)0xdd0faabb);
        regs_res.write_gp(14, (int32_t)0x14aabbcc);
        regs_res.write_gp(15, (int32_t)0x18171615);
        regs_res.write_gp(16, (int32_t)0x1c1b1a19);
        regs_res.write_gp(17, (int32_t)0x101f1e1d);
        regs_res.write_gp(18, (int32_t)0x24232221);
        regs_res.write_gp(19, (int32_t)0x24232221);
        regs_res.write_gp(26, (int32_t)0xaabbccdd);
        regs_res.write_gp(27, (int32_t)0x80020100);

        uint32_t addr;
        Memory mem_init(LITTLE);
        addr = 0x80020100;
        QVector<uint32_t> data_init { 0x04030201, 0x08070605, 0x0c0b0a09, 0x000f0e0d,
                                      0x14131211, 0x18171615, 0x1c1b1a19, 0x101f1e1d,
                                      0x24232221, 0x28272625, 0x2c2b2a29, 0x202f2e2d };
        foreach (uint32_t i, data_init) {
            memory_write_u32(&mem_init, addr, i);
            addr += 4;
        }
        Memory mem_res(LITTLE);
        addr = 0x80020100;
        QVector<uint32_t> data_res { 0xaabbccdd, 0xbbccdd05, 0xccdd0aaa, 0xdd0faabb,
                                     0x14aabbcc, 0x18171615, 0x1c1b1a19, 0x101f1e1d,
                                     0x24232221, 0x28272625, 0x2c2b2a29, 0x202f2e2d };
        foreach (uint32_t i, data_res) {
            memory_write_u32(&mem_res, addr, i);
            addr += 4;
        }

        regs_res.write_pc(regs_init.read_pc() + 4 * code.length() - 4);
        QTest::newRow("lw_sw_unaligned_be") << code << regs_init << regs_res << mem_init << mem_res;
    }
}

void TestCore::singlecore_memory_tests_data() {
    core_memory_tests_data();
}

void TestCore::pipecore_nc_memory_tests_data() {
    core_memory_tests_data();
}

void TestCore::pipecore_wt_na_memory_tests_data() {
    core_memory_tests_data();
}

void TestCore::pipecore_wt_a_memory_tests_data() {
    core_memory_tests_data();
}

void TestCore::pipecore_wb_memory_tests_data() {
    core_memory_tests_data();
}

void TestCore::singlecore_memory_tests() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);
    TrivialBus mem_init_frontend(&mem_init);
    TrivialBus mem_res_frontend(&mem_res);

    BranchPredictor predictor {};
    CSR::ControlState controlst {};

    CoreSingle core(&reg_init, &predictor, &mem_init_frontend, &mem_init_frontend, &controlst, Xlen::_32, config_isa_word_default);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}

void TestCore::pipecore_nc_memory_tests() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);
    TrivialBus mem_init_frontend(&mem_init);
    TrivialBus mem_res_frontend(&mem_res);

    BranchPredictor predictor {};
    CSR::ControlState controlst {};

    CorePipelined core(&reg_init, &predictor, &mem_init_frontend, &mem_init_frontend, &controlst, Xlen::_32, config_isa_word_default);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}

void TestCore::pipecore_wt_na_memory_tests() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);
    TrivialBus mem_init_frontend(&mem_init);
    TrivialBus mem_res_frontend(&mem_res);
    CacheConfig cache_conf;
    cache_conf.set_enabled(true);
    cache_conf.set_set_count(2);     // Number of sets
    cache_conf.set_block_size(1);    // Number of blocks
    cache_conf.set_associativity(2); // Degree of associativity
    cache_conf.set_replacement_policy(CacheConfig::RP_LRU);
    cache_conf.set_write_policy(CacheConfig::WP_THROUGH_NOALLOC);

    Cache i_cache(&mem_init_frontend, &cache_conf);
    Cache d_cache(&mem_init_frontend, &cache_conf);

    BranchPredictor predictor {};
    CSR::ControlState controlst {};

    CorePipelined core(&reg_init, &predictor, &i_cache, &d_cache, &controlst, Xlen::_32, config_isa_word_default);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}

void TestCore::pipecore_wt_a_memory_tests() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);
    TrivialBus mem_init_frontend(&mem_init);
    TrivialBus mem_res_frontend(&mem_res);
    CacheConfig cache_conf;
    cache_conf.set_enabled(true);
    cache_conf.set_set_count(2);     // Number of sets
    cache_conf.set_block_size(1);    // Number of blocks
    cache_conf.set_associativity(2); // Degree of associativity
    cache_conf.set_replacement_policy(CacheConfig::RP_LRU);
    cache_conf.set_write_policy(CacheConfig::WP_THROUGH_ALLOC);
    Cache i_cache(&mem_init_frontend, &cache_conf);
    Cache d_cache(&mem_init_frontend, &cache_conf);

    BranchPredictor predictor {};
    CSR::ControlState controlst {};

    CorePipelined core(&reg_init, &predictor, &i_cache, &d_cache, &controlst, Xlen::_32, config_isa_word_default);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}

void TestCore::pipecore_wb_memory_tests() {
    QFETCH(QVector<uint32_t>, code);
    QFETCH(Registers, reg_init);
    QFETCH(Registers, reg_res);
    QFETCH(Memory, mem_init);
    QFETCH(Memory, mem_res);
    TrivialBus mem_init_frontend(&mem_init);
    TrivialBus mem_res_frontend(&mem_res);
    CacheConfig cache_conf;
    cache_conf.set_enabled(true);
    cache_conf.set_set_count(4);     // Number of sets
    cache_conf.set_block_size(2);    // Number of blocks
    cache_conf.set_associativity(2); // Degree of associativity
    cache_conf.set_replacement_policy(CacheConfig::RP_LRU);
    cache_conf.set_write_policy(CacheConfig::WP_BACK);
    Cache i_cache(&mem_init_frontend, &cache_conf);
    Cache d_cache(&mem_init_frontend, &cache_conf);

    BranchPredictor predictor {};
    CSR::ControlState controlst {};

    CorePipelined core(&reg_init, &predictor, &i_cache, &d_cache, &controlst, Xlen::_32, config_isa_word_default);
    run_code_fragment(core, reg_init, reg_res, mem_init, mem_res, code);
}

void extension_m_data() {
    QTest::addColumn<vector<QString>>("instructions");
    QTest::addColumn<Registers>("registers");
    QTest::addColumn<RegisterValue>("x10_result");
    QTest::addColumn<Xlen>("xlen");

    Registers registers {};
    registers.write_gp(1, 1111111);
    registers.write_gp(2, 7);
    QTest::addRow("mul") << vector<QString> { "mul x10, x1, x2", "nop" } << registers
                         << RegisterValue { 7777777 } << Xlen::_32;
    registers.write_gp(1, 7777777);
    QTest::addRow("div") << vector<QString> { "div x10, x1, x2", "nop" } << registers
                         << RegisterValue { 1111111 } << Xlen::_32;
    registers.write_gp(2, 1000);
    QTest::addRow("rem") << vector<QString> { "rem x10, x1, x2", "nop" } << registers
                         << RegisterValue { 777 } << Xlen::_32;
    registers.write_gp(1, 15);
    registers.write_gp(2, -10);
    QTest::addRow("mulh 15x-10") << vector<QString> { "mulh x10, x1, x2", "nop" } << registers
                                 << RegisterValue { (uint64_t)0xffffffffffffffffULL } << Xlen::_32;
    QTest::addRow("mulhu 15x-10") << vector<QString> { "mulhu x10, x1, x2", "nop" } << registers
                                  << RegisterValue { 14 } << Xlen::_32;
    QTest::addRow("mulhsu 15x-10") << vector<QString> { "mulhsu x10, x1, x2", "nop" } << registers
                                   << RegisterValue { 14 } << Xlen::_32;
    QTest::addRow("mulh -10x15") << vector<QString> { "mulh x10, x2, x1", "nop" } << registers
                                 << RegisterValue { (uint64_t)0xffffffffffffffffULL } << Xlen::_32;
    QTest::addRow("mulhu -10x15") << vector<QString> { "mulhu x10, x2, x1", "nop" } << registers
                                  << RegisterValue { 14 } << Xlen::_32;
    QTest::addRow("mulhsu -10x15")
        << vector<QString> { "mulhsu x10, x2, x1", "nop" } << registers
        << RegisterValue { (uint64_t)0xffffffffffffffffULL } << Xlen::_32;
    registers.write_gp(1, -1);
    registers.write_gp(2, 0x10000);
    QTest::addRow("divu") << vector<QString> { "divu x10, x1, x2", "nop" } << registers
                          << RegisterValue { 0xFFFF } << Xlen::_32;
    QTest::addRow("remu") << vector<QString> { "remu x10, x1, x2", "nop" } << registers
                          << RegisterValue { 0xFFFF } << Xlen::_32;
}

// Extensions:
// =================================================================================================

// RV32M

void TestCore::singlecore_extension_m_data() {
    extension_m_data();
}

void TestCore::pipecore_extension_m_data() {
    extension_m_data();
}

void TestCore::singlecore_extension_m() {
    test_program_with_single_result<CoreSingle>();
}

void TestCore::pipecore_extension_m() {
    test_program_with_single_result<CorePipelined>();
}

QTEST_APPLESS_MAIN(TestCore)
