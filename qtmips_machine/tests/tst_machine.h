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
    // ProgramMemory
    void program_memory();
    void program_memory_data();
};

#endif // TST_MACHINE_H
