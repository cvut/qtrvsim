#include "qtmipsmachine.h"
#include "programloader.h"

using namespace machine;

QtMipsMachine::QtMipsMachine(const MachineConfig &cc) : QObject(), mcnf(&cc) {
    stat = ST_READY;

    ProgramLoader program(cc.elf());
    mem_program_only = new Memory();
    program.to_memory(mem_program_only);
    program_end = program.end();

    regs = new Registers();
    mem = new Memory(*mem_program_only);
    cch_program = new Cache(mem, &cc.cache_program(), cc.memory_access_time_read(), cc.memory_access_time_write());
    cch_data = new Cache(mem, &cc.cache_data(), cc.memory_access_time_read(), cc.memory_access_time_write());

    if (cc.pipelined())
        cr = new CorePipelined(regs, cch_program, cch_data, cc.hazard_unit());
    else
        cr = new CoreSingle(regs, cch_program, cch_data, cc.delay_slot());

    run_t = new QTimer(this);
    set_speed(0); // In default run as fast as possible
    connect(run_t, SIGNAL(timeout()), this, SLOT(step()));
}

const MachineConfig &QtMipsMachine::config() {
    return mcnf;
}

void QtMipsMachine::set_speed(unsigned val) {
    run_t->setInterval(val);
}

const Registers *QtMipsMachine::registers() {
    return regs;
}

const Memory *QtMipsMachine::memory() {
    return mem;
}

const Cache *QtMipsMachine::cache_program() {
    return cch_program;
}

const Cache *QtMipsMachine::cache_data() {
    return cch_data;
}

const Core *QtMipsMachine::core() {
    return cr;
}

const CoreSingle *QtMipsMachine::core_singe() {
    return mcnf.pipelined() ? nullptr : (const CoreSingle*)cr;
}

const CorePipelined *QtMipsMachine::core_pipelined() {
    return mcnf.pipelined() ? (const CorePipelined*)cr : nullptr;
}

enum QtMipsMachine::Status QtMipsMachine::status() {
    return stat;
}

bool QtMipsMachine::exited() {
    return stat == ST_EXIT || stat == ST_TRAPPED;
}

// We don't allow to call control methods when machine exited or if it's busy
// We rather silently fail.
#define CTL_GUARD do { if (exited() || stat == ST_BUSY) return; } while(false)

void QtMipsMachine::play() {
    CTL_GUARD;
    set_status(ST_RUNNING);
    run_t->start();
}

void QtMipsMachine::pause() {
    if (stat != ST_BUSY)
        CTL_GUARD;
    set_status(ST_READY);
    run_t->stop();
}

void QtMipsMachine::step() {
    CTL_GUARD;
    enum Status stat_prev = stat;
    set_status(ST_BUSY);
    emit tick();
    try {
        cr->step();
    } catch (QtMipsException &e) {
        run_t->stop();
        set_status(ST_TRAPPED);
        emit program_trap(e);
        return;
    }
    if (regs->read_pc() >= program_end) {
        run_t->stop();
        set_status(ST_EXIT);
        emit program_exit();
    } else {
        if (stat == ST_BUSY)
            set_status(stat_prev);
    }
}

void QtMipsMachine::restart() {
    pause();
    regs->reset();
    mem->reset(*mem_program_only);
    cch_program->reset();
    cch_data->reset();
    cr->reset();
    set_status(ST_READY);
}

void QtMipsMachine::set_status(enum Status st) {
    bool change = st != stat;
    stat = st;
    if (change)
        emit status_change(st);
}
