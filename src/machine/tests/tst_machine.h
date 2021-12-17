#ifndef TST_MACHINE_H
#define TST_MACHINE_H

#include <QtTest/QTest>
#include <cstdint>

class MachineTests : public QObject {
Q_OBJECT
private Q_SLOTS:
    // Test utils
    // Registers
    // Memory
    // Program loader
    // Instruction
    // Cache
    // Core depends on cache, therefore it is tested first.
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
