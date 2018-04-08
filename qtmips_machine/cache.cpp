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
    // Allocate replacement policy data
    switch (cnf.replacement_policy()) {
    case MachineConfigCache::RP_LFU:
        replc.lfu = new unsigned *[cnf.sets()];
        for (unsigned row = 0; row < cnf.sets(); row++)
            replc.lfu[row] = new unsigned[cnf.associativity()];
        break;
    case MachineConfigCache::RP_LRU:
        replc.lru = new time_t*[cnf.sets()];
        for (unsigned row = 0; row < cnf.sets(); row++)
            replc.lru[row] = new time_t[cnf.associativity()];
    default:
        break;
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
    for (unsigned as = 0; as < cnf.associativity(); as++)
        for (unsigned st = 0; st < cnf.sets(); st++)
            if (dt[as][st].valid)
                kick(as, st);
}

unsigned Cache::hit() {
    return hitc;
}

unsigned Cache::miss() {
    return missc;
}

void Cache::reset() {
    // Set all cells to ne invalid
    for (unsigned as = 0; as < cnf.associativity(); as++)
        for (unsigned st = 0; st < cnf.sets(); st++)
            dt[as][st].valid = false;
    // Note: we don't have to zero replacement policy data as those are zeroed when first used on invalid cell
    // Zero hit and miss rate
    hitc = 0;
    missc = 0;
}

const MachineConfigCache &Cache::config() {
    return cnf;
}

void Cache::access(std::uint32_t address, std::uint32_t **data, bool read) const {
    address = address >> 2;
    unsigned ssize = cnf.blocks() * cnf.sets();
    std::uint32_t tag = address / ssize;
    std::uint32_t index = address % ssize;
    std::uint32_t row = index / cnf.blocks();
    std::uint32_t col = index % cnf.blocks();

    unsigned indx = 0;
    // Try to locate exact block or some unused one
    while (indx < cnf.associativity() && dt[indx][row].valid && dt[indx][row].tag != tag)
        indx++;
    // Replace block
    if (indx >= cnf.associativity()) {
        // We have to kick something
        switch (cnf.replacement_policy()) {
        case MachineConfigCache::RP_RAND:
            indx = rand() % cnf.associativity();
            break;
        case MachineConfigCache::RP_LFU:
            {
            unsigned lowest = replc.lfu[row][0];
            indx = 0;
            for (unsigned i = 1; i < cnf.associativity(); i++)
                if (lowest > replc.lfu[row][i]) {
                    lowest = replc.lfu[row][i];
                    indx = i;
                }
            }
            break;
        case MachineConfigCache::RP_LRU:
            {
            time_t lowest = replc.lru[row][0];
            indx = 0;
            for (unsigned i = 1; i < cnf.associativity(); i++)
                if (lowest > replc.lru[row][i]) {
                    lowest = replc.lru[row][i];
                    indx = i;
                }
            }
            break;
        }
    }
    SANITY_ASSERT(indx < cnf.associativity(), "Probably unimplemented replacement policy");

    struct cache_data &cd = dt[indx][row];

    // Verify if we are not replacing
    if (cd.tag != tag && cd.valid)
        kick(indx, row);

    // Update statistics and otherwise read from memory
    if (cd.valid) {
        hitc++;
    } else {
        missc++;
        for (unsigned i = 0; i < cnf.blocks(); i++)
            cd.data[i] = mem->rword(base_address(tag, row) + (4*i));
    }

    // Update replc
    switch (cnf.replacement_policy()) {
    case MachineConfigCache::RP_LFU:
        replc.lru[row][indx]++;
        break;
    case MachineConfigCache::RP_LRU:
        replc.lfu[row][indx] = time(NULL);
        break;
    default:
        break;
    }

    cd.valid = true; // We either write to it or we read from memory. Either way it's valid when we leave Cache class
    cd.dirty = cd.dirty || !read;
    cd.tag = tag;
    *data = &cd.data[col];
}

void Cache::kick(unsigned associat_indx, unsigned row) const {
    struct cache_data &cd = dt[associat_indx][row];
    if (cd.dirty && cnf.write_policy() == MachineConfigCache::WP_BACK)
        for (unsigned i = 0; i < cnf.blocks(); i++)
        mem->wword(base_address(associat_indx, row) + (4*i), cd.data[i]);
    cd.valid = false;
    cd.dirty = false;

    switch (cnf.replacement_policy()) {
    case MachineConfigCache::RP_LFU:
        replc.lru[row][associat_indx] = 0;
        break;
    default:
        break;
    }
}

std::uint32_t Cache::base_address(std::uint32_t tag, unsigned row) const {
    return ((tag * cnf.blocks() * cnf.sets()) + (row * cnf.blocks())) << 2;
}
