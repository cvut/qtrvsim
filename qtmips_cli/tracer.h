#ifndef TRACER_H
#define TRACER_H

#include <QObject>
#include "qtmipsmachine.h"

class Tracer : public QObject {
    Q_OBJECT
public:
    Tracer(machine::QtMipsMachine *machine);

    // Trace registers
    void reg_pc();
    void reg_gp(std::uint8_t i);
    void reg_lo();
    void reg_hi();

private slots:
    void regs_pc_update(std::uint32_t val);
    void regs_gp_update(std::uint8_t i, std::uint32_t val);
    void regs_hi_lo_update(bool hi, std::uint32_t val);

private:
    machine::QtMipsMachine *machine;

    bool gp_regs[32];
    bool r_hi, r_lo;

    bool con_regs_pc, con_regs_gp, con_regs_hi_lo;
};

#endif // TRACER_H
