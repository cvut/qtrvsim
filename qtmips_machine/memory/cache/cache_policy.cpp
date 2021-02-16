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

#include "cache_policy.h"

#include "../../qtmipsexception.h"
#include "../../utils.h"

namespace machine {

std::unique_ptr<CachePolicy>
CachePolicy::get_policy_instance(const CacheConfig *config) {
    switch (config->replacement_policy()) {
    case CacheConfig::RP_RAND:
        return std::make_unique<CachePolicyRAND>(config->associativity());
    case CacheConfig::RP_LRU:
        return std::make_unique<CachePolicyLRU>(
            config->associativity(), config->set_count());
    case CacheConfig::RP_LFU:
        return std::make_unique<CachePolicyLFU>(
            config->associativity(), config->set_count());
    }

    Q_UNREACHABLE();
}

CachePolicyLRU::CachePolicyLRU(size_t associativity, size_t set_count)
    : associativity(associativity) {
    stats.resize(set_count);
    for (auto &row : stats) {
        row.reserve(associativity);
        for (size_t i = 0; i < associativity; i++) {
            row.push_back(i);
        }
    }
}

void CachePolicyLRU::update_stats(size_t way, size_t row, bool is_valid) {
    // The following code is essentially a single pass of bubble sort (with
    // temporary variable instead of inplace swapping) adding one element to
    // back or front (respectively) of a sorted array. The sort stops, when the
    // original location of the inserted element is reached and the original
    // instance is moved to the temporary variable (`next_way`).

    uint32_t next_way = way;

    if (is_valid) {
        ssize_t i = associativity - 1;
        do {
            SANITY_ASSERT(
                i >= 0, "LRU lost the way from priority queue - access");
            std::swap(stats.at(row).at(i), next_way);
            i--;
        } while (next_way != way);
    } else {
        size_t i = 0;
        do {
            SANITY_ASSERT(
                i < stats.size(),
                "LRU lost the way from priority queue - access");
            std::swap(stats.at(row).at(i), next_way);
            i++;
        } while (next_way != way);
    }
}

size_t CachePolicyLRU::select_way_to_evict(size_t row) const {
    return stats.at(row).at(0);
}

void CachePolicyLFU::update_stats(size_t way, size_t row, bool is_valid) {
    auto &stat_item = stats.at(row).at(way);

    if (is_valid) {
        stat_item += 1;
    } else {
        stat_item = 0;
    }
}

size_t CachePolicyLFU::select_way_to_evict(size_t row) const {
    size_t lowest = stats.at(row).at(0);
    size_t index = 0;
    for (size_t i = 1; i < stats.size(); i++) {
        if (stats.at(row).at(i) == 0) {
            // Only invalid blocks have zero stat
            return i;
        }
        if (lowest > stats.at(row).at(i)) {
            lowest = stats.at(row).at(i);
            index = i;
        }
    }
    return index;
}

CachePolicyLFU::CachePolicyLFU(size_t associativity, size_t set_count) {
    stats.resize(set_count, std::vector<uint32_t>(associativity, 0));
}

void CachePolicyRAND::update_stats(size_t way, size_t row, bool is_valid) {
    UNUSED(way) UNUSED(row) UNUSED(is_valid)

    // NOP
}

size_t CachePolicyRAND::select_way_to_evict(size_t row) const {
    UNUSED(row)
    return std::rand() % associativity; // NOLINT(cert-msc50-cpp)
}

} // namespace machine