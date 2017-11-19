#include "tracer.h"
#include <iostream>

using namespace std;

Tracer::Tracer(QtMipsMachine *machine) {
    this->machine = machine;
}

void Tracer::reg_pc() {
    connect(machine->registers(), SIGNAL(pc_update(std::uint32_t)), this, SLOT(regs_pc_update(std::uint32_t)));
}

void Tracer::regs_pc_update(std::uint32_t val) {
    cout << "PC:" << hex << val << endl;
}
