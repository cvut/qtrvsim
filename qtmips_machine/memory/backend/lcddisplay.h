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

#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H

#include "machinedefs.h"
#include "memory.h"
#include "qtmipsexception.h"

#include <QMap>
#include <QObject>
#include <cstdint>

namespace machine {

class LcdDisplay : public MemoryAccess {
    Q_OBJECT
public:
    LcdDisplay();
    ~LcdDisplay() override;

signals:
    void write_notification(uint32_t address, uint32_t value);
    void read_notification(uint32_t address, uint32_t *value) const;
    void pixel_update(uint x, uint y, uint r, uint g, uint b);

public:
    bool wword(uint32_t address, uint32_t value) override;
    uint32_t rword(uint32_t address, bool debug_access = false) const override;
    uint32_t get_change_counter() const override;

    inline uint width() const {
        return fb_width;
    }

    inline uint height() const {
        return fb_height;
    }

private:
    mutable uint32_t change_counter;
    uint32_t pixel_address(uint x, uint y) const;
    uchar *fb_data;
    size_t fb_size;
    unsigned fb_bpp;
    unsigned fb_width;
    unsigned fb_height;
    unsigned fb_linesize;
};

} // namespace machine

#endif // LCDDISPLAY_H
