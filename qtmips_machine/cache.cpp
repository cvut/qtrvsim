#include "cache.h"

using namespace machine;

Cache::Cache(Memory *m, MachineConfigCache *cc) : cnf(cc) {
    mem = m;
    // Allocate cache data structure
    dt = new struct cache_data*[cc->associativity()];
    for (unsigned i = 0; i < cc->associativity(); i++) {
        dt[i] = new cache_data[cc->sets()];
        for (unsigned y = 0; y < cc->sets(); y++) {
            dt[i][y].valid = false;
            dt[i][y].data = new std::uint32_t[cc->blocks()];
        }
    }
    // Zero hit and miss rate
    hitc = 0;
    missc = 0;
}

void Cache::wword(std::uint32_t address, std::uint32_t value) {
    std::uint32_t *data;
    access(address, &data, false);
    *data = value;

    if (cnf.write_policy() == MachineConfigCache::WP_TROUGH)
        mem->wword(address, value);
}

std::uint32_t Cache::rword(std::uint32_t address) const {
    std::uint32_t *data;
    access(address, &data, true);
    return *data;
}

void Cache::flush() {
    for (unsigned as = 0; as < cnf.associativity(); as++) {
        for (unsigned st = 0; st < cnf.sets(); st++) {
            struct cache_data &cd = dt[as][st];
            if (cnf.write_policy() == MachineConfigCache::WP_BACK && cd.valid && cd.dirty) {
                std::uint32_t base_address = cd.tag * cnf.blocks() * cnf.sets();
                for (unsigned i = 0; i < cnf.blocks(); i++)
                    mem->wword(base_address + (4*i), cd.data[i]);
            }
            cd.dirty = false;
            cd.valid = false;
        }
    }
}

unsigned Cache::hit() {
    return hitc;
}

unsigned Cache::miss() {
    return missc;
}

void Cache::access(std::uint32_t address, std::uint32_t **data, bool read) const {
    // TODO associativity
    address = address >> 2;
    unsigned ssize = cnf.blocks() * cnf.sets();
    std::uint32_t tag = address / ssize;
    std::uint32_t base_address = ((address / cnf.blocks()) * cnf.blocks()) << 2;
    std::uint32_t index = address % ssize;
    std::uint32_t row = index / cnf.blocks();
    std::uint32_t col = index % cnf.blocks();

    struct cache_data &cd = dt[0][row];

    // Verify if we are not replacing
    if (cd.tag != tag && cd.valid) {
        if (cd.dirty && cnf.write_policy() == MachineConfigCache::WP_BACK)
            for (unsigned i = 0; i < cnf.blocks(); i++)
                mem->wword(base_address + (4*i), cd.data[i]);
        cd.valid = false;
        cd.dirty = false;
    }

    if (cd.valid) {
        hitc++;
    } else {
        missc++;
        if (read) { // If this is read and we don't have valid content then read it from memory
            for (unsigned i = 0; i < cnf.blocks(); i++)
                cd.data[i] = mem->rword(base_address + (4*i));
        }
    }

    cd.valid = true; // We either write to it or we read from memory. Either way it's valid when we leave Cache class
    cd.dirty = cd.dirty || !read;
    cd.tag = tag;
    *data = &cd.data[col];
}
