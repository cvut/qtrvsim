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

#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <type_traits>

using std::size_t;

#if __GNUC__ >= 7
    #define FALLTROUGH __attribute__((fallthrough));
#else
    #define FALLTROUGH
#endif

/**
 * Raw byte in memory
 * - Intended for raw byte array
 */
typedef unsigned char byte;

inline constexpr uint32_t sign_extend(uint16_t v) {
    return ((v & 0x8000) ? 0xFFFF0000 : 0) | v;
}

template<class T>
void ignore(const T &) {}

#define UNIMPLEMENTED throw std::logic_error("Unimplemented");
#define PANIC throw std::logic_error("The program panicked.");
#define UNREACHABLE assert(false);
#define UNUSED(arg) ignore(arg);
/**
 * Annotate pointer ownership.
 * Smartpointer may be used in the future.
 */
#define OWNED

/**
 * Test whether given address is aligned to the given type.
 *
 * @tparam Address      type used to store the address
 * @tparam T            type to check alignment
 * @param address       address to check
 * @return              true if is aligned
 */
template<typename Address, typename T>
inline bool is_aligned_generic(Address address) {
    return static_cast<uintptr_t>(address) % std::alignment_of<T>::value;
}

/**
 * Divide and round up
 */
template<typename T1, typename T2>
inline constexpr T1 divide_and_ceil(T1 divident, T2 divisor) {
    return ((divident + divisor - 1) / divisor);
}

/**
 * Rounds number `n` down to the multiple of `base`.
 * (To by used as better macro.)
 *
 * @tparam T1   type of n
 * @tparam T2   type of base
 */
template<typename T1, typename T2>
inline constexpr T1 round_down_to_multiple(T1 n, T2 base) {
    return (n / base) * base;
}

/**
 * Rounds number `n` up to the multiple of `base`.
 * (To by used as better macro.)
 *
 * @tparam T1   type of n
 * @tparam T2   type of base
 */
template<typename T1, typename T2>
inline constexpr T1 round_up_to_multiple(T1 n, T2 base) {
    return round_down_to_multiple(n + base, base);
}

/**
 * Full generic byteswap.
 * Optimized specializations are provided bellow.
 */
template<typename T>
inline T byteswap(T val) {
    union U {
        T val;
        std::array<uint8_t, sizeof(T)> raw;
    } src, dst;

    src.val = val;
    std::reverse_copy(src.raw.begin(), src.raw.end(), dst.raw.begin());
    return dst.val;
}

template<>
inline uint8_t byteswap(uint8_t val) {
    // NOOP for single byte
    return val;
}

template<>
inline uint16_t byteswap(uint16_t val) {
    return (val << 8) | (val >> 8);
}

template<>
inline uint32_t byteswap(uint32_t val) {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
}
template<>
inline uint64_t byteswap(uint64_t val) {
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL)
          | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL)
          | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | (val >> 32);
}

#endif // UTILS_H
