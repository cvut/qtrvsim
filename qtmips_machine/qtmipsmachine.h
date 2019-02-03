#ifndef QTMIPSMACHINE_H
#define QTMIPSMACHINE_H

#include <QObject>
#include <QTimer>
#include <cstdint>
#include <qtmipsexception.h>
#include <machineconfig.h>
#include <registers.h>
#include <memory.h>
#include <core.h>
#include <cache.h>

namespace machine {

class QtMipsMachine : public QObject {
    Q_OBJECT
public:
    QtMipsMachine(const MachineConfig &cc);

    const MachineConfig &config();
    void set_speed(unsigned);

    const Registers *registers();
    const Memory *memory();
    const Cache *cache_program();
    const Cache *cache_data();
    const Core *core();
    const CoreSingle *core_singe();
    const CorePipelined *core_pipelined();

    enum Status {
        ST_READY, // Machine is ready to be started or step to be called
        ST_RUNNING, // Machine is running
        ST_BUSY, // Machine is calculating step
        ST_EXIT, // Machine exited
        ST_TRAPPED // Machine exited with failure
    };
    enum Status status();
    bool exited();

public slots:
    void play();
    void pause();
    void step();
    void restart();

signals:
    void program_exit();
    void program_trap(machine::QtMipsException &e);
    void status_change(enum machine::QtMipsMachine::Status st);
    void tick(); // Time tick
    void post_tick(); // Emitted after tick to allow updates

private:
    MachineConfig mcnf;

    Registers *regs;
    Memory *mem, *mem_program_only;
    Cache *cch_program, *cch_data;
    Core *cr;

    QTimer *run_t;

    std::uint32_t program_end;
    enum Status stat;
    void set_status(enum Status st);
};

}

#endif // QTMIPSMACHINE_H
