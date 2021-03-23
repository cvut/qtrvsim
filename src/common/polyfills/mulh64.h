/**
 * Provides set of functions to calculate high bits of 64 bit multiplication.
 * Naming reflects RISC-V specs.
 *
 * `mulh64` = signed x signed
 * `mulhu64` = unsigned x unsigned
 * `mulhsu64` = signed x unsigned
 *
 * @file
 */

#ifndef MULH64_H
#define MULH64_H

#include <cstdint>

/**
 * Get high bits of 64 bit unsigned multiplication.
 *
 * Source (BSD-3 in the referred project):
 * https://stackoverflow.com/questions/28868367/getting-the-high-part-of-64-bit-integer-multiplication
 */
static inline constexpr uint64_t mulhu64_fallback(uint64_t a, uint64_t b) {
    const uint64_t a_lower = (uint32_t)a;
    const uint64_t a_upper = (uint32_t)(a >> 32);
    const uint64_t b_lower = (uint32_t)b;
    const uint64_t b_upper = (uint32_t)(b >> 32);
    const uint64_t p11 = a_upper * b_upper, p01 = a_lower * b_upper;
    const uint64_t p10 = a_upper * b_lower, p00 = a_lower * b_lower;
    /*
        This is implementing schoolbook multiplication:

                x1 x0
        X       y1 y0
        -------------
                   00  LOW PART
        -------------
                00
             10 10     MIDDLE PART
        +       01
        -------------
             01
        + 11 11        HIGH PART
        -------------
    */

    const uint64_t middle = p10 + (p00 >> 32) + (uint32_t)p01;
    return p11 + (middle >> 32) + (p01 >> 32);
}

/**
 * mulhu64_fallback modified for signed multiplication
 *
 * SIGN marks show where sign extension has been added.
 * `*_upper` variables are considered signed and everything multiplied with them
 * is also signed.
 *
 * We use the fact, that in most compilers right shift of signed value is done
 * by arithmetic shift.
 */
static inline constexpr uint64_t mulh64_fallback(int64_t a, int64_t b) {
    const uint64_t a_lower = (uint32_t)a;
    const int64_t a_upper = (int32_t)(a >> 32); // SIGN
    const uint64_t b_lower = (uint32_t)b;
    const int64_t b_upper = (int32_t)(b >> 32); // SIGN
    const int64_t p11 = a_upper * b_upper;
    const int64_t p01 = a_lower * b_upper;
    const int64_t p10 = a_upper * b_lower;
    const uint64_t p00 = a_lower * b_lower;
    const int64_t middle = p10 + (p00 >> 32) + (uint32_t)p01;
    return p11 + (middle >> 32) + (p01 >> 32); // SIGN
}

/**
 * mulh64_fallback modified for mixed sign multiplication
 *
 * SIGN marks show where sign extension has been added.
 * `a_upper` is considered signed and everything multiplied with it is also
 * signed.
 *
 * We use the fact, that in most compilers right shift of signed value is done
 * by arithmetic shift.
 */
static inline constexpr uint64_t mulhsu64_fallback(int64_t a, uint64_t b) {
    const uint64_t a_lower = (uint32_t)a;
    const int64_t a_upper = (int32_t)(a >> 32); // SIGN
    const uint64_t b_lower = (uint32_t)b;
    const uint64_t b_upper = (uint32_t)(b >> 32);
    const int64_t p11 = a_upper * b_upper;
    const uint64_t p01 = a_lower * b_upper;
    const int64_t p10 = a_upper * b_lower;
    const uint64_t p00 = a_lower * b_lower;
    const int64_t middle = p10 + (p00 >> 32) + (uint32_t)p01;
    return p11 + (middle >> 32) + (p01 >> 32); // SIGN
}

#if defined(__SIZEOF_INT128__) // GNU C

static inline constexpr uint64_t mulh64(int64_t a, int64_t b) {
    unsigned __int128 prod = (unsigned __int128)a * (unsigned __int128)b;
    return (uint64_t)(prod >> 64);
}

static inline constexpr uint64_t mulhu64(uint64_t a, uint64_t b) {
    unsigned __int128 prod = (unsigned __int128)a * (unsigned __int128)b;
    return (uint64_t)(prod >> 64);
}

static inline constexpr uint64_t mulhsu64(int64_t a, uint64_t b) {
    unsigned __int128 prod = (unsigned __int128)a * (unsigned __int128)b;
    return (uint64_t)(prod >> 64);
}

#elif defined(_M_X64) || defined(_M_ARM64) // MSVC
    // MSVC for x86-64 or AArch64
    // possibly also  || defined(_M_IA64) || defined(_WIN64)
    // but the docs only guarantee x86-64!  Don't use *just* _WIN64; it doesn't
    // include AArch64 Android / Linux
    // https://docs.microsoft.com/en-gb/cpp/intrinsics/arm64-intrinsics

    #include <intrin.h>
    // https://docs.microsoft.com/en-gb/cpp/intrinsics/umulh
    #define mulhu64 __umulh
    // https://docs.microsoft.com/en-gb/cpp/intrinsics/mulh
    #define mulh64 __mulh
    // Not provided by MVSC
    #define mulhsu64 mulhsu64_fallback
#else
    #define mulh64 mulh64_fallback
    #define mulhu64 mulhu64_fallback
    #define mulhsu64 mulhsu64_fallback
#endif

#endif // MULH64_H
