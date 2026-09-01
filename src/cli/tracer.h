#ifndef TRACER_H
#define TRACER_H

#include "machine/instruction.h"
#include "machine/machine.h"
#include "machine/memory/address.h"
#include "machine/registers.h"

#include <QObject>

/**
 * Watches the step by step execution of the machine and prints requested state.
 */
class Tracer final : public QObject {
    Q_OBJECT
public:
    explicit Tracer(machine::Machine *machine);
    void start();

private slots:
    void step_output();

private:
    const machine::Core *core;
    const machine::CoreState &core_state;
    machine::CSR::PrivilegeLevel last_priv_lev;
    bool started = false;

public:
    std::array<bool, machine::REGISTER_COUNT> regs_to_trace = {};
    bool trace_fetch = false, trace_decode = false, trace_execute = false, trace_memory = false,
         trace_writeback = false, trace_pc = false, trace_wrmem = false, trace_rdmem = false,
         trace_regs_gp = false, trace_exception = false, trace_mode_change = false;
};

#endif // TRACER_H
