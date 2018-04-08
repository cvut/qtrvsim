#ifndef CACHE_H
#define CACHE_H

#include <memory.h>
#include <machineconfig.h>
#include <stdint.h>
#include <time.h>

namespace machine {

class Cache : public MemoryAccess {
    Q_OBJECT
public:
    Cache(Memory *m, const MachineConfigCache *c);

    void wword(std::uint32_t address, std::uint32_t value);
    std::uint32_t rword(std::uint32_t address) const;

    void flush(); // flush/sync cache

    unsigned hit() const; // Number of recorded hits
    unsigned miss() const; // Number of recorded misses

    void reset(); // Reset whole state of cache

    const MachineConfigCache &config() const;
    // TODO getters for cells

private:
    MachineConfigCache cnf;
    Memory *mem;

    struct cache_data {
        bool valid, dirty;
        std::uint32_t tag;
        std::uint32_t *data;
    };
    mutable struct cache_data **dt;

    union {
        time_t ** lru; // Access time
        unsigned **lfu; // Access count
    } replc; // Data used for replacement policy

    mutable unsigned hitc, missc; // Hit and miss counters

    void access(std::uint32_t address, std::uint32_t **data, bool read) const;
    void kick(unsigned associat_indx, unsigned row) const;
    std::uint32_t base_address(std::uint32_t tag, unsigned row) const;
};

}

#endif // CACHE_H
