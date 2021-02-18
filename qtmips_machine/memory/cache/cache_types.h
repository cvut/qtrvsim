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

#ifndef CACHE_TYPES_H
#define CACHE_TYPES_H

#include <cstdint>

namespace machine {

/**
 * Determiners location of address in single way of cache. This mean, where
 * given addresses should be stored, if present.
 */
struct CacheLocation {
    size_t row;
    size_t col;
    size_t tag;
    size_t byte;
};

/**
 * Single cache line. Appropriate cache block is stored in `data`.
 */
struct CacheLine {
    bool valid, dirty;
    size_t tag;
    std::vector<uint32_t> data;
};

/**
 * This is preferred over bool (write = true|false) for better readability.
 */
enum AccessType { READ, WRITE };

inline const char *to_string(AccessType a) {
    switch (a) {
    case READ: return "READ";
    case WRITE: return "WRITE";
    }
}

} // namespace machine

#endif // CACHE_TYPES_H
