/**
 * Bit manipulation library
 *
 * @file
 */

#ifndef QTRVSIM_BIT_OPS_H
#define QTRVSIM_BIT_OPS_H

#include <cassert>
#include <cstdint>
#include <limits>

using std::size_t;

/**
 * Get value of single bit as lowest bit
 *
 * Corresponds to instruction notation `SYMBOL[<bit_index>]`.
 */
template<typename T>
constexpr inline T get_bit(T val, size_t bit_index) {
    return (val >> bit_index) & 1;
}

/**
 * Generates a bitmask to mask a range of bits.
 */
template<typename T>
constexpr inline T get_bitmask(size_t start, size_t end) {
    const size_t len = start - end + 1;
    return ((1 << len) - 1) << end;
}

/**
 * Mask a range of bits in an integer-like value.
 */
template<typename T>
constexpr inline T mask_bits(T val, size_t start, size_t end) {
    return val & get_bitmask<T>(start, end);
}

/**
 * Extracts range of bits from an integer-like value.
 *
 * Corresponds to instruction notation `SYMBOL[start:end]`.
 */
template<typename T>
constexpr inline T get_bits(T val, size_t start, size_t end) {
    assert(start >= end);
    return mask_bits(val >> end, start - end, 0);
}

/**
 * Sign extend arbitrary bit range.
 */
template<typename T>
constexpr inline T sign_extend(T val, size_t size) {
    size_t T_size = std::numeric_limits<T>::digits;
    if (std::numeric_limits<T>::is_signed) T_size += 1;
    size_t shift = T_size - size;
    return ((int64_t)val << shift) >> shift;
}

#endif // QTRVSIM_BIT_OPS_H
