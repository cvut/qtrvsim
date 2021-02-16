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

#include "memory/cache/cache_types.h"

namespace machine {

Cache::Cache(
    MemoryAccess *memory,
    const CacheConfig *config,
    uint32_t memory_access_penalty_r,
    uint32_t memory_access_penalty_w,
    uint32_t memory_access_penalty_b)
    : cache_config(config)
    , mem(memory)
    , uncached_start(0xf0000000)
    , uncached_last(0xfffffffe)
    , access_pen_r(memory_access_penalty_r)
    , access_pen_w(memory_access_penalty_w)
    , access_pen_b(memory_access_penalty_b)
    , replacement_policy(CachePolicy::get_policy_instance(config)) {
    // Skip memory allocation if cache is disabled
    if (!config->enabled()) {
        return;
    }

    dt.resize(
        config->associativity(),
        std::vector<CacheLine>(
            config->set_count(),
            { .valid = false,
              .dirty = false,
              .tag = 0,
              .data = std::vector<uint32_t>(config->block_size()) }));
}

Cache::~Cache() = default;

bool Cache::wword(uint32_t address, uint32_t value) {
    if (!cache_config.enabled() || is_in_uncached_area(address)) {
        mem_writes++;
        emit memory_writes_update(mem_writes);
        update_all_statistics();
        return mem->write_word(address, value);
    }

    uint32_t data;
    const bool changed = access(address, &data, true, value);

    if (cache_config.write_policy() != CacheConfig::WP_BACK) {
        mem_writes++;
        emit memory_writes_update(mem_writes);
        update_all_statistics();
        return mem->write_word(address, value);
    }
    return changed;
}

uint32_t Cache::rword(uint32_t address, bool debug_access) const {
    if (!cache_config.enabled() || is_in_uncached_area(address)) {
        mem_reads++;
        emit memory_reads_update(mem_reads);
        update_all_statistics();
        return mem->read_word(address, debug_access);
    }

    if (debug_access) {
        if (!(location_status(address) & LOCSTAT_CACHED)) {
            return mem->read_word(address, debug_access);
        }
        return debug_rword(address);
    }
    uint32_t data;
    access(address, &data, false);
    return data;
}

bool Cache::is_in_uncached_area(uint32_t source) const {
    return (source >= uncached_start && source <= uncached_last);
}

void Cache::flush() {
    if (!cache_config.enabled()) {
        return;
    }

    for (size_t way = 0; way < cache_config.associativity(); way += 1) {
        for (size_t set_index = 0; set_index < cache_config.set_count();
             set_index += 1) {
            if (dt[way][set_index].valid) {
                kick(way, set_index);
                emit cache_update(
                    way, set_index, 0, false, false, 0, nullptr, false);
            }
        }
    }
    change_counter++;
    update_all_statistics();
}

void Cache::sync() {
    flush();
}

void Cache::reset() {
    // Set all cells to invalid
    if (cache_config.enabled()) {
        for (auto &set : dt) {
            for (auto &block : set) {
                block.valid = false;
            }
        }
        // Note: We don't have to zero replacement policy data as those are
        // zeroed when first used on invalid cell.
    }

    hit_read = 0;
    hit_write = 0;
    miss_read = 0;
    miss_write = 0;
    mem_reads = 0;
    mem_writes = 0;
    burst_reads = 0;
    burst_writes = 0;

    emit hit_update(get_hit_count());
    emit miss_update(get_miss_count());
    emit memory_reads_update(get_read_count());
    emit memory_writes_update(get_write_count());
    update_all_statistics();

    if (cache_config.enabled()) {
        for (size_t way = 0; way < cache_config.associativity(); way++) {
            for (size_t set_index = 0; set_index < cache_config.set_count();
                 set_index++) {
                emit cache_update(
                    way, set_index, 0, false, false, 0, nullptr, false);
            }
        }
    }
}

uint32_t Cache::debug_rword(uint32_t address) const {
    CacheLocation loc = compute_location(address);
    for (size_t way = 0; way < cache_config.associativity(); way++)
        if (dt[way][loc.row].valid && dt[way][loc.row].tag == loc.tag)
            return dt[way][loc.row].data[loc.col];
    return 0;
}

bool Cache::access(uint32_t address, uint32_t *data, bool write, uint32_t value)
    const {
    const CacheLocation loc = compute_location(address);
    size_t way = find_block_index(loc);

    // search failed - cache miss
    if (way >= cache_config.associativity()) {
        // if write through we do not need to allocate cache line does not
        // allocate
        if (write
            && cache_config.write_policy() == CacheConfig::WP_THROUGH_NOALLOC) {
            miss_write++;
            emit miss_update(get_miss_count());
            update_all_statistics();
            return false;
        }

        way = replacement_policy->select_way_to_evict(loc.row);
        kick(way, loc.row);

        SANITY_ASSERT(
            way < cache_config.associativity(),
            "Probably unimplemented replacement policy");
    }

    struct CacheLine &cd = dt[way][loc.row];

    // Update statistics and otherwise read from memory
    if (cd.valid) {
        if (write) {
            hit_write++;
        } else {
            hit_read++;
        }
        emit hit_update(get_hit_count());
        update_all_statistics();
    } else {
        if (write) {
            miss_write++;
        } else {
            miss_read++;
        }
        emit miss_update(get_miss_count());
        for (size_t i = 0; i < cache_config.block_size(); i++) {
            cd.data[i]
                = mem->read_word(calc_base_address(loc.tag, loc.row) + (4 * i));
            change_counter++;
        }
        mem_reads += cache_config.block_size();
        burst_reads += cache_config.block_size() - 1;
        emit memory_reads_update(mem_reads);
        update_all_statistics();
    }

    replacement_policy->update_stats(way, loc.row, cd.valid);

    cd.valid = true; // We either write to it or we read from memory. Either way
                     // it's valid when we leave Cache class
    cd.dirty = cd.dirty || write;
    cd.tag = loc.tag;
    *data = cd.data[loc.col];

    bool changed = false;
    if (write) {
        changed = cd.data[loc.col] != value;
        cd.data[loc.col] = value;
    }

    emit cache_update(
        way, loc.row, loc.col, cd.valid, cd.dirty, cd.tag, cd.data.data(),
        write);
    if (changed) {
        change_counter++;
    }
    return changed;
}

size_t Cache::find_block_index(const CacheLocation &loc) const {
    uint32_t index = 0;
    while (
        index < cache_config.associativity()
        && (!dt[index][loc.row].valid || dt[index][loc.row].tag != loc.tag)) {
        index++;
    }
    return index;
}

void Cache::kick(size_t way, size_t row) const {
    struct CacheLine &cd = dt[way][row];
    if (cd.dirty && cache_config.write_policy() == CacheConfig::WP_BACK) {
        for (unsigned i = 0; i < cache_config.block_size(); i++)
            mem->write_word(
                calc_base_address(cd.tag, row) + (4 * i), cd.data[i]);
        mem_writes += cache_config.block_size();
        burst_writes += cache_config.block_size() - 1;
        emit memory_writes_update(mem_writes);
    }
    cd.valid = false;
    cd.dirty = false;

    change_counter++;

    replacement_policy->update_stats(way, row, false);
}

void Cache::update_all_statistics() const {
    emit statistics_update(
        get_stall_count(), get_speed_improvement(), get_hit_rate());
}

uint32_t Cache::calc_base_address(size_t tag, size_t row) const {
    return (
        (tag * cache_config.set_count() + row) * cache_config.block_size()
        * BLOCK_ITEM_SIZE);
}

CacheLocation Cache::compute_location(uint32_t address) const {
    // Get address in multiples of 4 byte (basic storage unit size) amd get the
    // reminder to addreess individual byte.
    auto word_index = address / BLOCK_ITEM_SIZE;
    auto byte = address % BLOCK_ITEM_SIZE;
    // Associativity does not incluence location (hit will be on the
    // same place in each way). Lets therefore assume associtivty degree 1.
    // Same address modulo `way_size_words` will alias (up to associativity).
    auto way_size_words = cache_config.set_count() * cache_config.block_size();
    // Index in a way, when rows and cols would be rearranged into 1D array.
    auto index_in_way = word_index % way_size_words;
    auto tag = word_index / way_size_words;

    return { .row = index_in_way / cache_config.block_size(),
             .col = index_in_way % cache_config.block_size(),
             .tag = tag,
             .byte = byte };
}

enum LocationStatus Cache::location_status(uint32_t address) const {
    const CacheLocation loc = compute_location(address);

    if (cache_config.enabled()) {
        for (auto const &set : dt) {
            auto const &block = set[loc.row];

            if (block.valid && block.tag == loc.tag) {
                if (block.dirty
                    && cache_config.write_policy() == CacheConfig::WP_BACK) {
                    return (enum LocationStatus)(
                        LOCSTAT_CACHED | LOCSTAT_DIRTY);
                } else {
                    return LOCSTAT_CACHED;
                }
            }
        }
    }
    return mem->location_status(address);
}

const CacheConfig &Cache::get_config() const {
    return cache_config;
}

uint32_t Cache::get_change_counter() const {
    return change_counter;
}

uint32_t Cache::get_hit_count() const {
    return hit_read + hit_write;
}

uint32_t Cache::get_miss_count() const {
    return miss_read + miss_write;
}

uint32_t Cache::get_read_count() const {
    return mem_reads;
}

uint32_t Cache::get_write_count() const {
    return mem_writes;
}

uint32_t Cache::get_stall_count() const {
    uint32_t st_cycles
        = mem_reads * (access_pen_r - 1) + mem_writes * (access_pen_w - 1);
    if (access_pen_b != 0) {
        st_cycles -= burst_reads * (access_pen_r - access_pen_b)
                     + burst_writes * (access_pen_w - access_pen_b);
    }
    return st_cycles;
}

double Cache::get_speed_improvement() const {
    uint32_t lookup_time;
    uint32_t mem_access_time;
    uint32_t comp = hit_read + hit_write + miss_read + miss_write;
    if (comp == 0) {
        return 100.0;
    }
    lookup_time = hit_read + miss_read;
    if (cache_config.write_policy() == CacheConfig::WP_BACK) {
        lookup_time += hit_write + miss_write;
    }
    mem_access_time = mem_reads * access_pen_r + mem_writes * access_pen_w;
    if (access_pen_b != 0) {
        mem_access_time -= burst_reads * (access_pen_r - access_pen_b)
                           + burst_writes * (access_pen_w - access_pen_b);
    }
    return (
        (double)((miss_read + hit_read) * access_pen_r + (miss_write + hit_write) * access_pen_w)
        / (double)(lookup_time + mem_access_time) * 100);
}

double Cache::get_hit_rate() const {
    uint32_t comp = hit_read + hit_write + miss_read + miss_write;
    if (comp == 0) {
        return 0.0;
    }
    return (double)(hit_read + hit_write) / (double)comp * 100.0;
}

} // namespace machine