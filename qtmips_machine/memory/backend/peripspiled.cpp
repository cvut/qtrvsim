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

#include "peripspiled.h"

using namespace machine;

#define SPILED_REG_LED_LINE_o 0x004
#define SPILED_REG_LED_RGB1_o 0x010
#define SPILED_REG_LED_RGB2_o 0x014
#define SPILED_REG_LED_KBDWR_DIRECT_o 0x018

#define SPILED_REG_KBDRD_KNOBS_DIRECT_o 0x020
#define SPILED_REG_KNOBS_8BIT_o 0x024

PeripSpiLed::PeripSpiLed() {
    change_counter = 0;

    spiled_reg_led_line = 0;
    spiled_reg_led_rgb1 = 0;
    spiled_reg_led_rgb2 = 0;
    spiled_reg_led_kbdwr_direct = 0;

    spiled_reg_kbdrd_knobs_direct = 0;
    spiled_reg_knobs_8bit = 0;
}

PeripSpiLed::~PeripSpiLed() = default;

bool PeripSpiLed::wword(uint32_t address, uint32_t value) {
    bool changed = false;
#if 0
    printf("PeripSpiLed::wword address 0x%08lx data 0x%08lx\n",
           (unsigned long)address, (unsigned long)value);
#endif
    switch (address & ~3) {
    case SPILED_REG_LED_LINE_o:
        if (spiled_reg_led_line == value) {
            break;
        }
        spiled_reg_led_line = value;
        emit led_line_changed(value);
        break;
    case SPILED_REG_LED_RGB1_o:
        if (spiled_reg_led_rgb1 == value) {
            break;
        }
        spiled_reg_led_rgb1 = value;
        emit led_rgb1_changed(value);
        break;
    case SPILED_REG_LED_RGB2_o:
        if (spiled_reg_led_rgb2 == value) {
            break;
        }
        spiled_reg_led_rgb2 = value;
        emit led_rgb2_changed(value);
        break;
    default: break;
    }

    emit write_notification(address, value);

    if (changed) {
        change_counter++;
    }
    return changed;
}

uint32_t PeripSpiLed::rword(uint32_t address, bool debug_access) const {
    (void)debug_access;
    uint32_t value = 0x00000000;

    switch (address & ~3) {
    case SPILED_REG_LED_LINE_o: value = spiled_reg_led_line; break;
    case SPILED_REG_LED_RGB1_o: value = spiled_reg_led_rgb1; break;
    case SPILED_REG_LED_RGB2_o: value = spiled_reg_led_rgb2; break;
    case SPILED_REG_LED_KBDWR_DIRECT_o:
        value = spiled_reg_led_kbdwr_direct;
        break;
    case SPILED_REG_KBDRD_KNOBS_DIRECT_o:
        value = spiled_reg_kbdrd_knobs_direct;
        break;
    case SPILED_REG_KNOBS_8BIT_o: value = spiled_reg_knobs_8bit; break;
    default: break;
    }

    emit read_notification(address, &value);

#if 0
    printf("PeripSpiLed::rword address 0x%08lx value 0x%08lx\n",
           (unsigned long)address, (unsigned long)value);
#endif

    return value;
}

uint32_t PeripSpiLed::get_change_counter() const {
    return change_counter;
}

void PeripSpiLed::knob_update_notify(uint32_t val, uint32_t mask, int shift) {
    mask <<= shift;
    val <<= shift;
    if (!((spiled_reg_knobs_8bit ^ val) & mask)) {
        return;
    }
    spiled_reg_knobs_8bit &= ~mask;
    spiled_reg_knobs_8bit |= val;
    change_counter++;
    emit external_change_notify(
        this, SPILED_REG_KNOBS_8BIT_o, SPILED_REG_KNOBS_8BIT_o + 3, true);
}

void PeripSpiLed::red_knob_update(int val) {
    knob_update_notify(val, 0xff, 16);
}

void PeripSpiLed::green_knob_update(int val) {
    knob_update_notify(val, 0xff, 8);
}

void PeripSpiLed::blue_knob_update(int val) {
    knob_update_notify(val, 0xff, 0);
}

void PeripSpiLed::red_knob_push(bool state) {
    knob_update_notify(state ? 1 : 0, 1, 26);
}

void PeripSpiLed::green_knob_push(bool state) {
    knob_update_notify(state ? 1 : 0, 1, 25);
}

void PeripSpiLed::blue_knob_push(bool state) {
    knob_update_notify(state ? 1 : 0, 1, 24);
}
