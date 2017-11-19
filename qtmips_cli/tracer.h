#ifndef TRACER_H
#define TRACER_H

#include <QObject>
#include "qtmipsmachine.h"

class Tracer : public QObject {
    Q_OBJECT
public:
    Tracer(QtMipsMachine *machine);

    // Trace registers
    void reg_pc();

private slots:
    void regs_pc_update(std::uint32_t val);

private:
    QtMipsMachine *machine;
};

#endif // TRACER_H
