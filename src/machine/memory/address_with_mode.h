#ifndef ADDRESS_WITH_MODE_H
#define ADDRESS_WITH_MODE_H

#include <utility>
#include <cstdint>
#include <QMetaType>
#include "address.h"
#include "csr/address.h"

namespace machine {

struct AccessMode {
    uint32_t token = 0;

    AccessMode() = default;
    explicit AccessMode(uint32_t t) : token(t) {}

    static AccessMode pack(uint16_t asid, CSR::PrivilegeLevel priv, bool uncached = false) {
        uint32_t t = (static_cast<uint32_t>(priv) & 0x3u)
            | ((static_cast<uint32_t>(asid) & 0xFFFFu) << 2)
            | (uncached ? (1u << 18) : 0u);
        return AccessMode(t);
    }
    uint16_t asid() const { return static_cast<uint16_t>((token >> 2) & 0xFFFFu); }
    CSR::PrivilegeLevel priv() const {
        return static_cast<CSR::PrivilegeLevel>(token & 0x3u);
    }
    bool uncached() const { return ((token >> 18) & 0x1u) != 0; }
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
