#ifndef BYTESWAP_H
#define BYTESWAP_H

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

#endif // BYTESWAP_H
