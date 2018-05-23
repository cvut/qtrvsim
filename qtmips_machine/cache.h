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
    Cache(Memory *m, const MachineConfigCache *c, unsigned memory_access_penalty_r = 1, unsigned memory_access_penalty_w = 1);

    void wword(std::uint32_t address, std::uint32_t value);
    std::uint32_t rword(std::uint32_t address) const;

    void flush(); // flush cache
    void sync(); // Same as flush

    unsigned hit() const; // Number of recorded hits
    unsigned miss() const; // Number of recorded misses
    unsigned stalled_cycles() const; // Number of wasted cycles in memory waitin statistic
    double speed_improvement() const; // Speed improvement in percents in comare with no used cache
    double usage_efficiency() const; // Usage efficiency in percents

    void reset(); // Reset whole state of cache

    const MachineConfigCache &config() const;

signals:
    void hit_update(unsigned) const;
    void miss_update(unsigned) const;
    void statistics_update(unsigned stalled_cycles, double speed_improv, double usage_effic) const;
    void cache_update(unsigned associat, unsigned set, bool valid, bool dirty, std::uint32_t tag, const std::uint32_t *data) const;

private:
    MachineConfigCache cnf;
    Memory *mem;
    unsigned access_pen_r, access_pen_w;

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

    mutable unsigned hit_read, miss_read, hit_write, miss_write; // Hit and miss counters

    void access(std::uint32_t address, std::uint32_t *data, bool write, std::uint32_t value = 0) const;
    void kick(unsigned associat_indx, unsigned row) const;
    std::uint32_t base_address(std::uint32_t tag, unsigned row) const;
    void update_statistics() const;
};

}

#endif // CACHE_H
