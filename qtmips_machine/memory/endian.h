#ifndef QTMIPS_ENDIAN_H
#define QTMIPS_ENDIAN_H

#include "utils.h"

#include <QMetaType>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <type_traits>

namespace machine {
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
}

// Cross-platform endian detection
// https://gist.github.com/jtbr/7a43e6281e6cca353b33ee501421860c (MIT license)

// When available, these headers can improve platform endianness detection
#ifdef __has_include // C++17, supported as extension to C++11 in clang, GCC 5+,
                     // vs2015
    #if __has_include(<endian.h>)
        #include <endian.h> // gnu libc normally provides, linux
    #elif __has_include(<machine/endian.h>)
        #include <machine/endian.h> //open bsd, macos
    #elif __has_include(<sys/param.h>)
        #include <sys/param.h> // mingw, some bsd (not open/macos)
    #elif __has_include(<sys/isadefs.h>)
        #include <sys/isadefs.h> // solaris
    #endif
#endif

#if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
    #if (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)    \
        || (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)             \
        || (defined(_BYTE_ORDER) && _BYTE_ORDER == _BIG_ENDIAN)                \
        || (defined(BYTE_ORDER) && BYTE_ORDER == BIG_ENDIAN)                   \
        || (defined(__sun) && defined(__SVR4) && defined(_BIG_ENDIAN))         \
        || defined(__ARMEB__) || defined(__THUMBEB__)                          \
        || defined(__AARCH64EB__) || defined(_MIBSEB) || defined(__MIBSEB)     \
        || defined(__MIBSEB__) || defined(_M_PPC)
        #define __BIG_ENDIAN__
    #elif (                                                                    \
        defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)  \
        ||                                                         /* gcc */   \
        (defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN) /* linux    \
                                                                      header   \
                                                                    */         \
        || (defined(_BYTE_ORDER) && _BYTE_ORDER == _LITTLE_ENDIAN)             \
        || (defined(BYTE_ORDER) && BYTE_ORDER == LITTLE_ENDIAN) /* mingw       \
                                                                   header */   \
        || (defined(__sun) && defined(__SVR4) && defined(_LITTLE_ENDIAN))      \
        || /* solaris */                                                       \
        defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__)   \
        || defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)        \
        || defined(_M_IX86) || defined(_M_X64) || defined(_M_IA64)             \
        ||              /* msvc for intel processors */                        \
        defined(_M_ARM) /* msvc code on arm executes in little endian mode */
        #define __LITTLE_ENDIAN__
    #endif
#endif

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

/* Define byte-swap functions, using fast processor-native built-ins where
 * possible */
#if defined(_MSC_VER) // needs to be first because msvc doesn't short-circuit
                      // after failing defined(__has_builtin)
    #define bswap16(x) _byteswap_ushort((x))
    #define bswap32(x) _byteswap_ulong((x))
    #define bswap64(x) _byteswap_uint64((x))
#elif (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
    #define bswap16(x) __builtin_bswap16((x))
    #define bswap32(x) __builtin_bswap32((x))
    #define bswap64(x) __builtin_bswap64((x))
#elif defined(__has_builtin)                                                   \
    && __has_builtin(__builtin_bswap64) /* for clang; gcc 5 fails on this and  \
                                           && short circuit fails; must be     \
                                           after GCC check */
    #define bswap16(x) __builtin_bswap16((x))
    #define bswap32(x) __builtin_bswap32((x))
    #define bswap64(x) __builtin_bswap64((x))
#else
/* even in this case, compilers often optimize by using native instructions */
static inline uint16_t bswap16(uint16_t x) {
    return (val << 8) | (val >> 8);
}
static inline uint32_t bswap32(uint32_t x) {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
}
static inline uint64_t bswap64(uint64_t x) {
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL)
          | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL)
          | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | (val >> 32);
}
#endif

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

} // namespace machine

Q_DECLARE_METATYPE(machine::Endian)


#endif // QTMIPS_ENDIAN_H
