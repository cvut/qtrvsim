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

Cache::Cache(MemoryAccess  *m, const MachineConfigCache *cc, unsigned memory_access_penalty_r,
             unsigned memory_access_penalty_w, unsigned memory_access_penalty_b) : cnf(cc) {
    mem = m;
    access_pen_r = memory_access_penalty_r;
    access_pen_w = memory_access_penalty_w;
    access_pen_b = memory_access_penalty_b;
    uncached_start = 0xf0000000;
    uncached_last = 0xffffffff;
    // Zero hit and miss rate
    hit_read = 0;
    hit_write = 0;
    miss_read = 0;
    miss_write = 0;
    mem_reads = 0;
    mem_writes = 0;
    burst_reads = 0;
    burst_writes = 0;
    dt = nullptr;
    replc.lfu = nullptr;
    replc.lru = nullptr;
    change_counter = 0;

    // Skip any other initialization if cache is disabled
    if (!cc->enabled())
        return;

    // Allocate cache data structure
    dt = new struct cache_data*[cc->associativity()];
    for (unsigned i = 0; i < cc->associativity(); i++) {
        dt[i] = new cache_data[cc->sets()];
        for (unsigned y = 0; y < cc->sets(); y++) {
            dt[i][y].valid = false;
            dt[i][y].dirty = false;
            dt[i][y].data = new std::uint32_t[cc->blocks()];
        }
    }
    // Allocate replacement policy data
    switch (cnf.replacement_policy()) {
    case MachineConfigCache::RP_LFU:
        replc.lfu = new unsigned *[cnf.sets()];
        for (unsigned int row = 0; row < cnf.sets(); row++) {
            replc.lfu[row] = new unsigned[cnf.associativity()];
            for (unsigned int  i = 0; i < cnf.associativity(); i++)
                 replc.lfu[row][i] = 0;
        }
        break;
    case MachineConfigCache::RP_LRU:
        replc.lru = new unsigned int*[cnf.sets()];
        for (unsigned int row = 0; row < cnf.sets(); row++) {
            replc.lru[row] = new unsigned int[cnf.associativity()];
            for (unsigned int i = 0; i < cnf.associativity(); i++)
                replc.lru[row][i] = i;
        }
        break;
    case MachineConfigCache::RP_RAND:
    default:
        break;
    }
}

Cache::~Cache(){
    if (dt != nullptr) {
        for (unsigned i = 0; i < cnf.associativity(); i++) {
	    if (dt[i]) {
                for (unsigned y = 0; y < cnf.sets(); y++)
                    if (dt[i][y].data != nullptr)
                        delete[] dt[i][y].data;
                delete[] dt[i];
            }
	}
        delete[] dt;
    }
    switch (cnf.replacement_policy()) {
    case MachineConfigCache::RP_LFU:
        if (replc.lfu == nullptr)
            break;
        for (unsigned row = 0; row < cnf.sets(); row++)
            delete[] replc.lfu[row];
        delete [] replc.lfu;
        break;
    case MachineConfigCache::RP_LRU:
        if (replc.lru == nullptr)
            break;
        for (unsigned row = 0; row < cnf.sets(); row++)
            delete[] replc.lru[row];
        delete[] replc.lru;
    default:
        break;
    }
}


bool Cache::wword(std::uint32_t address, std::uint32_t value) {
    bool changed;

    if (!cnf.enabled() ||
        (address >= uncached_start && address <= uncached_last)) {
        mem_writes++;
        emit memory_writes_update(mem_writes);
        update_statistics();
        return mem->write_word(address, value);
    }

    std::uint32_t data;
    changed = access(address, &data, true, value);

    if (cnf.write_policy() != MachineConfigCache::WP_BACK) {
        mem_writes++;
        emit memory_writes_update(mem_writes);
        update_statistics();
        return mem->write_word(address, value);
    }
    return changed;
}

std::uint32_t Cache::rword(std::uint32_t address, bool debug_access) const {
    if (!cnf.enabled() ||
        (address >= uncached_start && address <= uncached_last)) {
        mem_reads++;
        emit memory_reads_update(mem_reads);
        update_statistics();
        return mem->read_word(address, debug_access);
    }

    if (debug_access) {
        if (!(location_status(address) & LOCSTAT_CACHED))
            return mem->read_word(address, debug_access);
        return debug_rword(address);
    }
    std::uint32_t data;
    access(address, &data, false);
    return data;
}

void Cache::flush() {
    if (!cnf.enabled())
        return;

    for (unsigned as = cnf.associativity(); as-- > 0 ; )
        for (unsigned st = 0; st < cnf.sets(); st++)
            if (dt[as][st].valid) {
                kick(as, st);
                emit cache_update(as, st, 0, false, false, 0, 0);
            }
    change_counter++;
    update_statistics();
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

unsigned Cache::memory_reads() const {
    return mem_reads;
}

unsigned Cache::memory_writes() const {
    return mem_writes;
}

unsigned Cache::stalled_cycles() const {
    unsigned st_cycles = mem_reads * (access_pen_r - 1) + mem_writes * (access_pen_w - 1);
    if (access_pen_b != 0)
        st_cycles -= burst_reads * (access_pen_r - access_pen_b) +
                     burst_writes * (access_pen_w - access_pen_b);
    return st_cycles;
}

double Cache::speed_improvement() const {
    unsigned lookup_time;
    unsigned mem_access_time;
    unsigned comp = hit_read + hit_write + miss_read + miss_write;
    if (comp == 0)
        return 100.0;
    lookup_time = hit_read + miss_read;
    if (cnf.write_policy() == MachineConfigCache::WP_BACK)
        lookup_time += hit_write + miss_write;
    mem_access_time = mem_reads * access_pen_r + mem_writes * access_pen_w;
    if (access_pen_b != 0)
        mem_access_time -= burst_reads * (access_pen_r - access_pen_b) +
                           burst_writes * (access_pen_w - access_pen_b);
    return (double)((miss_read + hit_read) * access_pen_r + (miss_write + hit_write) * access_pen_w) \
            / (double)(lookup_time + mem_access_time) \
            * 100;
}

double Cache::hit_rate() const {
    unsigned comp = hit_read + hit_write + miss_read + miss_write;
    if (comp == 0)
        return 0.0;
    return (double)(hit_read + hit_write) / (double)comp * 100.0;
}

void Cache::reset() {
    // Set all cells to invalid
    if (cnf.enabled()) {
        for (unsigned as = 0; as < cnf.associativity(); as++)
            for (unsigned st = 0; st < cnf.sets(); st++)
                dt[as][st].valid = false;
    }
    // Note: we don't have to zero replacement policy data as those are zeroed when first used on invalid cell
    // Zero hit and miss rate
    hit_read = 0;
    hit_write = 0;
    miss_read = 0;
    miss_write = 0;
    mem_reads = 0;
    mem_writes = 0;
    burst_reads = 0;
    burst_writes = 0;
    // Trigger signals
    emit hit_update(hit());
    emit miss_update(miss());
    emit memory_reads_update(memory_reads());
    emit memory_writes_update(memory_writes());
    update_statistics();
    if (cnf.enabled()) {
        for (unsigned as = 0; as < cnf.associativity(); as++)
            for (unsigned st = 0; st < cnf.sets(); st++)
                emit cache_update(as, st, 0, false, false, 0, 0);
    }
}

const MachineConfigCache &Cache::config() const {
    return cnf;
}

enum LocationStatus Cache::location_status(std::uint32_t address) const {
    std::uint32_t row, col, tag;
    compute_row_col_tag(row, col, tag, address);

    if (cnf.enabled()) {
        for (unsigned indx = 0; indx < cnf.associativity(); indx++) {
            if (dt[indx][row].valid && dt[indx][row].tag == tag) {
                if (dt[indx][row].dirty &&
                    cnf.write_policy() == MachineConfigCache::WP_BACK)
                    return (enum LocationStatus)(LOCSTAT_CACHED | LOCSTAT_DIRTY);
                else
                    return (enum LocationStatus)LOCSTAT_CACHED;
            }
        }
    }
    return mem->location_status(address);
}

std::uint32_t Cache::debug_rword(std::uint32_t address) const {
    std::uint32_t row, col, tag;
    compute_row_col_tag(row, col, tag, address);
    for (unsigned indx = 0; indx < cnf.associativity(); indx++)
        if (dt[indx][row].valid && dt[indx][row].tag == tag)
            return dt[indx][row].data[col];
    return 0;
}

bool Cache::access(std::uint32_t address, std::uint32_t *data, bool write, std::uint32_t value) const {
    bool changed = false;
    std::uint32_t row, col, tag;
    compute_row_col_tag(row, col, tag, address);

    unsigned indx = 0;
    // Try to locate exact block
    while (indx < cnf.associativity() && (!dt[indx][row].valid || dt[indx][row].tag != tag))
        indx++;
    // Need to find new block
    if (indx >= cnf.associativity()) {
        // if write through we do not need to alloecate cache line does not allocate
        if (write && cnf.write_policy() == MachineConfigCache::WP_TROUGH_NOALLOC) {
            miss_write++;
            emit miss_update(miss());
            update_statistics();
            return false;
        }
        // We have to kick something
        switch (cnf.replacement_policy()) {
        case MachineConfigCache::RP_RAND:
            indx = rand() % cnf.associativity();
            break;
        case MachineConfigCache::RP_LRU:
            {
                indx = replc.lru[row][0];
                break;
            }
        case MachineConfigCache::RP_LFU:
            {
                unsigned lowest = replc.lfu[row][0];
                indx = 0;
                for (unsigned i = 1; i < cnf.associativity(); i++) {
                    if (!dt[i][row].valid) {
                        indx = i;
                        break;
                    }
                    if (lowest > replc.lfu[row][i]) {
                        lowest = replc.lfu[row][i];
                        indx = i;
                    }
                }
                break;
            }
        }
    }
    SANITY_ASSERT(indx < cnf.associativity(), "Probably unimplemented replacement policy");

    struct cache_data &cd = dt[indx][row];

    // Verify if we are not replacing
    if (cd.valid && cd.tag != tag) {
        kick(indx, row);
        change_counter++;
    }

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
        for (unsigned i = 0; i < cnf.blocks(); i++) {
            cd.data[i] = mem->read_word(base_address(tag, row) + (4*i));
            change_counter++;
        }
        mem_reads += cnf.blocks();
        burst_reads += cnf.blocks() - 1;
        emit memory_reads_update(mem_reads);
        update_statistics();
    }

    // Update replcement data
    switch (cnf.replacement_policy()) {
    case MachineConfigCache::RP_LRU:
    {
        unsigned next_asi = indx;
        int i = cnf.associativity() - 1;
        unsigned tmp_asi = replc.lru[row][i];
        while (tmp_asi != indx) {
            SANITY_ASSERT(i >= 0, "LRU lost the way from priority queue - access");
            tmp_asi = replc.lru[row][i];
            replc.lru[row][i] = next_asi;
            next_asi = tmp_asi;
            i--;
        }
        break;
    }
    case MachineConfigCache::RP_LFU:
        if (cd.valid)
            replc.lfu[row][indx]++;
        else
            replc.lfu[row][indx] = 0;
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

    emit cache_update(indx, row, col, cd.valid, cd.dirty, cd.tag, cd.data);
    if (changed)
        change_counter++;
    return changed;
}

void Cache::kick(unsigned associat_indx, unsigned row) const {
    struct cache_data &cd = dt[associat_indx][row];
    if (cd.dirty && cnf.write_policy() == MachineConfigCache::WP_BACK) {
        for (unsigned i = 0; i < cnf.blocks(); i++)
            mem->write_word(base_address(cd.tag, row) + (4*i), cd.data[i]);
        mem_writes += cnf.blocks();
        burst_writes += cnf.blocks() - 1;
        emit memory_writes_update(mem_writes);
    }
    cd.valid = false;
    cd.dirty = false;

    switch (cnf.replacement_policy()) {
    case MachineConfigCache::RP_LRU:
    {
        unsigned next_asi = associat_indx;
        unsigned tmp_asi = replc.lru[row][0];
        int i = 1;
        while (tmp_asi != associat_indx) {
            SANITY_ASSERT(i < (int)cnf.associativity(), "LRU lost the way from priority queue - kick");
            tmp_asi = replc.lru[row][i];
            replc.lru[row][i] = next_asi;
            next_asi = tmp_asi;
            i++;
        }
        break;
    }
    case MachineConfigCache::RP_LFU:
        replc.lfu[row][associat_indx] = 0;
        break;
    default:
        break;
    }
}

std::uint32_t Cache::base_address(std::uint32_t tag, unsigned row) const {
    return ((tag * cnf.blocks() * cnf.sets()) + (row * cnf.blocks())) << 2;
}

void Cache::update_statistics() const {
    emit statistics_update(stalled_cycles(), speed_improvement(), hit_rate());
}
