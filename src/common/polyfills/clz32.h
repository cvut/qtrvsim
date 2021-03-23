#ifndef CLZ32_H
#define CLZ32_H

#if defined(__GNUC__) && __GNUC__ >= 4

static inline uint32_t clz32(uint32_t n) {
    int intbits = sizeof(int) * CHAR_BIT;
    if (n == 0) { return 32; }
    return __builtin_clz(n) - (intbits - 32);
}

#else /* Fallback for generic compiler */

// see https://en.wikipedia.org/wiki/Find_first_set#CLZ
static const uint8_t sig_table_4bit[16] = { 0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4 };

static inline uint32_t clz32(uint32_t n) {
    int len = 32;

    if (n & 0xFFFF0000) {
        len -= 16;
        n >>= 16;
    }
    if (n & 0xFF00) {
        len -= 8;
        n >>= 8;
    }
    if (n & 0xF0) {
        len -= 4;
        n >>= 4;
    }
    len -= sig_table_4bit[n];
    return len;
}

#endif

#endif // CLZ32_H
