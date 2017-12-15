#ifndef QTMIPSMACHINE_H
#define QTMIPSMACHINE_H

#include <QObject>
#include <QTimer>
#include <cstdint>
#include "qtmipsexception.h"
#include "machineconfig.h"
#include "registers.h"
#include "memory.h"
#include "core.h"
#include "cache.h"

class QtMipsMachine : public QObject {
    Q_OBJECT
public:
    QtMipsMachine(const MachineConfig &cc);

    void set_speed(unsigned);

    const Registers *registers();
    const Memory *memory();
    const Cache *cache();
    const Core *core();

public slots:
    // TODO handle speed
    void play();
    void pause();
    void step();
    void restart();

signals:
    void program_exit();
    void tick(); // Time tick

private:
    Registers *regs;
    Memory *mem;
    Cache *cch;
    Core *cr;

    unsigned run_speed;
    QTimer *run_t;

    std::uint32_t program_end;
    bool program_ended;
};

#endif // QTMIPSMACHINE_H
