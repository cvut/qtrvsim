#ifndef PAGE_TABLE_WALKER_H
#define PAGE_TABLE_WALKER_H

#include "memory/frontend_memory.h"
#include "sv32.h"
#include "virtual_address.h"

#include <inttypes.h>

namespace machine {

// Performs multi-level page-table walks (SV32) in memory to resolve a virtual address to a physical one.
class PageTableWalker {
public:
    explicit PageTableWalker(FrontendMemory *mem) : memory(mem) {}

    Address walk(const VirtualAddress &va, uint32_t raw_satp) const;

private:
    FrontendMemory *memory;
};

}

#endif //PAGE_TABLE_WALKER_H
