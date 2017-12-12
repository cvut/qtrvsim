#ifndef TST_MACHINE_H
#define TST_MACHINE_H

#include <QtTest>

class MachineTests : public QObject {
    Q_OBJECT
private Q_SLOTS:
    // Registers
    void registers_gp0();
    void registers_rw_gp();
    void registers_rw_hi_lo();
    void registers_pc();
    void registers_compare();
    // Memory
    void memory();
    void memory_data();
    void memory_section();
    void memory_section_data();
    void memory_endian();
    void memory_compare();
    void memory_write_ctl();
    void memory_write_ctl_data();
    void memory_read_ctl();
    void memory_read_ctl_data();
    // Program loader
    void program_loader();
    // Instruction
    void instruction();
    void instruction_access();
    // Alu
    void alu();
    void alu_data();
    void alu_except();
    void alu_except_data();
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
};

#endif // TST_MACHINE_H
