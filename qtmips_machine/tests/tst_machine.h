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
    // Memory
    void memory();
    void memory_data();
    void memory_section();
    void memory_section_data();
    void memory_endian();
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
    void core_regs();
    void core_regs_data();
    void core_mem();
    void core_mem_data();
};

#endif // TST_MACHINE_H
