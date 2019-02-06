// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

#include "cache.h"

using namespace machine;

Cache::Cache(MemoryAccess  *m, const MachineConfigCache *cc, unsigned memory_access_penalty_r, unsigned memory_access_penalty_w) : cnf(cc) {
    mem = m;
    access_pen_r = memory_access_penalty_r;
    access_pen_w = memory_access_penalty_w;
    uncached_start = 0xf0000000;
    uncached_last = 0xffffffff;
    // Zero hit and miss rate
    hit_read = 0;
    hit_write = 0;
    miss_read = 0;
    miss_write = 0;
    // Skip any other initialization if cache is disabled
    if (!cc->enabled())
        return;

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
}

bool Cache::wword(std::uint32_t address, std::uint32_t value) {
    bool changed;

    if (!cnf.enabled() ||
        (address >= uncached_start && address <= uncached_last)) {
        return mem->write_word(address, value);
    }

    std::uint32_t data;
    changed = access(address, &data, true, value);

    if (cnf.write_policy() == MachineConfigCache::WP_TROUGH)
        return mem->write_word(address, value);
    return changed;
}

std::uint32_t Cache::rword(std::uint32_t address) const {
    if (!cnf.enabled() ||
        (address >= uncached_start && address <= uncached_last)) {
        return mem->read_word(address);
    }

    std::uint32_t data;
    access(address, &data, false);
    return data;
}

void Cache::flush() {
    if (!cnf.enabled())
        return;

    for (unsigned as = 0; as < cnf.associativity(); as++)
        for (unsigned st = 0; st < cnf.sets(); st++)
            if (dt[as][st].valid)
                kick(as, st);
}

void Cache::sync() {
    flush();
}

unsigned Cache::hit() const {
    return hit_read + hit_write;
}

unsigned Cache::miss() const {
    return miss_read + miss_write;
}

unsigned Cache::stalled_cycles() const {
    return miss_read * (access_pen_r - 1) + miss_write * (access_pen_w - 1);
}

double Cache::speed_improvement() const {
    unsigned comp = hit_read + hit_write + miss_read + miss_write;
    if (comp == 0)
        return 100.0;
    return (double)((miss_read + hit_read) * access_pen_r + (miss_write + hit_write) * access_pen_w) \
            / (double)(hit_write + hit_read + miss_read * access_pen_r + miss_write * access_pen_w) \
            * 100;
}

double Cache::usage_efficiency() const {
    unsigned comp = hit_read + hit_write + miss_read + miss_write;
    if (comp == 0)
        return 0.0;
    return (double)(hit_read + hit_write) / (double)comp * 100.0;
}

void Cache::reset() {
    if (!cnf.enabled())
        return;

    // Set all cells to ne invalid
    for (unsigned as = 0; as < cnf.associativity(); as++)
        for (unsigned st = 0; st < cnf.sets(); st++)
            dt[as][st].valid = false;
    // Note: we don't have to zero replacement policy data as those are zeroed when first used on invalid cell
    // Zero hit and miss rate
    hit_read = 0;
    hit_write = 0;
    miss_read = 0;
    miss_write = 0;
    // Trigger signals
    emit hit_update(hit());
    emit miss_update(miss());
    update_statistics();
    for (unsigned as = 0; as < cnf.associativity(); as++)
        for (unsigned st = 0; st < cnf.sets(); st++)
            emit cache_update(as, st, false, false, 0, 0);
}

const MachineConfigCache &Cache::config() const {
    return cnf;
}

bool Cache::access(std::uint32_t address, std::uint32_t *data, bool write, std::uint32_t value) const {
    bool changed = false;
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
        if (write)
            hit_write++;
        else
            hit_read++;
        emit hit_update(hit());
        update_statistics();
    } else {
        if (write)
            miss_write++;
        else
            miss_read++;
        emit miss_update(miss());
        update_statistics();
        for (unsigned i = 0; i < cnf.blocks(); i++)
            cd.data[i] = mem->read_word(base_address(tag, row) + (4*i));
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
    cd.dirty = cd.dirty || write;
    cd.tag = tag;
    *data = cd.data[col];

    if (write) {
        changed = cd.data[col] != value;
        cd.data[col] = value;
    }

    emit cache_update(indx, row, cd.valid, cd.dirty, cd.tag, cd.data);
    return changed;
}

void Cache::kick(unsigned associat_indx, unsigned row) const {
    struct cache_data &cd = dt[associat_indx][row];
    if (cd.dirty && cnf.write_policy() == MachineConfigCache::WP_BACK)
        for (unsigned i = 0; i < cnf.blocks(); i++)
            mem->write_word(base_address(cd.tag, row) + (4*i), cd.data[i]);
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

void Cache::update_statistics() const {
    emit statistics_update(stalled_cycles(), speed_improvement(), usage_efficiency());
}
