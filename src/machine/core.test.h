#ifndef CORE_TEST_H
#define CORE_TEST_H

#include <QtTest>

class TestCore : public QObject {
    Q_OBJECT

private slots:
    // Not ported to RV:
    void singlecore_regs();
    void singlecore_regs_data();
    void pipecore_regs();
    void pipecore_regs_data();
    void singlecore_jmp();
    void singlecore_jmp_data();
    void pipecore_jmp();
    void pipecore_jmp_data();
    void singlecore_mem_data();
    void singlecore_mem();
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

    // Extensions:
    // =============================================================================================

    // RV32M
    void singlecore_extension_m_data();
    void pipecore_extension_m_data();
    void singlecore_extension_m();
    void pipecore_extension_m();
};

#endif // CORE_TEST_H
