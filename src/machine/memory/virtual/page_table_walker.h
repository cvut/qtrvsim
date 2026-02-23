#ifndef PAGE_TABLE_WALKER_H
#define PAGE_TABLE_WALKER_H

#include "memory/frontend_memory.h"
#include "sv32.h"
#include "sv39.h"
#include "virtual_address.h"

#include <inttypes.h>
#include <memory>

namespace machine {

// Performs multi-level page-table walks (SV32) in memory to resolve a virtual address to a physical
// one.
class PageTableWalker {
public:
    explicit PageTableWalker(
        FrontendMemory *mem,
        std::function<std::unique_ptr<GenericPte>(uint64_t)> pte_factory = sv32_pte_factory());

    template<typename T, int max_level_idx>
    Address walk(const VirtualAddress &va, uint64_t raw_satp);
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
