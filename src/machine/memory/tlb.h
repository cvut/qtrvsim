#ifndef TLB_H
#define TLB_H

#include "common/logging.h"
#include "frontend_memory.h"
#include "page_table_walker.h"
#include "virtual_address.h"

namespace machine {

enum TLBType { PROGRAM, DATA };

class Machine;

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
    ReadResult  read(void *dst, Address src, size_t sz, ReadOptions opts) const override {
        return const_cast<TLB*>(this)->translate_and_read(dst, src, sz, opts);
    }

    WriteResult write_vaddr(VirtualAddress dst, const void *src, size_t sz, WriteOptions opts) override;
    ReadResult read_vaddr(void *dst, VirtualAddress src, size_t sz, ReadOptions opts) const override;

    uint32_t get_change_counter() const override;

    signals:
      void firstWrite(VirtualAddress va);

private:
    struct Entry { Address phys; uint16_t asid; };
    std::unordered_map<uint64_t, Entry> cache;

    FrontendMemory *mem;
    PageTableWalker ptw;
    TLBType type;
    uint32_t current_satp_raw = 0;
    Machine *machine;
    bool first_instr_written = true;

    WriteResult translate_and_write(Address dst, const void *src, size_t sz, WriteOptions opts);
    ReadResult translate_and_read(void *dst, Address src, size_t sz, ReadOptions opts);

};

}

#endif //TLB_H