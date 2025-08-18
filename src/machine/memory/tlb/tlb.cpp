#include "tlb.h"

#include "csr/controlstate.h"
#include "machine.h"
#include "simulator_exception.h"

LOG_CATEGORY("machine.TLB");

namespace machine {

static bool is_mmio_region(uint64_t virt) {
    if (virt >= 0xFFFFC000u && virt <= 0xFFFFC1FFu) return true;
    if (virt >= 0xFFE00000u && virt <= 0xFFE4AFFFu) return true;
    if (virt >= 0xFFFD0000u && virt <= 0xFFFDBFFFu) return true;
    return false;
}

static Address bypass_mmio(Address vaddr) {
    return vaddr; // VA == PA for devices
}

TLB::TLB(FrontendMemory *memory, TLBType type_, Machine *mach)
    : FrontendMemory(memory->simulated_machine_endian)
    , mem(memory)
    , ptw(memory)
    , type(type_)
    , machine(mach) {
    auto &cfg = machine->config();
    num_sets_ = cfg.get_tlb_num_sets();
    associativity_ = cfg.get_tlb_associativity();
    auto pol = cfg.get_tlb_replacement_policy();
    repl_policy = make_tlb_policy(static_cast<TLBPolicyKind>(pol), associativity_, num_sets_);
    table.assign(num_sets_, std::vector<Entry>(associativity_));
    const char *tag = (type == PROGRAM ? "I" : "D");
    LOG("TLB[%s] constructed; sets=%zu way=%zu", tag, num_sets_, associativity_);
}

void TLB::on_csr_write(size_t internal_id, RegisterValue val) {
    if (internal_id != CSR::Id::SATP) return;
    current_satp_raw = static_cast<uint32_t>(val.as_u64());
    for (size_t s = 0; s < num_sets_; s++) {
        for (size_t w = 0; w < associativity_; w++) {
            table[s][w].valid = false;
        }
    }
    LOG("TLB: SATP changed → flushed all; new SATP=0x%08x", current_satp_raw);
}

void TLB::flush_single(VirtualAddress va, uint16_t asid) {
    uint64_t vpn = va.get_raw() >> 12;
    size_t s = set_index(vpn);
    for (size_t w = 0; w < associativity_; w++) {
        auto &e = table[s][w];
        if (e.valid && e.vpn == vpn && e.asid == asid) {
            e.valid = false;
            const char *tag = (type == PROGRAM ? "I" : "D");
            LOG("TLB[%s]: flushed VA=0x%llx ASID=%u", tag, (unsigned long long)va.get_raw(), asid);
        }
    }
}

Address TLB::translate_virtual_to_physical(Address vaddr) {
    uint64_t virt = vaddr.get_raw();
    if (is_mmio_region(virt)) { return bypass_mmio(vaddr); }

    if (((current_satp_raw >> 31) & 1) == 0) { return vaddr; }

    VirtualAddress va { virt };
    uint16_t asid = (current_satp_raw >> 22) & 0x1FF;
    uint64_t vpn = virt >> 12;
    uint64_t off = virt & ((1ULL << PAGE_SHIFT) - 1);
    size_t s = set_index(vpn);
    const char *tag = (type == PROGRAM ? "I" : "D");

    for (size_t w = 0; w < associativity_; w++) {
        auto &e = table[s][w];
        if (e.valid && e.vpn == vpn && e.asid == asid) {
            repl_policy->notify_access(s, w, /*valid=*/true);
            uint64_t pbase = e.phys.get_raw() & ~((1ULL << PAGE_SHIFT) - 1);
            return Address { pbase + off };
        }
    }

    Address leaf_pa;
    try {
        leaf_pa = ptw.walk(va, current_satp_raw);
    } catch (const SimulatorExceptionPageFault &pf) {
        if (pf_handler) {
            auto *rw_core = const_cast<Core *>(machine->core());
            auto *rw_regs = const_cast<Registers *>(machine->registers());
            pf_handler->handle_exception(
                rw_core, rw_regs,
                (type == PROGRAM ? EXCAUSE_INSN_PAGE_FAULT : EXCAUSE_LOAD_PAGE_FAULT),
                Address { 0 }, Address { 0 }, Address { 0 }, vaddr);
        }
        flush_single(va, asid);
        leaf_pa = ptw.walk(va, current_satp_raw);
    }

    uint64_t new_ppn = leaf_pa.get_raw() >> 12;
    size_t victim = repl_policy->select_way(s);

    auto &ent = table[s][victim];
    ent.valid = true;
    ent.asid = asid;
    ent.vpn = vpn;
    ent.phys = Address { new_ppn << PAGE_SHIFT };
    repl_policy->notify_access(s, victim, /*valid=*/true);

    LOG("TLB[%s]: mapped VA=0x%llx → PA=0x%llx (ASID=%u)", tag, (unsigned long long)virt,
        (unsigned long long)(new_ppn << PAGE_SHIFT), asid);

    return Address { (new_ppn << PAGE_SHIFT) + off };
}

WriteResult TLB::translate_and_write(Address dst, const void *src, size_t sz, WriteOptions opts) {
    if (first_instr_written) {
        first_instr_written = false;
        emit firstWrite(VirtualAddress { uint64_t(dst) }); // Update pc
    }
    Address pa = translate_virtual_to_physical(dst);
    return mem->write(pa, src, sz, opts);
}

ReadResult TLB::translate_and_read(void *dst, Address src, size_t sz, ReadOptions opts) {
    Address pa = translate_virtual_to_physical(src);
    return mem->read(dst, pa, sz, opts);
}

WriteResult TLB::write_vaddr(VirtualAddress dst, const void *src, size_t sz, WriteOptions opts) {
    return translate_and_write(Address { uint64_t(dst) }, src, sz, opts);
}

ReadResult TLB::read_vaddr(void *dst, VirtualAddress src, size_t sz, ReadOptions opts) const {
    return const_cast<TLB *>(this)->translate_and_read(dst, Address { uint64_t(src) }, sz, opts);
}

bool TLB::reverse_lookup(Address paddr, VirtualAddress &out_va) const {
    uint64_t ppn = paddr.get_raw() >> 12;
    uint64_t offset = paddr.get_raw() & 0xFFF;
    for (size_t s = 0; s < num_sets_; s++) {
        for (size_t w = 0; w < associativity_; w++) {
            auto &e = table[s][w];
            if (e.valid && (e.phys.get_raw() >> 12) == ppn) {
                out_va = VirtualAddress { (e.vpn << 12) | offset };
                return true;
            }
        }
    }
    return false;
}

} // namespace machine
