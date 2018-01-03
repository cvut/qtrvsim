#include "tracer.h"
#include <iostream>
#include <qtmipsexception.h>

using namespace std;
using namespace machine;

Tracer::Tracer(QtMipsMachine *machine) {
    this->machine = machine;
    for (unsigned i = 0; i < 32; i++)
        gp_regs[i] = false;
    r_hi = false;
    r_lo = false;

    con_regs_pc = false;
    con_regs_gp = false;
    con_regs_hi_lo = false;
}

#define CON(VAR, FROM, SIG, SLT) do { \
        if (!VAR) { \
            connect(FROM, SIGNAL(SIG), this, SLOT(SLT)); \
            VAR = true;\
        }\
    } while(false)

void Tracer::fetch() {
    CON(con_fetch, machine->core(), instruction_fetched(machine::Instruction&), instruction_fetch(machine::Instruction&));
}

void Tracer::reg_pc() {
    CON(con_regs_pc, machine->registers(), pc_update(std::uint32_t), regs_pc_update(std::uint32_t));
}

void Tracer::reg_gp(std::uint8_t i) {
    SANITY_ASSERT(i <= 32, "Trying to trace invalid gp.");
    CON(con_regs_gp, machine->registers(), gp_update(std::uint8_t,std::uint32_t), regs_gp_update(std::uint8_t,std::uint32_t));
    gp_regs[i] = true;
}

void Tracer::reg_lo() {
    CON(con_regs_hi_lo, machine->registers(), hi_lo_update(bool hi, std::uint32_t val), regs_hi_lo_update(bool hi, std::uint32_t val));
    r_lo = true;
}

void Tracer::reg_hi() {
    CON(con_regs_hi_lo, machine->registers(), hi_lo_update(bool hi, std::uint32_t val), regs_hi_lo_update(bool hi, std::uint32_t val));
    r_hi = true;
}

void Tracer::instruction_fetch(Instruction &inst) {
    cout << inst.to_str().toStdString() << endl;
}

void Tracer::regs_pc_update(std::uint32_t val) {
    cout << "PC:" << hex << val << endl;
}

void Tracer::regs_gp_update(std::uint8_t i, std::uint32_t val) {
    if (gp_regs[i])
        cout << "GP" << dec << (unsigned)i << ":" << hex << val << endl;
}

void Tracer::regs_hi_lo_update(bool hi, std::uint32_t val) {
    if (hi && r_hi)
        cout << "HI:" << hex << val << endl;
    else if (!hi && r_lo)
        cout << "LO:" << hex << val << endl;
}
