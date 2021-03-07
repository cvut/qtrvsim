#ifndef TRACER_H
#define TRACER_H

#include "machine/instruction.h"
#include "machine/machine.h"
#include "machine/memory/address.h"

#include <QObject>

class Tracer : public QObject {
    Q_OBJECT
public:
    Tracer(machine::Machine *machine);

    // Trace instructions in different stages/sections
    void fetch();
    void decode();
    void execute();
    void memory();
    void writeback();
    // Trace registers
    void reg_pc();
    void reg_gp(machine::RegisterId i);
    void reg_lo();
    void reg_hi();

private slots:
    void instruction_fetch(
        const machine::Instruction &inst,
        machine::Address inst_addr,
        machine::ExceptionCause excause,
        bool valid);
    void instruction_decode(
        const machine::Instruction &inst,
        machine::Address inst_addr,
        machine::ExceptionCause excause,
        bool valid);
    void instruction_execute(
        const machine::Instruction &inst,
        machine::Address inst_addr,
        machine::ExceptionCause excause,
        bool valid);
    void instruction_memory(
        const machine::Instruction &inst,
        machine::Address inst_addr,
        machine::ExceptionCause excause,
        bool valid);
    void instruction_writeback(
        const machine::Instruction &inst,
        machine::Address inst_addr,
        machine::ExceptionCause excause,
        bool valid);

    void regs_pc_update(machine::Address val);
    void regs_gp_update(machine::RegisterId i, machine::RegisterValue val);
    void regs_hi_lo_update(bool hi, machine::RegisterValue val) const;

private:
    machine::Machine *machine;

    bool gp_regs[32] {};
    bool r_hi, r_lo;

    bool con_fetch {}, con_decode {}, con_execute {}, con_memory {},
        con_writeback {}, con_regs_pc, con_regs_gp, con_regs_hi_lo;
};

#endif // TRACER_H
