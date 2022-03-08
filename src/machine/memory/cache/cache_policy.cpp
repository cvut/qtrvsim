#include "cache_policy.h"

#include "simulator_exception.h"
#include "utils.h"

namespace machine {

std::unique_ptr<CachePolicy>
CachePolicy::get_policy_instance(const CacheConfig *config) {
    if (config->enabled()) {
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
    } else {
        // Disabled cache will never use it.
        return { nullptr };
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

    try {
        // Statistics corresponding to single cache row
        std::vector<uint32_t> &row_stats = stats.at(row);
        uint32_t next_way = way;

        if (is_valid) {
            ssize_t i = associativity - 1;
            do {
                std::swap(row_stats.at(i), next_way);
                i--;
            } while (next_way != way);
        } else {
            size_t i = 0;
            do {
                std::swap(row_stats.at(i), next_way);
                i++;
            } while (next_way != way);
        }
    } catch (std::out_of_range &e) {
        throw SANITY_EXCEPTION("Out of range: LRU lost the way from priority queue.");
    }
}

size_t CachePolicyLRU::select_way_to_evict(size_t row) const {
    return stats.at(row).at(0);
}

CachePolicyLFU::CachePolicyLFU(size_t associativity, size_t set_count) {
    stats.resize(set_count, std::vector<uint32_t>(associativity, 0));
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
    for (size_t i = 0; i < stats.size(); i++) {
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

CachePolicyRAND::CachePolicyRAND(size_t associativity)
    : associativity(associativity) {
    // Reset random generator to make result reproducible.
    // Random is by default seeded by 1 (by cpp standard), so this makes it
    // consistent across multiple runs.
    // NOTE: Reproducibility applies only on the same execution environment.
    std::srand(1); // NOLINT(cert-msc51-cpp)
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