#ifndef TLB_H
#define TLB_H

#include "common/logging.h"
#include "memory/frontend_memory.h"
#include "memory/virtual/page_fault_handler.h"
#include "memory/virtual/page_table_walker.h"
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
    TLB(FrontendMemory *memory, TLBType type, Machine *mach);

    void on_csr_write(size_t internal_id, RegisterValue val);
    void flush_single(VirtualAddress va, uint16_t asid);

    Address translate_virtual_to_physical(Address va);

    WriteResult write(Address dst, const void *src, size_t sz, WriteOptions opts) override {
        return translate_and_write(dst, src, sz, opts);
    }
    ReadResult read(void *dst, Address src, size_t sz, ReadOptions opts) const override {
        return const_cast<TLB*>(this)->translate_and_read(dst, src, sz, opts);
    }

    WriteResult write_vaddr(VirtualAddress dst, const void *src, size_t sz, WriteOptions opts) override;
    ReadResult read_vaddr(void *dst, VirtualAddress src, size_t sz, ReadOptions opts) const override;

    uint32_t get_change_counter() const override {
        return mem->get_change_counter();
    }

    void set_replacement_policy(std::unique_ptr<TLBPolicy> p) {
        repl_policy = std::move(p);
    }

    bool reverse_lookup(Address paddr, VirtualAddress &out_va) const;

    void set_page_fault_handler(PageFaultHandler *h) { pf_handler = h; }

signals:
    void firstWrite(VirtualAddress va);

private:
    struct Entry {
        bool valid = false;
        uint16_t asid = 0;
        uint64_t vpn = 0;
        Address phys = Address{0};
        uint32_t lru = 0;
    };

    FrontendMemory *mem;
    PageTableWalker ptw;
    TLBType type;
    Machine *machine;
    PageFaultHandler *pf_handler = nullptr;
    uint32_t current_satp_raw = 0;
    bool first_instr_written = true;

    size_t num_sets_;
    size_t associativity_;
    std::vector<std::vector<Entry>> table;
    std::unique_ptr<TLBPolicy> repl_policy;

    WriteResult translate_and_write(Address dst, const void *src, size_t sz, WriteOptions opts);
    ReadResult translate_and_read(void *dst, Address src, size_t sz, ReadOptions opts);

    inline size_t set_index(uint64_t vpn) const {
        return vpn & (num_sets_ - 1);
    }
};

}

#endif //TLB_H