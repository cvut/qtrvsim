#ifndef ADDRESS_H
#define ADDRESS_H

#include "utils.h"

#include <QMetaType>
#include <cstdint>

using std::uint64_t;

namespace machine {

/**
 * Emulated physical memory address.
 *
 * It is guaranteed that all instances of this type belong to single address
 * space and therefore all can be compared fearlessly.
 * <p>
 * OPTIMIZATION NOTE: All methods are implemented in header file to support
 * inlining (as we want to use it as a primitive type) but too keep declaration
 * tidy out-of-line definitions are preferred.
 */
class Address {
private:
    uint64_t data; //> Real wrapped address

public:
    constexpr explicit Address(uint64_t);

    /**
     *  Default constructor results in null pointer.
     *  Why it exists? To make address default constructable for STL.
     */
    constexpr Address();

    constexpr Address(const Address &address) = default;            //> Copy constructor
    constexpr Address &operator=(const Address &address) = default; //> Assign constructor

    /**
     * Extracts raw numeric value of the address.
     * Why it exists? Output, unsafe operations not supported by operators.
     */
    [[nodiscard]] constexpr uint64_t get_raw() const;

    /**
     * Explicit cast operator to underlying type.
     * This is an alternative ti get_raw() method.
     *
     * NOTE: This operator is needed for correct functioning of
     * repeat_access_until_completed function defined in memory_utils.h.
     */
    constexpr explicit operator uint64_t() const;

    /**
     * More expressive way to assign null.
     * Why it exists? Equivalent of nullptr in cpp. Address(0) was ugly and too
     * common.
     */
    constexpr static Address null();

    /**
     * Test for null address
     *
     * Why is exists? Testing for null in very often and without this method a
     * creation of a new address object set to null would be required. That is
     * considered too long and badly readable. For other comparison we expect a
     * comparison with existing value to be the common case.
     * Alternative: `address.get_raw() == 0`
     * Alternative: `address == Address::null()`
     */
    [[nodiscard]] constexpr bool is_null() const;

    /**
     * Test whether address is aligned to the size of type T.
     *
     * Example:
     * ```
     *  is_aligned<uint32_t>(address) <=> address % 4 == 0
     * ```
     */
    template<typename T>
    [[nodiscard]] constexpr bool is_aligned() const;

    /* Eq */
    constexpr inline bool operator==(const Address &other) const;
    constexpr inline bool operator!=(const Address &other) const;

    /* Ord */
    constexpr inline bool operator<(const Address &other) const;
    constexpr inline bool operator>(const Address &other) const;
    constexpr inline bool operator<=(const Address &other) const;
    constexpr inline bool operator>=(const Address &other) const;

    /* Offset arithmetic */
    constexpr inline Address operator+(const uint64_t &offset) const;
    constexpr inline Address operator-(const uint64_t &offset) const;
    inline void operator+=(const uint64_t &offset);
    inline void operator-=(const uint64_t &offset);

    /* Bitwise arithmetic */
    constexpr inline Address operator&(const uint64_t &mask) const;
    constexpr inline Address operator|(const uint64_t &mask) const;
    constexpr inline Address operator^(const uint64_t &mask) const;
    constexpr inline Address operator>>(const uint64_t &size) const;
    constexpr inline Address operator<<(const uint64_t &size) const;

    /* Distance arithmetic */
    constexpr inline int64_t operator-(const Address &other) const;
};

constexpr Address operator"" _addr(unsigned long long literal) {
    return Address(literal);
}

constexpr Address::Address(uint64_t address) : data(address) {}

constexpr Address::Address() : data(0) {}

constexpr uint64_t Address::get_raw() const {
    return data;
}

constexpr Address::operator uint64_t() const {
    return this->get_raw();
}

constexpr Address Address::null() {
    return Address(0x0);
}

constexpr bool Address::is_null() const {
    return this->get_raw() == 0;
}

template<typename T>
constexpr bool Address::is_aligned() const {
    return is_aligned_generic<typeof(this->data), T>(this->data);
}

/*
 * Equality operators
 */

constexpr bool Address::operator==(const Address &other) const {
    return this->get_raw() == other.get_raw();
}

constexpr bool Address::operator!=(const Address &other) const {
    return this->get_raw() != other.get_raw();
}

/*
 * Ordering operators
 */

constexpr bool Address::operator<(const Address &other) const {
    return this->get_raw() < other.get_raw();
}

constexpr bool Address::operator>(const Address &other) const {
    return this->get_raw() > other.get_raw();
}

constexpr bool Address::operator<=(const Address &other) const {
    return this->get_raw() <= other.get_raw();
}

constexpr bool Address::operator>=(const Address &other) const {
    return this->get_raw() >= other.get_raw();
}

/*
 * Offset arithmetic operators
 */

constexpr Address Address::operator+(const uint64_t &offset) const {
    return Address(this->get_raw() + offset);
}

constexpr Address Address::operator-(const uint64_t &offset) const {
    return Address(this->get_raw() - offset);
}

void Address::operator+=(const uint64_t &offset) {
    data += offset;
}

void Address::operator-=(const uint64_t &offset) {
    data -= offset;
}

/*
 * Bitwise operators
 */

constexpr Address Address::operator&(const uint64_t &mask) const {
    return Address(this->get_raw() & mask);
}

constexpr Address Address::operator|(const uint64_t &mask) const {
    return Address(this->get_raw() | mask);
}

constexpr Address Address::operator^(const uint64_t &mask) const {
    return Address(get_raw() ^ mask);
}

constexpr Address Address::operator>>(const uint64_t &size) const {
    return Address(this->get_raw() >> size);
}

constexpr Address Address::operator<<(const uint64_t &size) const {
    return Address(this->get_raw() << size);
}

/*
 * Distance arithmetic operators
 */

constexpr int64_t Address::operator-(const Address &other) const {
    return this->get_raw() - other.get_raw();
}

} // namespace machine

Q_DECLARE_METATYPE(machine::Address)

#endif // ADDRESS_H
