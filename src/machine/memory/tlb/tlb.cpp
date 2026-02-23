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
    Xlen xlen,
    bool vm_enabled,
    uint32_t memory_access_penalty_r,
    uint32_t memory_access_penalty_w,
    uint32_t memory_access_penalty_b,
    bool memory_access_enable_b)
    : FrontendMemory(memory->simulated_machine_endian)
    , mem(memory)
    , type(type_)
    , tlb_config(config)
    , xlen(xlen)
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

    uint64_t new_satp = val.as_u64();
    bool old_mode = is_mode_enabled_in_satp(current_satp_raw);
    bool new_mode = is_mode_enabled_in_satp(new_satp);
    current_satp_raw = new_satp;
    if (old_mode != new_mode) {
        flush_all_entries();
        DEBUG(
            "TLB: SATP changed -> translation mode changed (%u -> %u), flushed all",
            static_cast<unsigned>(old_mode), static_cast<unsigned>(new_mode));
    } else {
        update_all_statistics();
    }
}

void TLB::flush_single(VirtualAddress va, uint16_t asid) {
    uint64_t vpn = va.get_raw() >> 12;
    size_t s = set_index(vpn);
    bool any_invalidated = false;
    const char *tag = type == PROGRAM ? "I" : "D";

    for (size_t w = 0; w < associativity_; w++) {
        auto &e = table[s][w];
        if (e.valid && e.vpn == vpn && (e.asid == asid || asid == 0)) {
            uint16_t old_asid = e.asid;
            uint64_t old_vpn = e.vpn;
            e.valid = false;
            DEBUG(
                "TLB[%s]: flushed VA=0x%llx ASID=%u (wildcard=%s)", tag,
                (unsigned long long)va.get_raw(), asid, (asid == 0 ? "true" : "false"));
            emit tlb_update(
                static_cast<unsigned>(w), static_cast<unsigned>(s), false, old_asid, old_vpn, 0ull,
                false);
            any_invalidated = true;
        }
    }

    if (any_invalidated) {
        ++change_counter;
        update_all_statistics();
    }
}

void TLB::flush_all_entries() {
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
    DEBUG("TLB[%s]: flushed all entries", tag);
    update_all_statistics();
}

void TLB::flush_by_asid(uint16_t asid) {
    bool any_invalidated = false;
    for (size_t s = 0; s < num_sets_; ++s) {
        for (size_t w = 0; w < associativity_; ++w) {
            auto &e = table[s][w];
            if (e.valid && e.asid == asid) {
                uint16_t old_asid = e.asid;
                uint64_t old_vpn = e.vpn;
                e.valid = false;
                emit tlb_update(
                    static_cast<unsigned>(w), static_cast<unsigned>(s), false, old_asid, old_vpn,
                    0ull, false);
                any_invalidated = true;
            }
        }
    }
    if (any_invalidated) {
        ++change_counter;
        update_all_statistics();
    }
}

void TLB::flush_by_vpn(uint64_t vpn) {
    bool any_invalidated = false;
    for (size_t s = 0; s < num_sets_; ++s) {
        for (size_t w = 0; w < associativity_; ++w) {
            auto &e = table[s][w];
            if (e.valid && e.vpn == vpn) {
                uint16_t old_asid = e.asid;
                uint64_t old_vpn = e.vpn;
                e.valid = false;
                emit tlb_update(
                    static_cast<unsigned>(w), static_cast<unsigned>(s), false, old_asid, old_vpn,
                    0ull, false);
                any_invalidated = true;
            }
        }
    }
    if (any_invalidated) {
        ++change_counter;
        update_all_statistics();
    }
}

void TLB::sync() {
    mem->sync();
    update_all_statistics();
}

void TLB::sfence_vma(uint64_t vaddr, uint64_t asid) {
    if (vaddr == 0 && asid == 0) {
        flush_all_entries();
        return;
    }

    if (vaddr != 0 && asid != 0) {
        VirtualAddress va { vaddr };
        flush_single(va, static_cast<uint16_t>(asid & 0xFFFFu));
        return;
    }

    if (vaddr == 0 && asid != 0) {
        flush_by_asid(static_cast<uint16_t>(asid & 0xFFFFu));
        return;
    }

    uint64_t vpn = vaddr >> 12;
    flush_by_vpn(vpn);
}

TLB::TranslationResult TLB::translate_virtual_to_physical(AddressWithMode vaddr) {
    uint64_t virt = vaddr.get_raw();

    AccessMode mode = vaddr.access_mode();
    CSR::PrivilegeLevel priv = mode.priv();
    uint16_t asid = mode.asid();

    bool satp_mode_on = is_mode_enabled_in_satp(current_satp_raw);
    bool should_translate = vm_enabled && satp_mode_on && (priv != CSR::PrivilegeLevel::MACHINE);

    if (!should_translate) { return { vaddr, std::numeric_limits<size_t>::max() }; }

    constexpr unsigned PAGE_SHIFT = 12;
    constexpr uint64_t PAGE_MASK = (1ULL << PAGE_SHIFT) - 1;
    constexpr size_t PAGE_BYTES = (1ULL << PAGE_SHIFT);

    uint64_t off = virt & PAGE_MASK;
    uint64_t vpn = virt >> PAGE_SHIFT;
    size_t s = set_index(vpn);
    const char *tag = (type == PROGRAM ? "I" : "D");

    // Check TLB hit
    for (size_t w = 0; w < associativity_; w++) {
        auto &e = table[s][w];
        if (e.valid && e.vpn == vpn && e.asid == asid) {
            repl_policy->notify_access(s, w, /*valid=*/true);
            uint64_t pbase = e.phys.get_raw() & ~PAGE_MASK;
            hit_count_++;
            emit hit_update(hit_count_);
            emit tlb_update(
                static_cast<unsigned>(w), static_cast<unsigned>(s), true, e.asid, e.vpn, pbase,
                false);
            update_all_statistics();
            return { Address { pbase + off }, static_cast<size_t>(PAGE_BYTES - off) };
        }
    }

    // TLB miss -> resolve with page table walker
    VirtualAddress va { virt };

    PageTableWalker walker(mem);

    Address resolved_pa;
    switch (xlen) {
    case Xlen::_32:
        walker.set_pte_factory(PageTableWalker::sv32_pte_factory());
        resolved_pa = walker.walk<Sv32Pte, 1>(va, current_satp_raw);
        break;
    case Xlen::_64:
        walker.set_pte_factory(PageTableWalker::sv39_pte_factory());
        resolved_pa = walker.walk<Sv39Pte, 2>(va, current_satp_raw);
        break;
    default:
        walker.set_pte_factory(PageTableWalker::sv32_pte_factory());
        resolved_pa = walker.walk<Sv32Pte, 1>(va, current_satp_raw);
        break;
    }

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

    DEBUG(
        "TLB[%s]: cached VA=0x%llx -> PA=0x%llx (ASID=%u) on miss", tag, (unsigned long long)virt,
        (unsigned long long)phys_base, asid);
    update_all_statistics();
    return { Address { phys_base + off }, static_cast<size_t>(PAGE_BYTES - off) };
}

WriteResult TLB::write(AddressWithMode dst, const void *src, size_t sz, WriteOptions opts) {
    return translate_and_write(dst, src, sz, opts);
}

ReadResult TLB::read(void *dst, AddressWithMode src, size_t sz, ReadOptions opts) const {
    return const_cast<TLB *>(this)->translate_and_read(dst, src, sz, opts);
}

WriteResult TLB::translate_and_write(AddressWithMode dst, const void *src, size_t sz, WriteOptions opts) {
    const uint8_t *cur_src = static_cast<const uint8_t *>(src);
    uint64_t cur_virt = dst.get_raw();
    size_t remaining = sz;

    size_t total_written = 0;
    bool any_changed = false;

    while (remaining > 0) {
        AddressWithMode cur_va(Address { cur_virt }, dst.access_mode());
        auto tr = translate_virtual_to_physical(cur_va);

        size_t bytes_to_write = std::min(remaining, tr.bytes_until_page_end);

        WriteResult wr = mem->write(tr.phys, cur_src, bytes_to_write, opts);
        total_written += wr.n_bytes;
        any_changed |= wr.changed;

        cur_src += bytes_to_write;
        cur_virt += bytes_to_write;
        remaining -= bytes_to_write;
    }

    return { .n_bytes = total_written, .changed = any_changed };
}

ReadResult TLB::translate_and_read(void *dst, AddressWithMode src, size_t sz, ReadOptions opts) {
    uint8_t *cur_dst = static_cast<uint8_t *>(dst);
    uint64_t cur_virt = src.get_raw();
    size_t remaining = sz;

    size_t total_read = 0;

    while (remaining > 0) {
        AddressWithMode cur_va(Address { cur_virt }, src.access_mode());
        auto tr = translate_virtual_to_physical(cur_va);

        size_t bytes_to_read = std::min(remaining, tr.bytes_until_page_end);

        ReadResult rr = mem->read(cur_dst, tr.phys, bytes_to_read, opts);
        total_read += rr.n_bytes;

        cur_dst += bytes_to_read;
        cur_virt += bytes_to_read;
        remaining -= bytes_to_read;
    }

    return { .n_bytes = total_read };
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
