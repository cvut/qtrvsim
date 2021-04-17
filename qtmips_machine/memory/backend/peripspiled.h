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
 * Copyright (c) 2020      Jakub Dupak <dupak.jakub@gmail.com>
 * Copyright (c) 2020      Max Hollmann <hollmmax@fel.cvut.cz>
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

#include "machinedefs.h"
#include "memory/backend/backend_memory.h"
#include "memory/memory_utils.h"
#include "qtmipsexception.h"

#include <cstdint>

namespace machine {

class PeripSpiLed final : public BackendMemory {
    Q_OBJECT
public:
    explicit PeripSpiLed(Endian simulated_machine_endian);
    ~PeripSpiLed() override;

signals:
    void write_notification(Offset address, uint32_t value) const;
    void read_notification(Offset address, uint32_t value) const;

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
    WriteResult write(
        Offset destination,
        const void *source,
        size_t size,
        WriteOptions options) override;

    ReadResult read(
        void *destination,
        Offset source,
        size_t size,
        ReadOptions options) const override;

    LocationStatus location_status(Offset offset) const override;

private:
    uint32_t read_reg(Offset source) const;
    bool write_reg(Offset destination, uint32_t value);
    void knob_update_notify(uint32_t val, uint32_t mask, size_t shift);

    /** endian of internal registers of the periphery use. */
    static constexpr Endian internal_endian = NATIVE_ENDIAN;
    uint32_t spiled_reg_led_line = 0;
    uint32_t spiled_reg_led_rgb1 = 0;
    uint32_t spiled_reg_led_rgb2 = 0;
    uint32_t spiled_reg_led_kbdwr_direct = 0;
    uint32_t spiled_reg_kbdrd_knobs_direct = 0;
    uint32_t spiled_reg_knobs_8bit = 0;
};

} // namespace machine

#endif // PERIPSPILED_H
