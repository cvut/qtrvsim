#include "page_table_walker.h"

#include "common/logging.h"
#include "machine.h"
#include "memory/backend/backend_memory.h"

#include <inttypes.h>

LOG_CATEGORY("machine.PTW");

namespace machine {

Address PageTableWalker::walk(const VirtualAddress &va, uint32_t raw_satp) const {
    if (((raw_satp >> 31) & 1) == 0) return Address { va.get_raw() };

    auto root_ppn = raw_satp & ((1u << 22) - 1);
    auto va_raw = static_cast<uint32_t>(va.get_raw());
    auto vpn1 = (va_raw >> VPN1_SHIFT) & VPN_MASK;
    auto vpn0 = (va_raw >> VPN0_SHIFT) & VPN_MASK;
    auto ppn = root_ppn;

    for (int lvl = 1; lvl >= 0; --lvl) {
        uint32_t idx = (lvl == 1 ? vpn1 : vpn0);
        Address pte_addr { (uint64_t(ppn) << PAGE_SHIFT) + idx * 4 };

        uint32_t raw_pte;
        memory->read(&raw_pte, pte_addr, sizeof(raw_pte), { .type = ae::INTERNAL });
        LOG("PTW: L%u PTE@0x%08" PRIx64 " = 0x%08x", lvl, pte_addr.get_raw(), raw_pte);

        Sv32Pte pte = Sv32Pte::from_uint(raw_pte);

        if (!pte.is_valid()) {
            throw SIMULATOR_EXCEPTION(
                PageFault, "PTW: page fault, leaf PTE invalid",
                QString::number(pte_addr.get_raw(), 16));
        }

        if (pte.is_leaf()) {
            Address pa = make_phys(va_raw, pte, lvl);
            LOG("PTW: L%u leaf → PA=0x%08" PRIx64, lvl, pa.get_raw());
            return pa;
        }

        if (pte.r() || pte.w() || pte.x()) {
            throw SIMULATOR_EXCEPTION(
                PageFault, "PTW: invalid non-leaf", QString::number(raw_pte, 16));
        }
        ppn = unsigned(pte.ppn());
    }

    throw SIMULATOR_EXCEPTION(PageFault, "PTW: no leaf found", "");
}

} // namespace machine
