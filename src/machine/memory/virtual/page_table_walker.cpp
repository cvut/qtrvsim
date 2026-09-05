#include "page_table_walker.h"

#include "common/logging.h"
#include "machine.h"

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

template<typename PagingMode, int max_level_idx>
WalkResult PageTableWalker::walk(
    const VirtualAddress &va,
    uint64_t raw_satp,
    uint64_t raw_sstatus,
    const AccessMode &access_mode,
    AccessEffects ae_type) {
    if (!(raw_satp & PagingMode::SATP_MODE_MASK)) {
        return WalkResult { Address { va.get_raw() }, nullptr };
    }

    uint64_t ppn = raw_satp & PagingMode::SATP_PPN_MASK;
    uint64_t va_raw = va.get_raw();
    WalkResult res;

    res.pte_was_written = false;

    for (int lvl = max_level_idx; lvl >= 0; --lvl) {
        uint32_t vpn_idx = (va_raw >> (PagingMode::PAGE_SHIFT + (lvl * PagingMode::VPN_BITS)))
                           & PagingMode::VPN_MASK;
        Address pte_addr { (ppn << PagingMode::PAGE_SHIFT)
                           + (vpn_idx * sizeof(typename PagingMode::RawType)) };
        typename PagingMode::RawType raw_pte = 0;
        switch (sizeof(raw_pte)) {
        case 4: raw_pte = memory->read_u32(pte_addr, ae_type); break;
        case 8: raw_pte = memory->read_u64(pte_addr, ae_type); break;
        default: assert(0);
        }
        DEBUG(
            "PTW: L%u PTE@0x%08" PRIx64 " = 0x%08" PRIx64, lvl, pte_addr.get_raw(),
            (uint64_t)raw_pte);
        std::unique_ptr<GenericPte> pte = pte_factory_(uint64_t(raw_pte));

        if (!pte->is_valid()) {
            throw SIMULATOR_EXCEPTION(
                PageFault, "PTW: page fault, leaf PTE invalid",
                QString::number(pte_addr.get_raw(), 16), get_current_cause(access_mode.opkind()),
                Address(va.get_raw()));
        }
        if (pte->is_leaf()) {
            uint64_t mask = (1ull << (lvl * PagingMode::VPN_BITS)) - 1;
            if (lvl > 0 && (pte->ppn() & mask) != 0) {
                throw SIMULATOR_EXCEPTION(
                    PageFault, "PTW: misaligned superpage", "",
                    get_current_cause(access_mode.opkind()), Address(va.get_raw()));
            }

            if (!check_permissions(*pte, raw_sstatus, access_mode.priv(), access_mode.opkind())) {
                throw SIMULATOR_EXCEPTION(
                    PageFault, "PTW: access fault (permission check failed)", "",
                    get_current_cause(access_mode.opkind()), Address(va.get_raw()));
            }
            if (!pte->d() && access_mode.opkind() == AccessOp::WRITE) {
                pte->set_d(true);
                res.pte_was_written = true;
            }
            if (!pte->a() && ae_type != ae::INTERNAL) {
                pte->set_a(true);
                res.pte_was_written = true;
            }
            if (res.pte_was_written) {
                switch (sizeof(raw_pte)) {
                case 4: memory->write_u32(pte_addr, uint32_t(pte->raw), ae_type); break;
                case 8: memory->write_u64(pte_addr, uint64_t(pte->raw), ae_type); break;
                default: assert(0);
                }
            }

            Address pa = pte->make_phys(va_raw, lvl);
            DEBUG("PTW: L%u leaf → PA=0x%08" PRIx64, lvl, pa.get_raw());
            res.phys = pa;
            res.pte_addr = pte_addr;
            res.leaf_pte = std::move(pte);
            return res;
        }

        if (pte->r() || pte->w() || pte->x()) {
            throw SIMULATOR_EXCEPTION(
                PageFault, "PTW: invalid non-leaf", QString::number(raw_pte, 16),
                get_current_cause(access_mode.opkind()), Address(va.get_raw()));
        }

        ppn = pte->ppn();
    }

    throw SIMULATOR_EXCEPTION(
        PageFault, "PTW: no leaf found", "", get_current_cause(access_mode.opkind()),
        Address(va.get_raw()));
}

template WalkResult PageTableWalker::walk<Sv32Pte, 1>(
    const VirtualAddress &va,
    uint64_t raw_satp,
    uint64_t raw_sstatus,
    const AccessMode &access_mode,
    AccessEffects ae_type);
template WalkResult PageTableWalker::walk<Sv39Pte, 2>(
    const VirtualAddress &va,
    uint64_t raw_satp,
    uint64_t raw_sstatus,
    const AccessMode &access_mode,
    AccessEffects ae_type);

template bool check_permissions<GenericPte>(
    const GenericPte &pte,
    uint64_t raw_sstatus,
    CSR::PrivilegeLevel priv,
    AccessOp op);

} // namespace machine
