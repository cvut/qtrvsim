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

#ifndef QTMIPS_REGISTER_VALUE_H
#define QTMIPS_REGISTER_VALUE_H

namespace machine {

/*
 * Register size configuration
 *
 * TODO: make compile time option
 */
using register_storage_t = uint32_t;

/**
 * Represents a value stored in register
 *
 * Register value is semantically considered to be only an array of bits
 *  and with no meaning assumed, therefore no operations are implemented
 *  and value has to be interpreted as numerical value.
 *
 * By default, registers are initialized to zero.
 */
class RegisterValue {
public:
    /**
     *
     * NOTE ON IMPLICIT CONVERSION:
     *  Implicit conversion from unsigned integer is allowed as RegisterValue
     *  as it essentially mean to forget the meaning of the value. Reverse
     *  direction is always explicit.
     *
     * Constructor needs to be defined even for uint32_t as cpp cannot decide
     *  whether to use uint64_t or int32_t.
     */
    constexpr inline RegisterValue(uint64_t value) : data(value) {};

    constexpr inline RegisterValue(uint32_t value) : data(value) {};

    constexpr inline RegisterValue() : data(0) {};

    constexpr inline RegisterValue(const RegisterValue &other) = default;

    /* Sign-extending constructors */

    constexpr inline RegisterValue(int64_t value) : data(value) {};

    constexpr inline RegisterValue(int32_t value) : data(value) {};

    constexpr inline RegisterValue(int16_t value) : data(value) {};

    constexpr inline RegisterValue(int8_t value) : data(value) {};

    constexpr inline int32_t as_i32() const {
        return (int32_t)data;
    };

    constexpr inline uint32_t as_u32() const {
        return (uint32_t)data;
    };

    constexpr inline uint64_t as_u64() const {
        return (uint64_t)data;
    };

    constexpr inline int64_t as_i64() const {
        return (int64_t)data;
    };

    /**
     * Equality operator is implemented as bit by bit comparison is reasonable
     *  for bit array.
     * It is necessary to make gp-register array comparable.
     */
    constexpr inline bool operator==(const RegisterValue &other) const {
        return data == other.data;
    }

    constexpr inline bool operator!=(const RegisterValue &other) const {
        return !(other == *this);
    }

private:
    register_storage_t data;
};

} // namespace machine
#endif // QTMIPS_REGISTER_VALUE_H
