#ifndef INTEGER_DECOMPOSITION_H
#define INTEGER_DECOMPOSITION_H

#include "common/endian.h"

#include <array>
#include <cstdint>


using namespace machine;
using std::array;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::uint8_t;

template<typename T, size_t N>
constexpr void decompose_int_to_array(array<T, N> &dst,
                                      uint64_t value,
                                      Endian endian) {
    for (size_t i = 0; i < N; ++i) {
        T val = (T) (value >> (8 * sizeof(T) * i));
        if (endian == LITTLE) {
            dst.at(i) = val;
        } else {
            dst.at(N - i - 1) = val;
        }
    }
}


struct IntegerDecomposition {
    uint64_t           u64{};
    array<uint32_t, 2> u32{};
    array<uint16_t, 4> u16{};
    array<uint8_t, 8>  u8{};

    constexpr IntegerDecomposition() = default;

    constexpr IntegerDecomposition(uint64_t value, Endian endian) {
        u64 = value;
        decompose_int_to_array(u32, value, endian);
        decompose_int_to_array(u16, value, endian);
        decompose_int_to_array(u8, value, endian);
    }

    constexpr bool operator==(const IntegerDecomposition &rhs) const {
        return u64 == rhs.u64 && u32 == rhs.u32 && u16 == rhs.u16
               && u8 == rhs.u8;
    }

    constexpr bool operator!=(const IntegerDecomposition &rhs) const {
        return !(rhs == *this);
    }
};


Q_DECLARE_METATYPE(IntegerDecomposition)


#endif // INTEGER_DECOMPOSITION_H
