#include "tlb.h"

#include "common/logging.h"
#include "csr/controlstate.h"
#include "machine.h"

#include <iostream>
#include <unordered_map>

LOG_CATEGORY("machine.TLB");

namespace machine {

static uint64_t make_key(VirtualAddress va, uint16_t asid) {
    uint64_t vpn = va.get_raw() >> 12;
    return (uint64_t(asid) << 48) | (vpn & 0x0000FFFFFFFFFFFFULL);
}

TLB::TLB(FrontendMemory *memory, TLBType type_, Machine *mach)
    : FrontendMemory(memory->simulated_machine_endian)
    , mem(memory)
    , ptw(memory)
    , type(type_)
    , machine(mach) {
    const char *tag = (type == PROGRAM ? "I" : "D");
    LOG("TLB[%s] constructed, backing=%p", tag, memory);
}

void TLB::on_csr_write(size_t internal_id, RegisterValue val) {
    if (internal_id != CSR::Id::SATP) return;
    current_satp_raw = static_cast<uint32_t>(val.as_u64());
    cache.clear();
    LOG("TLB: SATP changed → flushed all; new SATP=0x%08x", current_satp_raw);
}

void TLB::flush_single(VirtualAddress va, uint16_t asid) {
    uint64_t key = make_key(va, asid);
    if (cache.erase(key)) {
        const char *tag = (type == PROGRAM ? "I" : "D");
        LOG("TLB[%s]: flushed VA=0x%llx ASID=%u", tag, (unsigned long long)va.get_raw(), asid);
    }
}

WriteResult TLB::translate_and_write(Address dst, const void *src, size_t sz, WriteOptions opts) {
    if (first_instr_written) {
        first_instr_written = false;
        emit firstWrite(VirtualAddress { uint64_t(dst) });
    }

    Address pa = translate_virtual_to_physical(dst);
    return mem->write(pa, src, sz, opts);
}

ReadResult TLB::translate_and_read(void *dst, Address src, size_t sz, ReadOptions opts) {
    Address pa = translate_virtual_to_physical(src);
    return mem->read(dst, pa, sz, opts);
}

WriteResult TLB::write_vaddr(VirtualAddress dst, const void *src, size_t sz, WriteOptions opts) {
    Address a { uint64_t(dst) };
    return translate_and_write(a, src, sz, opts);
}

ReadResult TLB::read_vaddr(void *dst, VirtualAddress src, size_t sz, ReadOptions opts) const {
    return const_cast<TLB *>(this)->translate_and_read(dst, Address { uint64_t(src) }, sz, opts);
}

uint32_t TLB::get_change_counter() const {
    return mem->get_change_counter();
}

Address TLB::translate_virtual_to_physical(Address vaddr) {
    if (uint32_t mode = (current_satp_raw >> 31) & 1; mode == 0) return vaddr;

    uint64_t virt = vaddr.get_raw();

    VirtualAddress va { virt };
    uint16_t asid = (current_satp_raw >> 22) & 0x1FF;
    uint64_t key = make_key(va, asid);
    uint64_t page_off = virt & 0xFFF;
    const char *tag = (type == PROGRAM ? "I" : "D");

    if (auto it = cache.find(key); it != cache.end()) {
        uint64_t base = it->second.phys.get_raw() & ~0xFFFULL;
        return Address { base + page_off };
    }

    Address leaf_pa;
    try {
        leaf_pa = ptw.walk(va, current_satp_raw);
    } catch (const SimulatorExceptionPageFault &e) {
        auto root_ppn = current_satp_raw & ((1u << 22) - 1);
        auto va_raw = static_cast<uint32_t>(va.get_raw());
        auto vpn1 = (va_raw >> VPN1_SHIFT) & VPN_MASK;
        auto vpn0 = (va_raw >> VPN0_SHIFT) & VPN_MASK;

        uint32_t current_ppn = root_ppn;
        Address current_pte_addr;
        Sv32Pte current_pte;

        for (int lvl = 1; lvl >= 0; --lvl) {
            uint32_t idx = (lvl == 1 ? vpn1 : vpn0);
            current_pte_addr = Address { (uint64_t(current_ppn) << PAGE_SHIFT) + idx * 4 };

            uint32_t raw_pte;
            mem->read(&raw_pte, current_pte_addr, sizeof(raw_pte), { .type = ae::INTERNAL });
            current_pte = Sv32Pte::from_uint(raw_pte);

            if (!current_pte.is_valid()) {
                if (lvl == 0) {
                    break;
                } else {
                    uint32_t new_l0_table_ppn = machine->allocate_page();

                    Sv32Pte new_l1_pte {};
                    new_l1_pte.v = 1;
                    new_l1_pte.ppn = new_l0_table_ppn;
                    uint32_t bits = new_l1_pte.to_uint();
                    mem->write(current_pte_addr, &bits, sizeof(bits), { .type = ae::INTERNAL });
                    LOG("TLB[%s]: allocated L%u PT PPN=0x%x for VA=0x%llx", tag, lvl - 1,
                        new_l0_table_ppn, (unsigned long long)va.get_raw());
                    current_ppn = new_l0_table_ppn;
                }
            } else if (current_pte.is_leaf()) {
                throw SIMULATOR_EXCEPTION(
                    PageFault, "PTW: unexpected leaf PTE during page table creation",
                    QString::number(current_pte_addr.get_raw(), 16));
            } else {
                current_ppn = current_pte.ppn;
            }
        }

        uint32_t new_data_ppn = machine->allocate_page();

        Sv32Pte final_leaf_pte {};
        final_leaf_pte.v = 1; // Valid
        final_leaf_pte.r = 1; // Read
        final_leaf_pte.w = 1; // Write
        final_leaf_pte.x = 1; // Execute
        final_leaf_pte.u = 0; // Kernel mode
        final_leaf_pte.g = 0; // Not global
        final_leaf_pte.a = 1; // Accessed
        final_leaf_pte.d = 1; // Dirty
        final_leaf_pte.ppn = new_data_ppn;
        uint32_t bits = final_leaf_pte.to_uint();
        mem->write(current_pte_addr, &bits, sizeof(bits), { .type = ae::INTERNAL });

        LOG("TLB[%s]: allocated data/instr PPN=0x%x for VA=0x%llx", tag, new_data_ppn,
            (unsigned long long)va.get_raw());

        leaf_pa = ptw.walk(va, current_satp_raw);
    }

    cache[key] = Entry { leaf_pa, asid };
    uint64_t phys_base = leaf_pa.get_raw() & ~0xFFFULL;
    return Address { phys_base + page_off };
}

} // namespace machine
