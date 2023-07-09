#ifndef REGISTER_VALUE_H
#define REGISTER_VALUE_H

#include "machine/machineconfig.h"

#include <QMetaType>

namespace machine {

/*
 * Register size configuration
 *
 * TODO: make compile time option
 */
using register_storage_t = uint64_t;

/**
 * Represents a value stored in register
 *
 * Register value is semantically considered to be only an array of bits
 *  and with no meaning assumed, therefore no operations are implemented
 *  and value has to be interpreted as numerical value.
 *
 * By default, registers are initialized to zero.
 */
class RegisterValue {
public:
    /**
     *
     * NOTE ON IMPLICIT CONVERSION:
     *  Implicit conversion from unsigned integer is allowed as RegisterValue
     *  as it essentially mean to forget the meaning of the value. Reverse
     *  direction is always explicit.
     *
     * Constructor needs to be defined even for uint32_t as cpp cannot decide
     *  whether to use uint64_t or int32_t.
     */
    constexpr inline RegisterValue(uint64_t value) // NOLINT(google-explicit-constructor)
        : data(value) {};

    // Must be present to avoid ambiguity.
    constexpr inline RegisterValue(uint32_t value) // NOLINT(google-explicit-constructor)
        : data(value) {};

    constexpr inline RegisterValue() : data(0) {};

    constexpr inline RegisterValue(const RegisterValue &other) = default;
    constexpr inline RegisterValue &operator=(const RegisterValue &other) = default;

    /* Sign-extending constructors */

    constexpr inline RegisterValue(int64_t value) // NOLINT(google-explicit-constructor)
        : data(value) {};

    constexpr inline RegisterValue(int32_t value) // NOLINT(google-explicit-constructor)
        : data(value) {};

    constexpr inline RegisterValue(int16_t value) // NOLINT(google-explicit-constructor)
        : data(value) {};

    constexpr inline RegisterValue(int8_t value) // NOLINT(google-explicit-constructor)
        : data(value) {};

    [[nodiscard]] constexpr inline uint64_t as_xlen(Xlen xlen) const {
        switch (xlen) {
        case Xlen::_32: return as_u32();
        case Xlen::_64: return as_u64();
        default: UNREACHABLE
        }
    }

    [[nodiscard]] constexpr inline int8_t as_i8() const { return (int8_t)data; };

    [[nodiscard]] constexpr inline uint8_t as_u8() const { return (uint8_t)data; };

    [[nodiscard]] constexpr inline int16_t as_i16() const { return (int16_t)data; };

    [[nodiscard]] constexpr inline uint16_t as_u16() const { return (uint16_t)data; };

    [[nodiscard]] constexpr inline int32_t as_i32() const { return (int32_t)data; };

    [[nodiscard]] constexpr inline uint32_t as_u32() const { return (uint32_t)data; };

    [[nodiscard]] constexpr inline int64_t as_i64() const { return (int64_t)data; };

    [[nodiscard]] constexpr inline uint64_t as_u64() const { return (uint64_t)data; };

    constexpr explicit operator int8_t() const { return as_i8(); };

    constexpr explicit operator uint8_t() const { return as_u8(); };

    constexpr explicit operator int16_t() const { return as_i16(); };

    constexpr explicit operator uint16_t() const { return as_u16(); };

    constexpr explicit operator int32_t() const { return as_i32(); };

    constexpr explicit operator uint32_t() const { return as_u32(); };

    constexpr explicit operator int64_t() const { return as_i64(); };

    constexpr explicit operator uint64_t() const { return as_u64(); };

    /**
     * Equality operator is implemented as bit by bit comparison is reasonable
     *  for bit array.
     * It is necessary to make gp-register array comparable.
     */
    constexpr inline bool operator==(const RegisterValue &other) const {
        return data == other.data;
    }

    constexpr inline bool operator!=(const RegisterValue &other) const { return !(other == *this); }

private:
    register_storage_t data;
};

} // namespace machine

Q_DECLARE_METATYPE(machine::RegisterValue)

#endif // REGISTER_VALUE_H
