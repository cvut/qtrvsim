#include "page_fault_handler.h"

#include "common/logging.h"
#include "machine.h"
#include "simulator_exception.h"
#include "sv32.h"

LOG_CATEGORY("machine.PageFaultHandler");

using namespace machine;

PageFaultHandler::PageFaultHandler(Machine *m, TLB &tlb_) : machine(m), tlb(tlb_) {}

bool PageFaultHandler::handle_exception(
    Core *core,
    Registers * /*regs*/,
    ExceptionCause excause,
    Address /*inst_addr*/,
    Address /*next_addr*/,
    Address /*jump_branch_pc*/,
    Address fault_addr) {
    if (excause != EXCAUSE_INSN_PAGE_FAULT && excause != EXCAUSE_LOAD_PAGE_FAULT
        && excause != EXCAUSE_STORE_PAGE_FAULT) {
        return true;
    }

    VirtualAddress va(fault_addr.get_raw());
    uint32_t raw_satp = core->get_control_state()->read_internal(CSR::Id::SATP).as_u32();
    uint16_t asid = (raw_satp >> 22) & 0x1FF;
    LOG("PageFaultHandler: allocating page for VA=0x%08" PRIx64, va.get_raw());
    perform_page_allocation(va, raw_satp);
    tlb.flush_single(va, asid);
    (void)tlb.translate_virtual_to_physical(Address { va.get_raw() });

    return false;
}

void PageFaultHandler::perform_page_allocation(const VirtualAddress &va, uint32_t raw_satp) {
    constexpr unsigned PAGE_SHIFT = 12;
    constexpr uint32_t VPN_MASK = (1u << VPN_BITS) - 1;

    uint32_t root_ppn = raw_satp & ((1u << PPN_BITS) - 1);

    uint32_t va_raw = static_cast<uint32_t>(va.get_raw());
    uint32_t vpn1 = (va_raw >> VPN1_SHIFT) & VPN_MASK;
    uint32_t vpn0 = (va_raw >> VPN0_SHIFT) & VPN_MASK;

    uint32_t current_ppn = root_ppn;
    Address pte_addr;
    Sv32Pte pte;

    for (int lvl = 1; lvl >= 0; --lvl) {
        uint32_t idx = (lvl == 1 ? vpn1 : vpn0);
        pte_addr = Address { (uint64_t(current_ppn) << PAGE_SHIFT) + idx * sizeof(uint32_t) };

        uint32_t raw;
        machine->memory_rw()->read(
            &raw, Offset(pte_addr.get_raw()), sizeof(raw), { .type = ae::INTERNAL });
        pte = Sv32Pte::from_uint(raw);

        if (!pte.is_valid()) {
            if (lvl == 0) { break; }
            uint32_t new_ppn = machine->allocate_page();
            Sv32Pte new_pte {};
            new_pte.v = 1;
            new_pte.ppn = new_ppn;
            uint32_t bits = new_pte.to_uint();
            machine->memory_rw()->write(
                Offset(pte_addr.get_raw()), &bits, sizeof(bits), { .type = ae::INTERNAL });

            LOG("PageFaultHandler: L%u PT alloc PPN=0x%x for VA=0x%08" PRIx64, lvl - 1, new_ppn,
                va.get_raw());

            current_ppn = new_ppn;
            continue;
        }

        if (pte.is_leaf()) {
            throw SIMULATOR_EXCEPTION(
                PageFault, "Unexpected leaf PTE during page table creation",
                QString::number(pte_addr.get_raw(), 16));
        }
        current_ppn = pte.ppn;
    }

    uint32_t data_ppn = machine->allocate_page();
    Sv32Pte leaf {};
    leaf.v = leaf.r = leaf.w = leaf.x = leaf.a = leaf.d = 1;
    leaf.ppn = data_ppn;

    uint32_t leaf_bits = leaf.to_uint();
    machine->memory_rw()->write(
        Offset(pte_addr.get_raw()), &leaf_bits, sizeof(leaf_bits), { .type = ae::INTERNAL });

    LOG("PageFaultHandler: mapped VA=0x%08" PRIx64 " → PPN=0x%x", va.get_raw(), data_ppn);
}
