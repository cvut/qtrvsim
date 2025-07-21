#ifndef PAGE_TABLE_WALKER_H
#define PAGE_TABLE_WALKER_H

#include "frontend_memory.h"
#include "virtual_address.h"
#include "sv32.h"
#include <inttypes.h>

namespace machine {

class PageTableWalker {
public:
    explicit PageTableWalker(FrontendMemory *mem) : memory(mem) {}

    Address walk(const VirtualAddress &va, uint32_t raw_satp) const;

private:
    FrontendMemory *memory;
};

}

#endif //PAGE_TABLE_WALKER_H
