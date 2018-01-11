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
    CON(con_fetch, machine->core(), instruction_fetched(const machine::Instruction&), instruction_fetch(const machine::Instruction&));
}

void Tracer::decode() {
    CON(con_fetch, machine->core(), instruction_decoded(const machine::Instruction&), instruction_decode(const machine::Instruction&));
}

void Tracer::execute() {
    CON(con_fetch, machine->core(), instruction_executed(const machine::Instruction&), instruction_execute(const machine::Instruction&));
}

void Tracer::memory() {
    CON(con_fetch, machine->core(), instruction_memory(const machine::Instruction&), instruction_memory(const machine::Instruction&));
}

void Tracer::writeback() {
    CON(con_fetch, machine->core(), instruction_writeback(const machine::Instruction&), instruction_writeback(const machine::Instruction&));
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

void Tracer::instruction_fetch(const Instruction &inst) {
    cout << "Fetch: " << inst.to_str().toStdString() << endl;
}

void Tracer::instruction_decode(const machine::Instruction &inst) {
    cout << "Decode: " << inst.to_str().toStdString() << endl;
}

void Tracer::instruction_execute(const machine::Instruction &inst) {
    cout << "Execute: " << inst.to_str().toStdString() << endl;
}

void Tracer::instruction_memory(const machine::Instruction &inst) {
    cout << "Memory: " << inst.to_str().toStdString() << endl;
}

void Tracer::instruction_writeback(const machine::Instruction &inst) {
    cout << "Writeback: " << inst.to_str().toStdString() << endl;
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
