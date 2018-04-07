#ifndef CACHE_H
#define CACHE_H

#include <memory.h>
#include <machineconfig.h>
#include <stdint.h>

namespace machine {

class Cache : public MemoryAccess {
    Q_OBJECT
public:
    Cache(Memory *m, MachineConfigCache *c);

    void wword(std::uint32_t address, std::uint32_t value);
    std::uint32_t rword(std::uint32_t address) const;

    void flush(); // flush/sync cache

    unsigned hit(); // Number of recorded hits
    unsigned miss(); // Number of recorded misses

    // TODO reset

private:
    MachineConfigCache cnf;
    Memory *mem;

    struct cache_data {
        bool valid, dirty;
        std::uint32_t tag;
        std::uint32_t *data;
    };
    mutable struct cache_data **dt;

    mutable unsigned hitc, missc;

    void access(std::uint32_t address, std::uint32_t **data, bool read) const;
};

}

#endif // CACHE_H
