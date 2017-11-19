#include "qtmipsmachine.h"
#include "programloader.h"

QtMipsMachine::QtMipsMachine(const MachineConfig &cc) {
    ProgramLoader program(cc.elf());

    regs = new Registers();
    mem = new Memory();

    program.to_memory(mem);
    program_end = program.end();

    MemoryAccess *coremem;
    switch (cc.cache()) {
    case MachineConfig::CCT_NONE:
        cch = nullptr;
        coremem = mem;
        break;
    case MachineConfig::CCT_ASSOCIATIVE:
        // TODO
        coremem = mem;
        //coremem = cch = new CacheAssociative();
        break;
    }

    // TODO pipelined
    cr = new CoreSingle(regs, coremem);

    run_speed = 1;
    run_t = new QTimer(this);
    connect(run_t, SIGNAL(timeout()), this, SLOT(step()));
}

void QtMipsMachine::set_speed(unsigned val) {
    run_speed = val;
}

const Registers *QtMipsMachine::registers() {
    return regs;
}

const Memory *QtMipsMachine::memory() {
    return mem;
}

const Cache *QtMipsMachine::cache() {
    return cch;
}

const Core *QtMipsMachine::core() {
    return cr;
}

void QtMipsMachine::play() {
    run_t->start(run_speed);
}

void QtMipsMachine::pause() {
    run_t->stop();
}

void QtMipsMachine::step() {
    cr->step();
    if (regs->read_pc() >= program_end)
        emit program_exit();
}

void QtMipsMachine::restart() {
    // TODO
}
