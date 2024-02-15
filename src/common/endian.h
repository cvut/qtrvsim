#ifndef ENDIAN_H
#define ENDIAN_H

#include "polyfills/byteswap.h"
#include "polyfills/endian_detection.h"
#include "utils.h"

#include <QMetaType>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <type_traits>

/**
 * Memory endian type (used for bot simulator and host machine).
 * Standard enum is not available until c++20.
 */
enum Endian { LITTLE, BIG };

inline const char *to_string(Endian val) {
    switch (val) {
    case LITTLE: return "LITTLE";
    case BIG: return "BIG";
    }
    UNREACHABLE;
}

inline constexpr Endian get_native_endian() {
#if (defined(__BIG_ENDIAN__))
    return BIG;
#elif (defined(__LITTLE_ENDIAN__))
    return LITTLE;
#else
    static_assert(
        false, "Could not detect endian or endian is neither big nor little.");
#endif
}

constexpr Endian NATIVE_ENDIAN = get_native_endian();

/**
 * Full generic byteswap. Used in generic functions.
 * Optimized specializations are provided bellow.
 */
template<typename T>
inline T byteswap(T val) {
    static_assert(
        sizeof(T) <= 8, "Byteswap of large types is implementation dependant.");

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
    // NOP for single byte
    return val;
}
template<>
inline uint16_t byteswap(uint16_t val) {
    return bswap16(val);
}
template<>
inline uint32_t byteswap(uint32_t val) {
    return bswap32(val);
}
template<>
inline uint64_t byteswap(uint64_t val) {
    return bswap64(val);
}
/**
 * Conditionally byteswap value. Condition if usually mismatch of endian on
 * interface of two memory levels or memory and core.
 */
template<typename T>
T byteswap_if(T value, bool condition) {
    return (condition) ? byteswap(value) : value;
}

Q_DECLARE_METATYPE(Endian)

#endif // ENDIAN_H
