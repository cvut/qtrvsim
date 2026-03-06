#ifndef PAGE_TABLE_WALKER_H
#define PAGE_TABLE_WALKER_H

#include "memory/frontend_memory.h"
#include "sv32.h"
#include "sv39.h"
#include "virtual_address.h"

#include <inttypes.h>
#include <memory>

namespace machine {

struct WalkResult {
    Address phys;
    Address pte_addr;
    std::unique_ptr<GenericPte> leaf_pte;
    bool pte_was_written = false;

    WalkResult() = default;
    WalkResult(Address phys_, std::unique_ptr<GenericPte> leaf_pte_)
        : phys(phys_)
        , leaf_pte(std::move(leaf_pte_)) {}
};

// Performs multi-level page-table walks (SV32) in memory to resolve a virtual address to a physical
// one.
class PageTableWalker {
public:
    explicit PageTableWalker(
        FrontendMemory *mem,
        std::function<std::unique_ptr<GenericPte>(uint64_t)> pte_factory = sv32_pte_factory());

    template<typename T, int max_level_idx>
    WalkResult walk(
        const VirtualAddress &va,
        uint64_t raw_satp,
        uint64_t raw_sstatus,
        const AccessMode &access_mode);

    static std::function<std::unique_ptr<GenericPte>(uint64_t)> sv32_pte_factory();
    static std::function<std::unique_ptr<GenericPte>(uint64_t)> sv39_pte_factory();

    [[nodiscard]] const std::function<std::unique_ptr<GenericPte>(uint64_t)> &pte_factory() const {
        return pte_factory_;
    }

    void set_pte_factory(std::function<std::unique_ptr<GenericPte>(uint64_t)> pte_factory) {
        pte_factory_ = std::move(pte_factory);
    }

private:
    FrontendMemory *memory;
    std::function<std::unique_ptr<GenericPte>(uint64_t)> pte_factory_;
};

} // namespace machine

#endif // PAGE_TABLE_WALKER_H
