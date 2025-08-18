#ifndef PAGE_FAULT_HANDLER_H
#define PAGE_FAULT_HANDLER_H

#include "core.h"
#include "memory/address.h"
#include "virtual_address.h"

namespace machine {
class TLB;
class Machine;

// Catches page-fault exceptions, invokes page allocation, and recovers execution.
class PageFaultHandler : public ExceptionHandler {
public:
    PageFaultHandler(Machine *m, TLB &tlb);

    bool handle_exception(
        Core *core,
        Registers *regs,
        ExceptionCause excause,
        Address inst_addr,
        Address next_addr,
        Address jump_branch_pc,
        Address fault_addr) override;



private:
    Machine *machine;
    TLB &tlb;

    void perform_page_allocation(const VirtualAddress &va, uint32_t raw_satp);
};

}

#endif // PAGE_FAULT_HANDLER_H
