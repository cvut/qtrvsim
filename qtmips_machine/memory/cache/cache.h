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

#ifndef CACHE_H
#define CACHE_H

#include "machineconfig.h"
#include "memory/backend/memory.h"
#include "memory/cache/cache_policy.h"
#include "memory/cache/cache_types.h"

#include <cstdint>
#include <memory>

namespace machine {

constexpr size_t BLOCK_ITEM_SIZE = sizeof(uint32_t);

/**
 * NOTE ON TERMINILOGY:
 * N-way set associative cache consist of N ways (where N is degree
 * of associtivity). Arguments requesting N are called `associativity` of the
 * cache. Each way consist of blocks. (When we want to highlight, that we talk
 * about data + management tags, we speek of cache line. When we speek about
 * location of block within a way, we use the term `row`. Each block consits of
 * some number of basic storage units (here `uint32_t`). To locate a single unit
 * withing block, we use therm `col` (as column).
 *
 * Set is consists of all block on the same row accross the ways.
 *
 * We can imagine a cache as 3D array indexed via triple (`way`, `row`, `col`).
 * Row and col are derived from part of a address deterministically. The rest
 * of the address is called `tag`. Set is obtained via linear search and placing
 * into cache, it is determined by cache replacement policy
 * (see `memory/cache/cache_policy.h`).
 */
class Cache : public MemoryAccess {
    Q_OBJECT
public:
    Cache(
        MemoryAccess *memory,
        const CacheConfig *config,
        uint32_t memory_access_penalty_r = 1,
        uint32_t memory_access_penalty_w = 1,
        uint32_t memory_access_penalty_b = 0);

    ~Cache() override;

    bool wword(uint32_t address, uint32_t value) override;
    uint32_t rword(uint32_t address, bool debug_access = false) const override;
    uint32_t get_change_counter() const override;

    void flush();         // flush cache
    void sync() override; // Same as flush

    uint32_t get_hit_count() const;       // Number of recorded hits
    uint32_t get_miss_count() const;      // Number of recorded misses
    uint32_t get_read_count() const;      // Number backing/main memory reads
    uint32_t get_write_count() const;     // Number backing/main memory writes
    uint32_t get_stall_count() const;     // Number of wasted cycles in
                                          // memory waiting statistic
    double get_speed_improvement() const; // Speed improvement in percents in
                                          // comare with no used cache
    double get_hit_rate() const;          // Usage efficiency in percents

    void reset(); // Reset whole state of cache

    const CacheConfig &get_config() const;

    enum LocationStatus location_status(uint32_t address) const override;

signals:

    void hit_update(uint32_t) const;

    void miss_update(uint32_t) const;

    void statistics_update(
        uint32_t stalled_cycles,
        double speed_improv,
        double hit_rate) const;
    void cache_update(
        size_t way,
        size_t row,
        size_t col,
        bool valid,
        bool dirty,
        size_t tag,
        const uint32_t *data,
        bool write) const;

    void memory_writes_update(uint32_t) const;

    void memory_reads_update(uint32_t) const;

private:
    const CacheConfig cache_config;
    MemoryAccess *const mem = nullptr;
    const uint32_t uncached_start;
    const uint32_t uncached_last;
    const uint32_t access_pen_r, access_pen_w, access_pen_b;
    const std::unique_ptr<CachePolicy> replacement_policy;

    mutable std::vector<std::vector<CacheLine>> dt;

    mutable uint32_t hit_read = 0, miss_read = 0, hit_write = 0, miss_write = 0,
                     mem_reads = 0, mem_writes = 0, burst_reads = 0,
                     burst_writes = 0, change_counter = 0;

    void debug_read(uint32_t source, void *destination, size_t size) const;

    uint32_t calc_base_address(size_t tag, size_t row) const;

    uint32_t debug_rword(uint32_t address) const;

    bool access(uint32_t address, uint32_t *data, bool write, uint32_t value = 0)
        const;

    void kick(size_t way, size_t row) const;

    uint32_t base_address(uint32_t tag, unsigned row) const;

    void update_all_statistics() const;

    CacheLocation compute_location(uint32_t address) const;

    /**
     * Searches for given tag in a set
     *
     * @param loc       requested location in cache
     * @return          associatity index of found block, max index + 1 if not
     * found
     */
    size_t find_block_index(const CacheLocation &loc) const;

    bool is_in_uncached_area(uint32_t source) const;
};

} // namespace machine

#endif // CACHE_H
