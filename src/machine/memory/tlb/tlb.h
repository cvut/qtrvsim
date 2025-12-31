#ifndef TLB_H
#define TLB_H

#include "common/logging.h"
#include "csr/address.h"
#include "memory/frontend_memory.h"
#include "memory/virtual/sv32.h"
#include "memory/virtual/virtual_address.h"
#include "tlb_policy.h"

#include <cstdint>

namespace machine {

enum TLBType { PROGRAM, DATA };

class Machine;

// Implements a set-associative Translation Lookaside Buffer (TLB) frontend over physical memory,
// handling virtual to physical translation, flush, and replacement policy.
class TLB : public FrontendMemory {
    Q_OBJECT
public:
    TLB(FrontendMemory *memory,
        TLBType type,
        const TLBConfig *config,
        bool vm_enabled = true,
        uint32_t memory_access_penalty_r = 1,
        uint32_t memory_access_penalty_w = 1,
        uint32_t memory_access_penalty_b = 0,
        bool memory_access_enable_b = false);

    void on_csr_write(size_t internal_id, RegisterValue val);
    void flush_single(VirtualAddress va, uint16_t asid);

    void flush();

    void sync() override;

    Address translate_virtual_to_physical(AddressWithMode vaddr);

    WriteResult write(AddressWithMode dst, const void *src, size_t sz, WriteOptions opts) override;

    ReadResult read(void *dst, AddressWithMode src, size_t sz, ReadOptions opts) const override;

    uint32_t get_change_counter() const override { return mem->get_change_counter(); }

    void set_replacement_policy(std::unique_ptr<TLBPolicy> p) { repl_policy = std::move(p); }

    uint32_t root_page_table_ppn() const { return current_satp_raw & ((1u << PPN_BITS) - 1); }

    bool reverse_lookup(Address paddr, VirtualAddress &out_va) const;

    unsigned get_hit_count() const { return hit_count_; }
    unsigned get_miss_count() const { return miss_count_; }
    double get_hit_rate() const;
    uint32_t get_read_count() const { return mem_reads; }
    uint32_t get_write_count() const { return mem_writes; }
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
        bool is_write);
    void hit_update(unsigned val);
    void miss_update(unsigned val);
    void statistics_update(unsigned stalled_cycles, double speed_improv, double hit_rate);
    void memory_reads_update(uint32_t val);
    void memory_writes_update(uint32_t val);

private:
    struct Entry {
        bool valid = false;
        uint16_t asid = 0;
        uint64_t vpn = 0;
        Address phys = Address { 0 };
        uint32_t lru = 0;
    };

    FrontendMemory *mem;
    TLBType type;
    const TLBConfig tlb_config;
    uint32_t current_satp_raw = 0;
    const bool vm_enabled;

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
    mutable uint32_t burst_reads = 0;
    mutable uint32_t burst_writes = 0;
    mutable uint32_t change_counter = 0;

    WriteResult
    translate_and_write(AddressWithMode dst, const void *src, size_t sz, WriteOptions opts);
    ReadResult translate_and_read(void *dst, AddressWithMode src, size_t sz, ReadOptions opts);
    inline size_t set_index(uint64_t vpn) const { return vpn & (num_sets_ - 1); }
    inline bool is_mode_enabled_in_satp(uint32_t satp_raw) { return (satp_raw & (1u << 31)) != 0; }
};

} // namespace machine

#endif // TLB_H
