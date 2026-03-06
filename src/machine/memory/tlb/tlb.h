#ifndef TLB_H
#define TLB_H

#include "common/logging.h"
#include "csr/address.h"
#include "memory/frontend_memory.h"
#include "memory/virtual/sv32.h"
#include "memory/virtual/virtual_address.h"
#include "tlb_policy.h"

#include <cstdint>
#include <memory/virtual/sv39.h>
#include <mutex>

namespace machine {

enum TLBType { PROGRAM, DATA };
enum AccessStatus { ACCESS_OK, ACCESS_NOT_OK };
enum UpdateStatus { UPDATE_REQUIRED, UPDATE_NOT_REQUIRED };

class Machine;

// Implements a set-associative Translation Lookaside Buffer (TLB) frontend over physical memory,
// handling virtual to physical translation, flush, and replacement policy.
class TLB : public FrontendMemory {
    Q_OBJECT
public:
    struct Entry {
        bool valid = false;
        uint16_t asid = 0;
        uint64_t vpn = 0;
        Address phys = Address { 0 };
        Address pte_addr = Address { 0 };
        uint8_t pte_bytes = 0;
        uint32_t lru = 0;
        bool R;
        bool W;
        bool X;
        bool U;
        bool G;
        bool A;
        bool D;

        [[nodiscard]] bool r() const { return R; }

        [[nodiscard]] bool w() const { return W; }

        [[nodiscard]] bool x() const { return X; }

        [[nodiscard]] bool u() const { return U; }

        [[nodiscard]] bool g() const { return G; }

        [[nodiscard]] bool a() const { return A; }

        [[nodiscard]] bool d() const { return D; }
    };

    struct TranslationResult {
        Address phys;
        size_t bytes_until_page_end;
        Entry *entry = nullptr;
    };

    TLB(FrontendMemory *memory,
        FrontendMemory *pt_walk_mem,
        TLBType type,
        const TLBConfig *config,
        Xlen xlen,
        bool vm_enabled = true,
        uint32_t memory_access_penalty_r = 1,
        uint32_t memory_access_penalty_w = 1,
        uint32_t memory_access_penalty_b = 0,
        bool memory_access_enable_b = false);

    void on_csr_write(size_t internal_id, RegisterValue val);
    void flush_single(VirtualAddress va, uint16_t asid);

    void flush_all_entries();

    void flush_by_asid(uint16_t asid);

    void flush_by_vpn(uint64_t vpn);

    void sync() override;

    void sfence_vma(uint64_t vaddr, uint64_t asid) override;

    TranslationResult translate_virtual_to_physical(AddressWithMode vaddr);

    WriteResult write(AddressWithMode dst, const void *src, size_t sz, WriteOptions opts) override;

    ReadResult read(void *dst, AddressWithMode src, size_t sz, ReadOptions opts) const override;

    uint32_t get_change_counter() const override {
        uint32_t base = mem->get_change_counter();
        if (pt_walk_mem != mem) base += pt_walk_mem->get_change_counter();
        return base;
    }

    void set_replacement_policy(std::unique_ptr<TLBPolicy> p) { repl_policy = std::move(p); }

    uint64_t root_page_table_ppn() const {
        switch (xlen) {
        case Xlen::_32: return current_satp_raw & ((uint64_t(1) << Sv32Pte::PPN_BITS) - 1ULL);
        case Xlen::_64: return current_satp_raw & ((uint64_t(1) << Sv39Pte::PPN_BITS) - 1ULL);
        default: return current_satp_raw & ((uint64_t(1) << Sv32Pte::PPN_BITS) - 1ULL);
        }
    }

    bool reverse_lookup(Address paddr, VirtualAddress &out_va) const;

    unsigned get_hit_count() const { return hit_count_; }
    unsigned get_miss_count() const { return miss_count_; }
    double get_hit_rate() const;
    uint32_t get_read_count() const { return mem_reads + (pt_walk_mem == mem ? 0 : ptw_reads); }
    uint32_t get_write_count() const { return mem_writes + (pt_walk_mem == mem ? 0 : ptw_writes); }
    uint32_t get_burst_read_count() const { return burst_reads; }
    uint32_t get_burst_write_count() const { return burst_writes; }

    uint32_t get_stall_count() const;
    double get_speed_improvement() const;
    const TLBConfig &get_config() const;

    void reset();
    void update_all_statistics();

signals:
    void tlb_update(
        unsigned way,
        unsigned set,
        bool valid,
        uint16_t asid,
        uint64_t vpn,
        uint64_t phys_base,
        bool R,
        bool W,
        bool X,
        bool U,
        bool G,
        bool A,
        bool D);
    void hit_update(unsigned val);
    void miss_update(unsigned val);
    void statistics_update(unsigned stalled_cycles, double speed_improv, double hit_rate);
    void memory_reads_update(uint32_t val);
    void memory_writes_update(uint32_t val);

private:
    FrontendMemory *mem;
    FrontendMemory *pt_walk_mem;
    TLBType type;
    const TLBConfig tlb_config;
    uint64_t current_satp_raw = 0;
    uint64_t current_sstatus_raw = 0;
    Xlen xlen;
    const bool vm_enabled;
    mutable std::mutex pte_lock_;

    size_t num_sets_;
    size_t associativity_;
    std::vector<std::vector<Entry>> table;
    std::unique_ptr<TLBPolicy> repl_policy;

    const uint32_t access_pen_r;
    const uint32_t access_pen_w;
    const uint32_t access_pen_b;
    const bool access_ena_b;

    mutable unsigned hit_count_ = 0;
    mutable unsigned miss_count_ = 0;
    mutable uint32_t mem_reads = 0;
    mutable uint32_t mem_writes = 0;
    mutable uint32_t ptw_reads = 0;
    mutable uint32_t ptw_writes = 0;
    mutable uint32_t burst_reads = 0;
    mutable uint32_t burst_writes = 0;
    mutable uint32_t change_counter = 0;

    WriteResult translate_and_write(AddressWithMode dst, const void *src, size_t sz, WriteOptions opts);
    ReadResult translate_and_read(void *dst, AddressWithMode src, size_t sz, ReadOptions opts);
    UpdateStatus ensure_ad_bits(Entry &e, AccessOp op);
    template<typename RawPte>
    UpdateStatus ensure_ad_bits_impl(Entry &e, AccessOp op);
    inline size_t set_index(uint64_t vpn) const { return vpn & (num_sets_ - 1); }
    inline bool is_mode_enabled_in_satp(uint64_t satp_raw) const {
        switch (xlen) {
        case Xlen::_32: return (satp_raw & (1u << 31)) != 0;
        case Xlen::_64: return ((satp_raw >> 60) & 0xFULL) != 0;
        default: return (satp_raw & (1u << 31)) != 0;
        }
    }
};

} // namespace machine

#endif // TLB_H
