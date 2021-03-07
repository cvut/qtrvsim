#ifndef TST_MACHINE_H
#define TST_MACHINE_H

#include <QtTest/QTest>
#include <cstdint>

class MachineTests : public QObject {
Q_OBJECT
private Q_SLOTS:
    // Test utils
    static void integer_decomposition();
    // Registers
    void registers_gp0();
    void registers_rw_gp();
    void registers_rw_hi_lo();
    void registers_pc();
    void registers_compare();
    // Memory
    static void memory();
    static void memory_data();
    static void memory_section();
    static void memory_section_data();
    void memory_compare();
    void memory_compare_data();
    static void memory_write_ctl_data();
    static void memory_write_ctl();
    static void memory_read_ctl_data();
    static void memory_read_ctl();
    // Program loader
    void program_loader();
    // Instruction
    void instruction();
    void instruction_access();
    // Alu
    void alu();
    void alu_data();
    void alu_trap_overflow();
    void alu_trap_overflow_data();
    void alu_except();
    void alu_except_data();
    // Cache
    // Core depends on cache, therefore it is tested first.
    static void cache_data();
    static void cache();
    static void cache_correctness_data();
    static void cache_correctness();
    // Core
    void singlecore_regs();
    void singlecore_regs_data();
    void pipecore_regs();
    void pipecore_regs_data();
    void singlecore_jmp();
    void singlecore_jmp_data();
    void pipecore_jmp();
    void pipecore_jmp_data();
    void singlecore_mem();
    void singlecore_mem_data();
    void pipecore_mem();
    void pipecore_mem_data();
    void singlecore_alu_forward();
    void singlecore_alu_forward_data();
    void pipecore_alu_forward();
    void pipecore_alu_forward_data();
    void pipecorestall_alu_forward();
    void pipecorestall_alu_forward_data();
    void singlecore_memory_tests_data();
    void pipecore_nc_memory_tests_data();
    void pipecore_wt_na_memory_tests_data();
    void pipecore_wt_a_memory_tests_data();
    void pipecore_wb_memory_tests_data();
    void singlecore_memory_tests();
    void pipecore_nc_memory_tests();
    void pipecore_wt_na_memory_tests();
    void pipecore_wt_a_memory_tests();
    void pipecore_wb_memory_tests();
};

#endif // TST_MACHINE_H
