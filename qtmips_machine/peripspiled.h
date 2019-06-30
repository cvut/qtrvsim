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

#ifndef PERIPSPILED_H
#define PERIPSPILED_H

#include <QObject>
#include <QMap>
#include <cstdint>
#include <qtmipsexception.h>
#include "machinedefs.h"
#include "memory.h"

namespace machine {

class PeripSpiLed : public MemoryAccess {
    Q_OBJECT
public:
    PeripSpiLed();
    ~PeripSpiLed();

signals:
    void write_notification(std::uint32_t address, std::uint32_t value);
    void read_notification(std::uint32_t address, std::uint32_t *value) const;

    void led_line_changed(uint val) const;
    void led_rgb1_changed(uint val) const;
    void led_rgb2_changed(uint val) const;

public slots:
    void red_knob_update(int val);
    void green_knob_update(int val);
    void blue_knob_update(int val);
    void red_knob_push(bool state);
    void green_knob_push(bool state);
    void blue_knob_push(bool state);
public:
    bool wword(std::uint32_t address, std::uint32_t value) override;
    std::uint32_t rword(std::uint32_t address, bool debug_access = false) const override;
    virtual std::uint32_t get_change_counter() const override;
private:
    void knob_update_notify(std::uint32_t val, std::uint32_t mask, int shift);

    mutable std::uint32_t change_counter;
    std::uint32_t spiled_reg_led_line;
    std::uint32_t spiled_reg_led_rgb1;
    std::uint32_t spiled_reg_led_rgb2;
    std::uint32_t spiled_reg_led_kbdwr_direct;

    std::uint32_t spiled_reg_kbdrd_knobs_direct;
    std::uint32_t spiled_reg_knobs_8bit;
};

}

#endif // PERIPSPILED_H
