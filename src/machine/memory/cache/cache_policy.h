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
 * Copyright (c) 2020      Jakub Dupak <dupak.jakub@gmail.com>
 * Copyright (c) 2020      Max Hollmann <hollmmax@fel.cvut.cz>
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

#ifndef CACHE_POLICY_H
#define CACHE_POLICY_H

#include "machineconfig.h"
#include "memory/cache/cache_types.h"

#include <cstdint>
#include <cstdlib>
#include <memory>

using std::size_t;

namespace machine {

/**
 * Cache replacement policy interface.
 *
 * For clarification of cache terminiology, see docstring of `Cache` in
 * `memory/cache/cache.h`.
 */
class CachePolicy {
public:
    virtual size_t select_way_to_evict(size_t row) const = 0;

    /**
     * To be called by cache on any change of validity.
     * @param way           associativity way
     * @param row           cache row (index of block/set)
     * @param is_valid      is cache data valid (as in `cd.valid`)
     */
    virtual void update_stats(size_t way, size_t row, bool is_valid) = 0;

    virtual ~CachePolicy() = default;

    static std::unique_ptr<CachePolicy>
    get_policy_instance(const CacheConfig *config);
};

/**
 * Last recently used policy
 *
 *  Keeps queue of associativity indexes from last accessed
 *   to most recently accesed.
 *  Empty indexes are shifted to the begining.
 */
class CachePolicyLRU final : public CachePolicy {
public:
    /**
     * @param associativity     degree of assiciaivity
     * @param set_count         number of blocks / rows in a way (or sets in
     * cache)
     */
    CachePolicyLRU(size_t associativity, size_t set_count);

    size_t select_way_to_evict(size_t row) const final;

    void update_stats(size_t way, size_t row, bool is_valid) final;

private:
    /**
     * Last access order queues for each cache set (row)
     */
    std::vector<std::vector<uint32_t>> stats;
    const size_t associativity;
};

/**
 * Least frequently used policy
 *
 *  Keeps statistics of access count for each associativity index.
 *  Resets statistics when set(row) is evicted.
 */
class CachePolicyLFU final : public CachePolicy {
public:
    /**
     * @param associativity     degree of assiciaivity
     * @param set_count         number of blocks / rows is way (or sets in
     * cache)
     */
    CachePolicyLFU(size_t associativity, size_t set_count);

    size_t select_way_to_evict(size_t row) const final;

    void update_stats(size_t way, size_t row, bool is_valid) final;

private:
    std::vector<std::vector<uint32_t>> stats;
};

class CachePolicyRAND final : public CachePolicy {
public:
    /**
     * @param associativity     degree of associativity
     */
    explicit CachePolicyRAND(size_t associativity);

    size_t select_way_to_evict(size_t row) const final;

    void update_stats(size_t way, size_t row, bool is_valid) final;

private:
    size_t associativity;
};

} // namespace machine

#endif // CACHE_POLICY_H
