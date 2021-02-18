// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

#include "cop0state.h"

#include "core.h"
#include "machinedefs.h"
#include "qtmipsexception.h"

using namespace machine;

#define COUNTER_IRQ_LEVEL 7

// sorry, unimplemented: non-trivial designated initializers not supported

static enum Cop0State::Cop0Registers cop0reg_map[32][8] = {
    /*0*/ {},
    /*1*/ {},
    /*2*/ {},
    /*3*/ {},
    /*4*/ { Cop0State::Unsupported, Cop0State::Unsupported, Cop0State::UserLocal },
    /*5*/ {},
    /*6*/ {},
    /*7*/ {},
    /*8*/ { Cop0State::BadVAddr },
    /*9*/ { Cop0State::Count },
    /*10*/ {},
    /*11*/ { Cop0State::Compare },
    /*12*/ { Cop0State::Status },
    /*13*/ { Cop0State::Cause },
    /*14*/ { Cop0State::EPC },
    /*15*/ { Cop0State::Unsupported, Cop0State::EBase },
    /*16*/ { Cop0State::Config },
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

const Cop0State::cop0reg_desc_t Cop0State::cop0reg_desc[Cop0State::COP0REGS_CNT]
    = {
          [Cop0State::Unsupported] = { "Unsupported", 0x00000000, 0x00000000,
                                       &Cop0State::read_cop0reg_default,
                                       &Cop0State::write_cop0reg_default },
          [Cop0State::UserLocal] = { "UserLocal", 0xffffffff, 0x00000000,
                                     &Cop0State::read_cop0reg_default,
                                     &Cop0State::write_cop0reg_user_local },
          [Cop0State::BadVAddr] = { "BadVAddr", 0x00000000, 0x00000000,
                                    &Cop0State::read_cop0reg_default,
                                    &Cop0State::write_cop0reg_default },
          [Cop0State::Count]
          = { "Count", 0xffffffff, 0x00000000, &Cop0State::read_cop0reg_default,
              &Cop0State::write_cop0reg_count_compare },
          [Cop0State::Compare] = { "Compare", 0xffffffff, 0x00000000,
                                   &Cop0State::read_cop0reg_default,
                                   &Cop0State::write_cop0reg_count_compare },
          [Cop0State::Status] = { "Status", Status_IE | Status_IntMask,
                                  0x00000000, &Cop0State::read_cop0reg_default,
                                  &Cop0State::write_cop0reg_default },
          [Cop0State::Cause]
          = { "Cause", 0x00000000, 0x00000000, &Cop0State::read_cop0reg_default,
              &Cop0State::write_cop0reg_default },
          [Cop0State::EPC]
          = { "EPC", 0xffffffff, 0x00000000, &Cop0State::read_cop0reg_default,
              &Cop0State::write_cop0reg_default },
          [Cop0State::EBase]
          = { "EBase", 0xfffffffc, 0x80000000, &Cop0State::read_cop0reg_default,
              &Cop0State::write_cop0reg_default },
          [Cop0State::Config] = { "Config", 0x00000000, 0x00000000,
                                  &Cop0State::read_cop0reg_default,
                                  &Cop0State::write_cop0reg_default },
      };

Cop0State::Cop0State(Core *core) : QObject() {
    this->core = core;
    reset();
}

Cop0State::Cop0State(const Cop0State &orig) : QObject() {
    this->core = orig.core;
    for (int i = 0; i < COP0REGS_CNT; i++) {
        this->cop0reg[i] = orig.read_cop0reg((enum Cop0Registers)i);
    }
}

void Cop0State::setup_core(Core *core) {
    this->core = core;
}

uint32_t Cop0State::read_cop0reg(uint8_t rd, uint8_t sel) const {
    SANITY_ASSERT(
        rd < 32, QString("Trying to read from cop0 register ") + QString(rd)
                     + ',' + QString(sel));
    SANITY_ASSERT(
        sel < 8, QString("Trying to read from cop0 register ") + QString(rd)
                     + ',' + QString(sel));
    enum Cop0Registers reg = cop0reg_map[rd][sel];
    SANITY_ASSERT(
        reg != 0, QString("Cop0 register ") + QString(rd) + ',' + QString(sel)
                      + "unsupported");
    return read_cop0reg(reg);
}

void Cop0State::write_cop0reg(uint8_t rd, uint8_t sel, RegisterValue value) {
    SANITY_ASSERT(
        rd < 32, QString("Trying to write to cop0 register ") + QString(rd)
                     + ',' + QString(sel));
    SANITY_ASSERT(
        sel < 8, QString("Trying to write to cop0 register ") + QString(rd)
                     + ',' + QString(sel));
    enum Cop0Registers reg = cop0reg_map[rd][sel];
    SANITY_ASSERT(
        reg != 0, QString("Cop0 register ") + QString(rd) + ',' + QString(sel)
                      + "unsupported");
    write_cop0reg(reg, value);
}

uint32_t Cop0State::read_cop0reg(enum Cop0Registers reg) const {
    SANITY_ASSERT(
        reg != Unsupported && reg < COP0REGS_CNT,
        QString("Trying to read from cop0 register ") + QString(reg));
    return (this->*cop0reg_desc[reg].reg_read)(reg);
}

void Cop0State::write_cop0reg(enum Cop0Registers reg, RegisterValue value) {
    SANITY_ASSERT(
        reg != Unsupported && reg < COP0REGS_CNT,
        QString("Trying to write to cop0 register ") + QString(reg));
    (this->*cop0reg_desc[reg].reg_write)(reg, value.as_u32());
}

QString Cop0State::cop0reg_name(enum Cop0Registers reg) {
    return QString(cop0reg_desc[(int)reg].name);
}

uint32_t Cop0State::read_cop0reg_default(enum Cop0Registers reg) const {
    uint32_t val;
    val = cop0reg[(int)reg];
    emit cop0reg_read(reg, val);
    return val;
}

void Cop0State::write_cop0reg_default(enum Cop0Registers reg, uint32_t value) {
    uint32_t mask = cop0reg_desc[(int)reg].write_mask;
    cop0reg[(int)reg] = (value & mask) | (cop0reg[(int)reg] & ~mask);
    emit cop0reg_update(reg, cop0reg[(int)reg]);
}

bool Cop0State::operator==(const Cop0State &c) const {
    for (int i = 0; i < COP0REGS_CNT; i++) {
        if (read_cop0reg((enum Cop0Registers)i)
            != c.read_cop0reg((enum Cop0Registers)i)) {
            return false;
        }
    }
    return true;
}

bool Cop0State::operator!=(const Cop0State &c) const {
    return !this->operator==(c);
}

void Cop0State::reset() {
    for (int i = 1; i < COP0REGS_CNT; i++) {
        this->cop0reg[i] = cop0reg_desc[i].init_value;
        emit cop0reg_update((enum Cop0Registers)i, cop0reg[i]);
    }
    last_core_cycles = 0;
}

void Cop0State::update_execption_cause(
    enum ExceptionCause excause,
    bool in_delay_slot) {
    if (in_delay_slot) {
        cop0reg[(int)Cause] |= 0x80000000;
    } else {
        cop0reg[(int)Cause] &= ~0x80000000;
    }
    cop0reg[(int)Cause] &= ~0x0000007f;
    if (excause != EXCAUSE_INT) {
        cop0reg[(int)Cause] |= (int)excause << 2;
    }
    emit cop0reg_update(Cause, cop0reg[(int)Cause]);
}

void Cop0State::set_interrupt_signal(uint irq_num, bool active) {
    uint32_t mask;
    if (irq_num >= 8) {
        return;
    }
    mask = Status_Int0 << irq_num;
    if (active) {
        cop0reg[(int)Cause] |= mask;
    } else {
        cop0reg[(int)Cause] &= ~mask;
    }
    emit cop0reg_update(Cause, cop0reg[(int)Cause]);
}

bool Cop0State::core_interrupt_request() {
    uint32_t irqs;

    update_count_and_compare_irq();

    irqs = cop0reg[(int)Status];
    irqs &= cop0reg[(int)Cause];
    irqs &= Status_IntMask;

    return irqs && cop0reg[(int)Status] & Status_IntMask
           && !(cop0reg[(int)Status] & Status_EXL)
           && !(cop0reg[(int)Status] & Status_ERL);
}

void Cop0State::set_status_exl(bool value) {
    if (value) {
        cop0reg[(int)Status] |= Status_EXL;
    } else {
        cop0reg[(int)Status] &= ~Status_EXL;
    }
    emit cop0reg_update(Status, cop0reg[(int)Status]);
}

Address Cop0State::exception_pc_address() {
    return Address(cop0reg[(int)EBase] + 0x180);
}

void Cop0State::write_cop0reg_count_compare(
    enum Cop0Registers reg,
    uint32_t value) {
    set_interrupt_signal(COUNTER_IRQ_LEVEL, false);
    write_cop0reg_default(reg, value);
}

void Cop0State::update_count_and_compare_irq() {
    uint32_t core_cycles;
    uint32_t count_orig;
    if (core == nullptr) {
        return;
    }
    count_orig = cop0reg[(int)Count];
    core_cycles = core->get_cycle_count();
    cop0reg[(int)Count] += core_cycles - last_core_cycles;
    last_core_cycles = core_cycles;
    emit cop0reg_update(Count, cop0reg[(int)Count]);

    if ((int32_t)(cop0reg[(int)Compare] - count_orig) > 0
        && (int32_t)(cop0reg[(int)Compare] - cop0reg[(int)Count]) <= 0) {
        set_interrupt_signal(COUNTER_IRQ_LEVEL, true);
    }
}

void Cop0State::write_cop0reg_user_local(enum Cop0Registers reg, uint32_t value) {
    write_cop0reg_default(reg, value);
    if (core != nullptr) {
        core->set_c0_userlocal(value);
    }
}
