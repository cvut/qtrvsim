#ifndef QTRVSIM_CSR_ADDRESS_H
#define QTRVSIM_CSR_ADDRESS_H

#include "common/math/bit_ops.h"
#include "simulator_exception.h"

#include <cstdint>

namespace machine { namespace CSR {
    /**
     * Spec vol. 2: Table 2.1
     */
    enum class PrivilegeLevel {
        UNPRIVILEGED = 0b00, //> Unprivileged and User-Level CSRs, unimplemented
        SUPERVISOR = 0b01,   //> Supervisor-Level CSRs, unimplemented
        HYPERVISOR = 0b10,   //> Hypervisor and VS CSRs, unimplemented
        MACHINE = 0b11,      //> Machine-Level CSRs
    };

    struct Address {
        constexpr explicit Address(uint16_t address) : data(address) {
            SANITY_ASSERT(
                address < (1 << 12), "CSR register address is out of the ISA "
                                     "specified range (12bits)");
        }

        constexpr Address(const Address &other) = default;
        constexpr Address &operator=(const Address &other) = default;

        uint16_t data;

        /*
         * By convention, the upper 4 bits of the CSR address (csr[11:8]) are used to encode the
         * read and write accessibility of the CSRs according to privilege level as shown in Table
         * 2.1.
         */

        /** The top two bits (csr[11:10]) indicate whether the register is read/write
         * (00, 01, or 10) or read-only (11).
         */
        [[nodiscard]] constexpr bool is_writable() const { return get_bits(data, 11, 10) != 0b11; }

        /**
         * The next two bits (csr[9:8]) encode the lowest privilege level that can access the CSR.
         */
        [[nodiscard]] constexpr PrivilegeLevel get_privilege_level() const {
            return static_cast<PrivilegeLevel>(get_bits(data, 9, 8));
        }

        bool operator<(const Address &rhs) const { return data < rhs.data; }
        bool operator>(const Address &rhs) const { return rhs < *this; }
        bool operator<=(const Address &rhs) const { return !(rhs < *this); }
        bool operator>=(const Address &rhs) const { return !(*this < rhs); }
        bool operator==(const Address &rhs) const { return data == rhs.data; }
        bool operator!=(const Address &rhs) const { return data != rhs.data; }
    };

    constexpr Address operator"" _csr(unsigned long long literal) {
        return Address(literal);
    }
}} // namespace machine::CSR

template<>
struct std::hash<machine::CSR::Address> {
    std::size_t operator()(machine::CSR::Address const &addr) const noexcept {
        return std::hash<decltype(addr.data)> {}(addr.data);
    }
};

#endif // QTRVSIM_CSR_ADDRESS_H
