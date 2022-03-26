#include "controlstate.h"

#include "core.h"
#include "machinedefs.h"
#include "simulator_exception.h"

using namespace machine;

#define COUNTER_IRQ_LEVEL 7

// sorry, unimplemented: non-trivial designated initializers not supported

static enum ControlState::CsrRegisters csrRegMap[32][8] = {
    /*0*/ {},
    /*1*/ {},
    /*2*/ {},
    /*3*/ {},
    /*4*/ { ControlState::Unsupported, ControlState::Unsupported},
    /*5*/ {},
    /*6*/ {},
    /*7*/ {},
    /*8*/ { ControlState::mtval },
    /*9*/ { ControlState::mcycle },
    /*10*/ {},
    /*11*/ { ControlState::Compare },
    /*12*/ { ControlState::mstatus },
    /*13*/ { ControlState::mcause },
    /*14*/ { ControlState::mepc },
    /*15*/ { ControlState::Unsupported, ControlState::mtvec },
    /*17*/ {},
    /*18*/ {},
    /*19*/ {},
    /*20*/ {},
    /*21*/ {},
    /*22*/ {},
    /*23*/ {},
    /*24*/ {},
    /*25*/ {},
    /*26*/ {},
    /*27*/ {},
    /*28*/ {},
    /*29*/ {},
    /*30*/ {},
    /*31*/ {},
};

// sorry, unimplemented: non-trivial designated initializers not supported

const ControlState::csrRegDesc_t ControlState::csrRegDesc[ControlState::CSR_REGS_CNT] = {
    [ControlState::Unsupported] = {
        "Unsupported", 0x00000000, 0x00000000,
        &ControlState::read_csr_default,
        &ControlState::write_csr_default },
    [ControlState::mstatus] = {
        "mstatus", Status_IE | Status_IntMask,
        0x00000000, &ControlState::read_csr_default,
        &ControlState::write_csr_default },
    [ControlState::mtvec] = {
        "mtvec", 0xfffffffffffffffc, 0x00000100,
        &ControlState::read_csr_default,
        &ControlState::write_csr_default },
    [ControlState::mepc] = {
        "mepc", 0xffffffffffffffff, 0x00000000,
        &ControlState::read_csr_default,
        &ControlState::write_csr_default },
    [ControlState::mcause] = {
        "mcause", 0x00000000, 0x00000000,
        &ControlState::read_csr_default,
        &ControlState::write_csr_default },
    [ControlState::mtval] = {
        "mtval", 0x00000000, 0x00000000,
        &ControlState::read_csr_default,
        &ControlState::write_csr_default },
    [ControlState::mcycle] = {
        "mcycle", 0xffffffff, 0x00000000,
        &ControlState::read_csr_default,
        &ControlState::write_csr_count_compare },
    [ControlState::Compare] = {
        "Compare", 0xffffffff, 0x00000000,
        &ControlState::read_csr_default,
        &ControlState::write_csr_count_compare },
};

ControlState::ControlState(Core *core) : QObject() {
    this->core = core;
    reset();
}

ControlState::ControlState(const ControlState &orig) : QObject() {
    this->core = orig.core;
    for (int i = 0; i < CSR_REGS_CNT; i++) {
        this->csr_data[i] = orig.read_csr((enum CsrRegisters)i);
    }
}

void ControlState::setup_core(Core *core) {
    this->core = core;
}

uint64_t ControlState::read_csr(uint32_t rd, uint8_t sel) const {
    SANITY_ASSERT(
        rd < 32, QString("Trying to read from CSR register ") + QString::number(rd) + ','
                     + QString::number(sel));
    SANITY_ASSERT(
        sel < 8, QString("Trying to read from CSR register ") + QString::number(rd) + ','
                     + QString::number(sel));
    enum CsrRegisters reg = csrRegMap[rd][sel];
    SANITY_ASSERT(
        reg != 0, QString("CSR register ") + QString::number(rd) + ',' + QString::number(sel)
                      + "unsupported");
    return read_csr(reg);
}

void ControlState::write_csr(uint32_t rd, uint8_t sel, RegisterValue value) {
    SANITY_ASSERT(
        rd < 32, QString("Trying to write to CSR register ") + QString::number(rd) + ','
                     + QString::number(sel));
    SANITY_ASSERT(
        sel < 8, QString("Trying to write to CSR register ") + QString::number(rd) + ','
                     + QString::number(sel));
    enum CsrRegisters reg = csrRegMap[rd][sel];
    SANITY_ASSERT(
        reg != 0, QString("CSR register ") + QString::number(rd) + ',' + QString::number(sel)
                      + "unsupported");
    write_csr(reg, value);
}

uint64_t ControlState::read_csr(enum CsrRegisters reg) const {
    SANITY_ASSERT(
        reg != Unsupported && reg < CSR_REGS_CNT,
        QString("Trying to read from CSR register ") + QString::number(reg));
    return (this->*csrRegDesc[reg].reg_read)(reg);
}

void ControlState::write_csr(enum CsrRegisters reg, RegisterValue value) {
    SANITY_ASSERT(
        reg != Unsupported && reg < CSR_REGS_CNT,
        QString("Trying to write to CSR register ") + QString::number(reg));
    (this->*csrRegDesc[reg].reg_write)(reg, value.as_u32());
}

QString ControlState::csr_name(enum CsrRegisters reg) {
    return QString(csrRegDesc[(int)reg].name);
}

uint64_t ControlState::read_csr_default(enum CsrRegisters reg) const {
    uint64_t val;
    val = csr_data[(int)reg];
    emit csr_read(reg, val);
    return val;
}

void ControlState::write_csr_default(enum CsrRegisters reg, uint64_t value) {
    uint64_t mask = csrRegDesc[(int)reg].write_mask;
    csr_data[(int)reg] = (value & mask) | (csr_data[(int)reg] & ~mask);
    emit csr_update(reg, csr_data[(int)reg]);
}

bool ControlState::operator==(const ControlState &c) const {
    for (int i = 0; i < CSR_REGS_CNT; i++) {
        if (read_csr((enum CsrRegisters)i)
            != c.read_csr((enum CsrRegisters)i)) {
            return false;
        }
    }
    return true;
}

bool ControlState::operator!=(const ControlState &c) const {
    return !this->operator==(c);
}

void ControlState::reset() {
    for (int i = 1; i < CSR_REGS_CNT; i++) {
        this->csr_data[i] = csrRegDesc[i].init_value;
        emit csr_update((enum CsrRegisters)i, csr_data[i]);
    }
    last_core_cycles = 0;
}

void ControlState::update_execption_cause(enum ExceptionCause excause) {
    csr_data[(int)mcause] &= ~0x80000000;
    csr_data[(int)mcause] &= ~0x0000007f;
    if (excause != EXCAUSE_INT) { csr_data[(int)mcause] |= (int)excause << 2; }
    emit csr_update(mcause, csr_data[(int)mcause]);
}

void ControlState::set_interrupt_signal(uint irq_num, bool active) {
    uint64_t mask;
    if (irq_num >= 8) {
        return;
    }
    mask = Status_Int0 << irq_num;
    if (active) {
        csr_data[(int)mcause] |= mask;
    } else {
        csr_data[(int)mcause] &= ~mask;
    }
    emit csr_update(mcause, csr_data[(int)mcause]);
}

bool ControlState::core_interrupt_request() {
    uint64_t irqs;

    update_count_and_compare_irq();

    irqs = csr_data[(int)mstatus];
    irqs &= csr_data[(int)mcause];
    irqs &= Status_IntMask;

    return irqs && csr_data[(int)mstatus] & Status_IntMask
           && !(csr_data[(int)mstatus] & Status_EXL)
           && !(csr_data[(int)mstatus] & Status_ERL);
}

void ControlState::set_status_exl(bool value) {
    if (value) {
        csr_data[(int)mstatus] |= Status_EXL;
    } else {
        csr_data[(int)mstatus] &= ~Status_EXL;
    }
    emit csr_update(mstatus, csr_data[(int)mstatus]);
}

Address ControlState::exception_pc_address() {
    return Address(csr_data[(int)mtvec]);
}

void ControlState::write_csr_count_compare(
    enum CsrRegisters reg,
    uint64_t value) {
    set_interrupt_signal(COUNTER_IRQ_LEVEL, false);
    write_csr_default(reg, value);
}

void ControlState::update_count_and_compare_irq() {
    uint64_t core_cycles;
    uint64_t count_orig;
    if (core == nullptr) {
        return;
    }
    count_orig = csr_data[(int)mcycle];
    core_cycles = core->get_cycle_count();
    csr_data[(int)mcycle] += core_cycles - last_core_cycles;
    last_core_cycles = core_cycles;
    emit csr_update(mcycle, csr_data[(int)mcycle]);

    if ((int32_t)(csr_data[(int)Compare] - count_orig) > 0
        && (int32_t)(csr_data[(int)Compare] - csr_data[(int)mcycle]) <= 0) {
        set_interrupt_signal(COUNTER_IRQ_LEVEL, true);
    }
}
