#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <array>
#include <cassert>
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
#define UNREACHABLE Q_UNREACHABLE();
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

#endif // UTILS_H
