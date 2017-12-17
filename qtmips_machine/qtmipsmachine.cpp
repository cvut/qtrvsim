#include "qtmipsmachine.h"
#include "programloader.h"

using namespace machine;

QtMipsMachine::QtMipsMachine(const MachineConfig &cc) {
    ProgramLoader program(cc.elf());

    regs = new Registers();
    mem = new Memory();

    program.to_memory(mem);
    program_end = program.end();
    program_ended = false;

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
    default:
        throw QTMIPS_EXCEPTION(Sanity, "Trying to configure unknown cache type", "");
    }

    // TODO pipelined
    cr = new CoreSingle(regs, coremem);

    run_speed = 1;
    run_t = new QTimer(this);
    connect(run_t, SIGNAL(timeout()), this, SLOT(step()));
}

void QtMipsMachine::set_speed(unsigned val) {
    run_speed = val;
    if (run_t->isActive())
        play();
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
    if (program_ended)
        return;
    run_t->start(run_speed);
}

void QtMipsMachine::pause() {
    if (program_ended)
        return;
    run_t->stop();
}

void QtMipsMachine::step() {
    if (program_ended) // Ignore if program ended
        return;
    emit tick();
    cr->step();
    if (regs->read_pc() >= program_end) {
        program_ended = true;
        run_t->stop();
        emit program_exit();
    }
}

void QtMipsMachine::restart() {
    if (!program_ended)
        run_t->stop(); // Stop timer if program is still running
    // TODO
}
