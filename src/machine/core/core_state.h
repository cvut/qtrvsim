#ifndef QTRVSIM_CORE_STATE_H
#define QTRVSIM_CORE_STATE_H

#include "common/memory_ownership.h"
#include "machinedefs.h"
#include "memory/address_range.h"
#include "pipeline.h"

#include <QMap>
#include <cstdint>
#include <machineconfig.h>
using std::uint32_t;

namespace machine {

struct CoreState {
    Pipeline pipeline = {};
    AddressRange LoadReservedRange;
    uint32_t stall_count = 0;
    uint32_t cycle_count = 0;
    unsigned current_privilege_u = static_cast<unsigned>(CSR::PrivilegeLevel::MACHINE);
    unsigned current_asid_u = 0u;

    [[nodiscard]] CSR::PrivilegeLevel current_privilege() const noexcept {
        return static_cast<CSR::PrivilegeLevel>(current_privilege_u);
    }

    void set_current_privilege(CSR::PrivilegeLevel p) noexcept {
        current_privilege_u = static_cast<unsigned>(p);
    }

    [[nodiscard]] uint16_t current_asid() const noexcept {
        return static_cast<uint16_t>(current_asid_u & 0x1FFu);
    }

    void set_current_asid(uint16_t a) noexcept {
        current_asid_u = static_cast<unsigned>(a & 0x1FFu);
    }
};

} // namespace machine
#endif // QTRVSIM_CORE_STATE_H
