#ifndef ADDRESS_WITH_MODE_H
#define ADDRESS_WITH_MODE_H

#include "address.h"
#include "csr/address.h"
#include "memory_utils.h"

#include <QMetaType>
#include <cstdint>
#include <utility>

namespace machine {

// field masks / shifts
static constexpr uint32_t PRIV_MASK = 0x3u; // 2 bits
static constexpr int PRIV_SHIFT = 0;
static constexpr uint32_t ASID_MASK = 0xFFFFu;  // 16 bits
static constexpr int ASID_SHIFT = 2;            // bits 2..17
static constexpr uint32_t UNCACHED_MASK = 0x3u; // 2 bits
static constexpr int UNCACHED_SHIFT = 18;       // bits 18..19
static constexpr uint32_t OPKIND_MASK = 0xFFu;  // 8 bits
static constexpr int OPKIND_SHIFT = 20;         // bits 20..27

struct AccessMode {
    uint32_t token = 0;

    AccessMode() = default;
    explicit AccessMode(uint32_t t) : token(t) {}

    static AccessMode
    pack(uint16_t asid, CSR::PrivilegeLevel priv, AccessOp opkind, uint8_t uncached = 0) {
        uint32_t t = ((static_cast<uint32_t>(priv) & PRIV_MASK) << PRIV_SHIFT)
                     | ((static_cast<uint32_t>(asid) & ASID_MASK) << ASID_SHIFT)
                     | ((static_cast<uint32_t>(uncached) & UNCACHED_MASK) << UNCACHED_SHIFT)
                     | ((static_cast<uint32_t>(static_cast<uint8_t>(opkind)) & OPKIND_MASK)
                        << OPKIND_SHIFT);
        return AccessMode(t);
    }

    uint16_t asid() const { return static_cast<uint16_t>((token >> ASID_SHIFT) & ASID_MASK); }

    CSR::PrivilegeLevel priv() const {
        return static_cast<CSR::PrivilegeLevel>((token >> PRIV_SHIFT) & PRIV_MASK);
    }

    uint8_t uncached() const {
        return static_cast<uint8_t>((token >> UNCACHED_SHIFT) & UNCACHED_MASK);
    }

    AccessOp opkind() const { return static_cast<AccessOp>((token >> OPKIND_SHIFT) & OPKIND_MASK); }

    uint32_t raw() const { return token; }
};

class AddressWithMode : public Address {
private:
    AccessMode mode;

public:
    constexpr AddressWithMode() : Address(), mode() {}

    constexpr explicit AddressWithMode(uint64_t addr, AccessMode m = {}) : Address(addr), mode(m) {}

    constexpr AddressWithMode(const Address &addr, AccessMode m = {}) : Address(addr), mode(m) {}

    constexpr AddressWithMode(const AddressWithMode &other) = default;
    AddressWithMode &operator=(const AddressWithMode &other) = default;

    constexpr AccessMode access_mode() const noexcept { return mode; }
    void set_access_mode(AccessMode m) noexcept { mode = m; }
    uint32_t access_mode_raw() const noexcept { return mode.raw(); }

    constexpr bool operator==(const AddressWithMode &other) const noexcept {
        return static_cast<const Address&>(*this) == static_cast<const Address&>(other)
               && mode.raw() == other.mode.raw();
    }
    constexpr bool operator!=(const AddressWithMode &other) const noexcept { return !(*this == other); }

    constexpr std::pair<Address, AccessMode> unpack() const noexcept {
        return { Address(get_raw()), mode };
    }
};

} // namespace machine

Q_DECLARE_METATYPE(machine::AddressWithMode)

#endif // ADDRESS_WITH_MODE_H
