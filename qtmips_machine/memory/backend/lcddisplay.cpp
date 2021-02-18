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

#include "lcddisplay.h"

#ifdef DEBUG_LCD
    #undef DEBUG_LCD
    #define DEBUG_LCD true
#else
    #define DEBUG_LCD false
#endif

namespace machine {

LcdDisplay::LcdDisplay()
    : fb_width(480)
    , fb_height(320)
    , fb_bits_per_pixel(16)
    , fb_data(get_fb_size_bytes(), 0) {}

LcdDisplay::~LcdDisplay() = default;

WriteResult LcdDisplay::write(
    Offset destination,
    const void *source,
    size_t size,
    WriteOptions options) {
    UNUSED(options)
    return write_by_u32(
        destination, source, size, [&](Offset src) { return read_reg(src); },
        [&](Offset src, uint32_t value) { return write_reg(src, value); });
}

ReadResult LcdDisplay::read(
    void *destination,
    Offset source,
    size_t size,
    ReadOptions options) const {
    UNUSED(options)
    return read_by_u32(
        destination, source, size, [&](Offset src) { return read_reg(src); });
}

uint32_t LcdDisplay::read_reg(Offset source) const {
    Q_ASSERT((source & 3U) == 0); // uint32_t alligned

    if (source + 3 >= get_fb_size_bytes()) {
        return 0;
    }

    uint32_t value;
    memcpy(&value, &fb_data[source], sizeof(value));

    // TODO Switch to if constexpr as soon as we have cpp17.
    if (DEBUG_LCD) {
        printf(
            "LcdDisplay::readreg address 0x%08lx data 0x%08lx\n",
            (unsigned long)source, (unsigned long)value);
    }

    emit read_notification(source, value);
    return value;
}

bool LcdDisplay::write_reg(Offset destination, uint32_t value) {
    Q_ASSERT((destination & 3U) == 0); // uint32_t aligned

    if (destination + 3 >= get_fb_size_bytes()) {
        printf("WARNING: LCD display - read out of range.");
        return false;
    }

    // TODO Switch to if constexpr as soon as we have cpp17.
    if (DEBUG_LCD) {
        printf(
            "LcdDisplay::write_reg address 0x%08lx data 0x%08lx\n",
            (unsigned long)destination, (unsigned long)value);
    }

    if (read_reg(destination) == value) {
        return false;
    }

    memcpy(&fb_data[destination], &value, sizeof(value));

    size_t x, y;
    std::tie(x, y) = get_pixel_from_address(destination);

    const uint32_t last_addr = destination + 3;
    uint32_t pixel_addr;
    uint32_t pixek_data;
    uint r, g, b;

    while ((pixel_addr = get_address_from_pixel(x, y)) <= last_addr) {
        pixek_data = fb_data[pixel_addr] << 8u;
        pixek_data |= fb_data[pixel_addr + 1];

        r = ((pixek_data >> 11u) & 0x1fu) << 3u;
        g = ((pixek_data >> 5u) & 0x3fu) << 2u;
        b = ((pixek_data >> 0u) & 0x1fu) << 3u;

        emit pixel_update(x, y, r, g, b);

        if (++x >= fb_width) {
            x = 0;
            y++;
        }
    }

    emit write_notification(destination, value);

    return true;
}

size_t LcdDisplay::get_address_from_pixel(size_t x, size_t y) const {
    size_t address = y * get_fb_line_size();
    if (fb_bits_per_pixel > 12) {
        address += x * divide_and_ceil(fb_bits_per_pixel, 8u);
    } else {
        address += x * fb_bits_per_pixel / 8;
    }

    return address;
}

std::tuple<size_t, size_t>
LcdDisplay::get_pixel_from_address(size_t address) const {
    size_t y = address / get_fb_line_size();
    size_t x = (fb_bits_per_pixel > 12)
                   ? (address - y * get_fb_line_size())
                         / ((fb_bits_per_pixel + 7) >> 3u)
                   : (address - y * get_fb_line_size()) * 8 / fb_bits_per_pixel;
    return std::make_tuple(x, y);
}

size_t LcdDisplay::get_fb_line_size() const {
    return (fb_bits_per_pixel > 12) ? ((fb_bits_per_pixel + 7) >> 3u) * fb_width
                                    : (fb_bits_per_pixel * fb_width + 7) >> 3u;
}

size_t LcdDisplay::get_fb_size_bytes() const {
    return get_fb_line_size() * fb_height;
}
LocationStatus LcdDisplay::location_status(Offset offset) const {
    if ((offset | ~3u) >= get_fb_size_bytes()) {
        return LOCSTAT_ILLEGAL;
    }
    return LOCSTAT_NONE;
}

} // namespace machine
