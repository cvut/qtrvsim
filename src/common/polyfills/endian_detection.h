#ifndef ENDIAN_DETECTION_H
#define ENDIAN_DETECTION_H
/**
 * Including this file ensures that GCC macros `__LITTLE_ENDIAN__` or
 * `__BIG_ENDIAN__` are defined regardless of OS and compiler. If used
 * system/compiler is not supported, it will stop the compilation using a
 * static assert. Precisely one of the given macros is always guaranteed to be
 * defined.
 *
 * @file
 */

// Cross-platform endian detection source
// https://gist.github.com/jtbr/7a43e6281e6cca353b33ee501421860c (MIT license)

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

#if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
static_assert(
    false,
    "Current compiler/system is not supported by the endian "
    "detection polyfill code or it uses unsupported endian.\n"
    "Supported endians are: BIG | LITTLE.");
#endif

#if defined(__LITTLE_ENDIAN__) && defined(__BIG_ENDIAN__)
static_assert(
    false,
    "Both endianness macros are defined. This is a bug of endian "
    "detection. Please report it via a github issue.");
#endif

#endif // ENDIAN_DETECTION_H
