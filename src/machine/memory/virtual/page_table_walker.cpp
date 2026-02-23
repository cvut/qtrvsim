#include "page_table_walker.h"

#include "common/logging.h"
#include "machine.h"
#include "memory/backend/backend_memory.h"

#include <inttypes.h>

LOG_CATEGORY("machine.PTW");

namespace machine {

PageTableWalker::PageTableWalker(
    FrontendMemory *mem,
    std::function<std::unique_ptr<GenericPte>(uint64_t)> pte_factory)
    : memory(mem)
    , pte_factory_(std::move(pte_factory)) {}

std::function<std::unique_ptr<GenericPte>(uint64_t)> PageTableWalker::sv32_pte_factory() {
    return
        [](uint64_t raw) -> std::unique_ptr<GenericPte> { return std::make_unique<Sv32Pte>(raw); };
}

std::function<std::unique_ptr<GenericPte>(uint64_t)> PageTableWalker::sv39_pte_factory() {
    return
        [](uint64_t raw) -> std::unique_ptr<GenericPte> { return std::make_unique<Sv39Pte>(raw); };
}

template<typename T, int max_level_idx>
Address PageTableWalker::walk(const VirtualAddress &va, uint64_t raw_satp) {
    if (!(raw_satp & T::SATP_MODE_MASK)) { return Address { va.get_raw() }; }

    uint64_t ppn = raw_satp & T::SATP_PPN_MASK;
    uint64_t va_raw = va.get_raw();

    for (int lvl = max_level_idx; lvl >= 0; --lvl) {
        // Calculate the index for the current level
        // Sv32: Level 1 starts at bit 22, Level 0 at 12 (Shift = 12 + lvl * 10)
        // Sv39: Level 2 starts at bit 30, Level 1 at 21, Level 0 at 12 (Shift = 12 + lvl * 9)
        uint32_t vpn_idx = (va_raw >> (T::PAGE_SHIFT + (lvl * T::VPN_BITS))) & T::VPN_MASK;

        Address pte_addr { (ppn << T::PAGE_SHIFT) + (vpn_idx * sizeof(typename T::RawType)) };
        typename T::RawType raw_pte = 0;
        memory->read(&raw_pte, pte_addr, sizeof(raw_pte), { .type = ae::INTERNAL });
        DEBUG(
            "PTW: L%u PTE@0x%08" PRIx64 " = 0x%08" PRIx64, lvl, pte_addr.get_raw(),
            (uint64_t)raw_pte);
        std::unique_ptr<GenericPte> pte = pte_factory_(uint64_t(raw_pte));

        if (!pte->is_valid()) {
            throw SIMULATOR_EXCEPTION(
                PageFault, "PTW: page fault, leaf PTE invalid",
                QString::number(pte_addr.get_raw(), 16));
        }
        if (pte->is_leaf()) {
            uint64_t mask = (1ull << (lvl * T::VPN_BITS)) - 1;
            if (lvl > 0 && (pte->ppn() & mask) != 0) {
                throw SIMULATOR_EXCEPTION(PageFault, "PTW: misaligned superpage", "");
            }
            Address pa = pte->make_phys(va_raw, lvl);
            DEBUG("PTW: L%u leaf → PA=0x%08" PRIx64, lvl, pa.get_raw());
            return pa;
        }

        if (pte->r() || pte->w() || pte->x()) {
            throw SIMULATOR_EXCEPTION(
                PageFault, "PTW: invalid non-leaf", QString::number(raw_pte, 16));
        }
        ppn = pte->ppn();
    }

    throw SIMULATOR_EXCEPTION(PageFault, "PTW: no leaf found", "");
}

template Address PageTableWalker::walk<Sv32Pte, 1>(const VirtualAddress &va, uint64_t raw_satp);
template Address PageTableWalker::walk<Sv39Pte, 2>(const VirtualAddress &va, uint64_t raw_satp);

} // namespace machine
