#include "tlb.h"

#include "csr/controlstate.h"
#include "machine.h"
#include "memory/virtual/page_table_walker.h"
#include "memory/virtual/sv32.h"

LOG_CATEGORY("machine.TLB");

namespace machine {

TLB::TLB(
    FrontendMemory *memory,
    TLBType type_,
    const TLBConfig *config,
    bool vm_enabled,
    uint32_t memory_access_penalty_r,
    uint32_t memory_access_penalty_w,
    uint32_t memory_access_penalty_b,
    bool memory_access_enable_b)
    : FrontendMemory(memory->simulated_machine_endian)
    , mem(memory)
    , type(type_)
    , tlb_config(config)
    , vm_enabled(vm_enabled)
    , access_pen_r(memory_access_penalty_r)
    , access_pen_w(memory_access_penalty_w)
    , access_pen_b(memory_access_penalty_b)
    , access_ena_b(memory_access_enable_b) {
    num_sets_ = tlb_config.get_tlb_num_sets();
    associativity_ = tlb_config.get_tlb_associativity();
    auto pol = tlb_config.get_tlb_replacement_policy();
    repl_policy = make_tlb_policy(static_cast<TLBPolicyKind>(pol), associativity_, num_sets_);
    table.assign(num_sets_, std::vector<Entry>(associativity_));
    const char *tag = (type == PROGRAM ? "I" : "D");
    LOG("TLB[%s] constructed; sets=%zu way=%zu", tag, num_sets_, associativity_);

    emit hit_update(hit_count_);
    emit miss_update(miss_count_);
    emit memory_reads_update(mem_reads);
    emit memory_writes_update(mem_writes);
    update_all_statistics();
}

void TLB::on_csr_write(size_t internal_id, RegisterValue val) {
    if (internal_id != CSR::Id::SATP) return;
    current_satp_raw = static_cast<uint32_t>(val.as_u64());
    for (size_t s = 0; s < num_sets_; s++) {
        for (size_t w = 0; w < associativity_; w++) {
            auto &e = table[s][w];
            if (e.valid) {
                uint16_t old_asid = e.asid;
                uint64_t old_vpn = e.vpn;
                e.valid = false;
                emit tlb_update(
                    static_cast<unsigned>(w), static_cast<unsigned>(s), false, old_asid, old_vpn,
                    0ull, false);
            }
        }
    }
    LOG("TLB: SATP changed → flushed all; new SATP=0x%08x", current_satp_raw);
    update_all_statistics();
}

void TLB::flush_single(VirtualAddress va, uint16_t asid) {
    uint64_t vpn = va.get_raw() >> 12;
    size_t s = set_index(vpn);
    for (size_t w = 0; w < associativity_; w++) {
        auto &e = table[s][w];
        if (e.valid && e.vpn == vpn && e.asid == asid) {
            uint16_t old_asid = e.asid;
            uint64_t old_vpn = e.vpn;
            e.valid = false;
            const char *tag = (type == PROGRAM ? "I" : "D");
            LOG("TLB[%s]: flushed VA=0x%llx ASID=%u", tag, (unsigned long long)va.get_raw(), asid);
            emit tlb_update(
                static_cast<unsigned>(w), static_cast<unsigned>(s), false, old_asid, old_vpn, 0ull,
                false);
            update_all_statistics();
        }
    }
}

void TLB::flush() {
    if (num_sets_ == 0 || associativity_ == 0) return;
    const char *tag = (type == PROGRAM ? "I" : "D");
    for (size_t s = 0; s < num_sets_; s++) {
        for (size_t w = 0; w < associativity_; w++) {
            auto &e = table[s][w];
            if (e.valid) {
                uint16_t old_asid = e.asid;
                uint64_t old_vpn = e.vpn;
                e.valid = false;
                emit tlb_update(
                    static_cast<unsigned>(w), static_cast<unsigned>(s), false, old_asid, old_vpn,
                    0ull, false);
            }
        }
    }
    change_counter++;
    LOG("TLB[%s]: flushed all entries", tag);
    update_all_statistics();
}

void TLB::sync() {
    flush();
}

Address TLB::translate_virtual_to_physical(Address vaddr) {
    uint64_t virt = vaddr.get_raw();

<<<<<<< ours
    if (!vm_enabled) { return vaddr; }
=======
    AccessMode mode = vaddr.access_mode();
    CSR::PrivilegeLevel priv = mode.priv();
    uint16_t asid = mode.asid();

    bool satp_mode_on = is_mode_enabled_in_satp(current_satp_raw);
    bool should_translate = vm_enabled && satp_mode_on && (priv != CSR::PrivilegeLevel::MACHINE);

    if (!should_translate) {
        return vaddr;
    }
>>>>>>> theirs

    constexpr unsigned PAGE_SHIFT = 12;
    constexpr uint64_t PAGE_MASK = (1ULL << PAGE_SHIFT) - 1;

    uint64_t off = virt & ((1ULL << PAGE_SHIFT) - 1);
    uint64_t vpn = virt >> PAGE_SHIFT;
    size_t s = set_index(vpn);
    const char *tag = (type == PROGRAM ? "I" : "D");
    uint16_t asid = (current_satp_raw >> 22) & 0x1FF;

    // Check TLB hit
    for (size_t w = 0; w < associativity_; w++) {
        auto &e = table[s][w];
        if (e.valid && e.vpn == vpn && e.asid == asid) {
            repl_policy->notify_access(s, w, /*valid=*/true);
            uint64_t pbase = e.phys.get_raw() & ~((1ULL << PAGE_SHIFT) - 1);
            hit_count_++;
            emit hit_update(hit_count_);
            emit tlb_update(
                static_cast<unsigned>(w), static_cast<unsigned>(s), true, e.asid, e.vpn, pbase,
                false);
            update_all_statistics();
            return Address { pbase + off };
        }
    }

    // TLB miss -> resolve with page table walker
    VirtualAddress va { static_cast<uint32_t>(virt) };
    PageTableWalker walker(mem);

    Address resolved_pa = walker.walk(va, current_satp_raw);
    mem_reads += 1;
    emit memory_reads_update(mem_reads);

    // Cache the resolved mapping in the TLB
    uint64_t phys_base = resolved_pa.get_raw() & ~PAGE_MASK;
    size_t victim = repl_policy->select_way(s);
    auto &ent = table[s][victim];
    ent.valid = true;
    ent.asid = asid;
    ent.vpn = vpn;
    ent.phys = Address { phys_base };
    repl_policy->notify_access(s, victim, /*valid=*/true);
    miss_count_++;
    emit miss_update(miss_count_);
    emit tlb_update(
        static_cast<unsigned>(victim), static_cast<unsigned>(s), true, ent.asid, ent.vpn, phys_base,
        false);

    LOG("TLB[%s]: cached VA=0x%llx -> PA=0x%llx (ASID=%u) on miss", tag, (unsigned long long)virt,
        (unsigned long long)phys_base, asid);
    update_all_statistics();
    return Address { phys_base + off };
}

WriteResult TLB::translate_and_write(Address dst, const void *src, size_t sz, WriteOptions opts) {
    Address pa = translate_virtual_to_physical(dst);
    return mem->write(pa, src, sz, opts);
}

ReadResult TLB::translate_and_read(void *dst, Address src, size_t sz, ReadOptions opts) {
    Address pa = translate_virtual_to_physical(src);
    return mem->read(dst, pa, sz, opts);
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
<<<<<<< ours
bool TLB::is_in_uncached_area(Address source) const {
    return (source >= uncached_start && source <= uncached_last);
}
=======
>>>>>>> theirs

double TLB::get_hit_rate() const {
    unsigned comp = hit_count_ + miss_count_;
    if (comp == 0) return 0.0;
    return (double)hit_count_ / (double)comp * 100.0;
}

uint32_t TLB::get_stall_count() const {
    uint32_t st_cycles = mem_reads * (access_pen_r - 1) + mem_writes * (access_pen_w - 1);
    st_cycles += miss_count_;
    if (access_ena_b) {
        st_cycles -= burst_reads * (access_pen_r - access_pen_b)
                     + burst_writes * (access_pen_w - access_pen_b);
    }
    return st_cycles;
}

const TLBConfig &TLB::get_config() const {
    return tlb_config;
}

double TLB::get_speed_improvement() const {
    unsigned comp = hit_count_ + miss_count_;
    if (comp == 0) return 100.0;
    uint32_t lookup_time = comp;
    uint32_t mem_access_time = mem_reads * access_pen_r + mem_writes * access_pen_w;
    if (access_ena_b) {
        mem_access_time -= burst_reads * (access_pen_r - access_pen_b)
                           + burst_writes * (access_pen_w - access_pen_b);
    }
    double baseline_time = (double)comp * (double)access_pen_r;
    double with_tlb_time = (double)lookup_time + (double)mem_access_time;
    if (with_tlb_time == 0.0) return 100.0;
    return (baseline_time / with_tlb_time) * 100.0;
}

void TLB::update_all_statistics() {
    emit statistics_update(get_stall_count(), get_speed_improvement(), get_hit_rate());
}

void TLB::reset() {
    for (size_t s = 0; s < num_sets_; s++) {
        for (size_t w = 0; w < associativity_; w++) {
            auto &e = table[s][w];
            if (e.valid) {
                uint16_t old_asid = e.asid;
                uint64_t old_vpn = e.vpn;
                e.valid = false;
                emit tlb_update(
                    static_cast<unsigned>(w), static_cast<unsigned>(s), false, old_asid, old_vpn,
                    0ull, false);
            }
        }
    }

    hit_count_ = 0;
    miss_count_ = 0;
    mem_reads = 0;
    mem_writes = 0;
    burst_reads = 0;
    burst_writes = 0;
    change_counter = 0;

    emit hit_update(get_hit_count());
    emit miss_update(get_miss_count());
    emit memory_reads_update(get_read_count());
    emit memory_writes_update(get_write_count());
    update_all_statistics();
}

} // namespace machine
