#ifndef VIRTUAL_ADDRESS_H
#define VIRTUAL_ADDRESS_H

#include "utils.h"

#include <QMetaType>
#include <cstdint>

using std::uint64_t;

namespace machine {

class VirtualAddress {
private:
    uint64_t data; // Raw virtual address

public:
    constexpr explicit VirtualAddress(uint64_t);

    constexpr VirtualAddress();

    constexpr VirtualAddress(const VirtualAddress &address) = default;
    constexpr VirtualAddress &operator=(const VirtualAddress &address) = default;

    [[nodiscard]] constexpr uint64_t get_raw() const;

    constexpr explicit operator uint64_t() const;

    constexpr static VirtualAddress null();

    [[nodiscard]] constexpr bool is_null() const;

    template<typename T>
    [[nodiscard]] constexpr bool is_aligned() const;

    /* Equality */
    constexpr inline bool operator==(const VirtualAddress &other) const;
    constexpr inline bool operator!=(const VirtualAddress &other) const;

    /* Ordering */
    constexpr inline bool operator<(const VirtualAddress &other) const;
    constexpr inline bool operator>(const VirtualAddress &other) const;
    constexpr inline bool operator<=(const VirtualAddress &other) const;
    constexpr inline bool operator>=(const VirtualAddress &other) const;

    /* Offset arithmetic */
    constexpr inline VirtualAddress operator+(const uint64_t &offset) const;
    constexpr inline VirtualAddress operator-(const uint64_t &offset) const;
    inline void operator+=(const uint64_t &offset);
    inline void operator-=(const uint64_t &offset);

    /* Bitwise */
    constexpr inline VirtualAddress operator&(const uint64_t &mask) const;
    constexpr inline VirtualAddress operator|(const uint64_t &mask) const;
    constexpr inline VirtualAddress operator^(const uint64_t &mask) const;
    constexpr inline VirtualAddress operator>>(const uint64_t &size) const;
    constexpr inline VirtualAddress operator<<(const uint64_t &size) const;

    /* Distance arithmetic */
    constexpr inline int64_t operator-(const VirtualAddress &other) const;
};

constexpr VirtualAddress operator"" _vaddr(unsigned long long literal) {
    return VirtualAddress(literal);
}

// Implementations

constexpr VirtualAddress::VirtualAddress(uint64_t address) : data(address) {}

constexpr VirtualAddress::VirtualAddress() : data(0) {}

constexpr uint64_t VirtualAddress::get_raw() const {
    return data;
}

constexpr VirtualAddress::operator uint64_t() const {
    return this->get_raw();
}

constexpr VirtualAddress VirtualAddress::null() {
    return VirtualAddress(0x0);
}

constexpr bool VirtualAddress::is_null() const {
    return this->get_raw() == 0;
}

template<typename T>
constexpr bool VirtualAddress::is_aligned() const {
    return is_aligned_generic<decltype(this->data), T>(this->data);
}

/* Equality */

constexpr bool VirtualAddress::operator==(const VirtualAddress &other) const {
    return this->get_raw() == other.get_raw();
}

constexpr bool VirtualAddress::operator!=(const VirtualAddress &other) const {
    return this->get_raw() != other.get_raw();
}

/* Ordering */

constexpr bool VirtualAddress::operator<(const VirtualAddress &other) const {
    return this->get_raw() < other.get_raw();
}

constexpr bool VirtualAddress::operator>(const VirtualAddress &other) const {
    return this->get_raw() > other.get_raw();
}

constexpr bool VirtualAddress::operator<=(const VirtualAddress &other) const {
    return this->get_raw() <= other.get_raw();
}

constexpr bool VirtualAddress::operator>=(const VirtualAddress &other) const {
    return this->get_raw() >= other.get_raw();
}

/* Offset arithmetic */

constexpr VirtualAddress VirtualAddress::operator+(const uint64_t &offset) const {
    return VirtualAddress(this->get_raw() + offset);
}

constexpr VirtualAddress VirtualAddress::operator-(const uint64_t &offset) const {
    return VirtualAddress(this->get_raw() - offset);
}

void VirtualAddress::operator+=(const uint64_t &offset) {
    data += offset;
}

void VirtualAddress::operator-=(const uint64_t &offset) {
    data -= offset;
}

/* Bitwise */

constexpr VirtualAddress VirtualAddress::operator&(const uint64_t &mask) const {
    return VirtualAddress(this->get_raw() & mask);
}

constexpr VirtualAddress VirtualAddress::operator|(const uint64_t &mask) const {
    return VirtualAddress(this->get_raw() | mask);
}

constexpr VirtualAddress VirtualAddress::operator^(const uint64_t &mask) const {
    return VirtualAddress(this->get_raw() ^ mask);
}

constexpr VirtualAddress VirtualAddress::operator>>(const uint64_t &size) const {
    return VirtualAddress(this->get_raw() >> size);
}

constexpr VirtualAddress VirtualAddress::operator<<(const uint64_t &size) const {
    return VirtualAddress(this->get_raw() << size);
}

/* Distance arithmetic */

constexpr int64_t VirtualAddress::operator-(const VirtualAddress &other) const {
    return this->get_raw() - other.get_raw();
}

}

Q_DECLARE_METATYPE(machine::VirtualAddress)

#endif // VIRTUAL_ADDRESS_H
