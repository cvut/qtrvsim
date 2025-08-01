#include "tlb_policy.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <numeric>

namespace machine {

TLBPolicyRAND::TLBPolicyRAND(size_t assoc) : associativity(assoc) {
    std::srand(1);
}
size_t TLBPolicyRAND::select_way(size_t) const {
    return std::rand() % associativity;
}
void TLBPolicyRAND::notify_access(size_t, size_t, bool) {
    /* no state */
}

TLBPolicyLRU::TLBPolicyLRU(size_t assoc, size_t sets) : associativity(assoc), set_count(sets) {
    stats.resize(sets, std::vector<uint32_t>(assoc));
    for (auto &row : stats) {
        std::iota(row.begin(), row.end(), 0);
    }
}
size_t TLBPolicyLRU::select_way(size_t set) const {
    return stats[set][0];
}
void TLBPolicyLRU::notify_access(size_t set, size_t way, bool valid) {
    auto &row = stats[set];
    uint32_t next = way;
    if (valid) {
        for (int i = int(row.size()) - 1; i >= 0; --i) {
            std::swap(row[i], next);
            if (next == way) break;
        }
    } else {
        for (unsigned int &i : row) {
            std::swap(i, next);
            if (next == way) break;
        }
    }
}

TLBPolicyLFU::TLBPolicyLFU(size_t assoc, size_t sets) : associativity(assoc), set_count(sets) {
    stats.assign(sets, std::vector<uint32_t>(assoc, 0));
}
size_t TLBPolicyLFU::select_way(size_t set) const {
    const auto &row = stats[set];
    size_t idx = 0;
    uint32_t minv = row[0];
    for (size_t i = 1; i < row.size(); ++i) {
        if (row[i] == 0 || row[i] < minv) {
            minv = row[i];
            idx = i;
            if (minv == 0) break;
        }
    }
    return idx;
}
void TLBPolicyLFU::notify_access(size_t set, size_t way, bool valid) {
    if (valid) {
        stats[set][way]++;
    } else {
        stats[set][way] = 0;
    }
}

TLBPolicyPLRU::TLBPolicyPLRU(size_t assoc, size_t sets)
    : associativity(assoc)
    , set_count(sets)
    , c_log2(std::ceil(std::log2(float(assoc)))) {
    size_t tree_size = (1u << c_log2) - 1;
    tree.assign(sets, std::vector<uint8_t>(tree_size, 0));
}
size_t TLBPolicyPLRU::select_way(size_t set) const {
    const auto &bits = tree[set];
    size_t idx = 0;
    size_t node = 0;
    for (size_t lvl = 0; lvl < c_log2; ++lvl) {
        uint8_t b = bits[node];
        idx = (idx << 1) | b;
        node = ((1u << (lvl + 1)) - 1) + idx;
    }
    return std::min(idx, associativity - 1);
}
void TLBPolicyPLRU::notify_access(size_t set, size_t way, bool) {
    auto &bits = tree[set];
    size_t node = 0;
    for (size_t lvl = 0; lvl < c_log2; ++lvl) {
        uint8_t dir = (way >> (c_log2 - lvl - 1)) & 1;
        bits[node] = dir ? 0 : 1;
        node = ((1u << (lvl + 1)) - 1) + ((dir ? 1 : 0));
    }
}

std::unique_ptr<TLBPolicy>
make_tlb_policy(TLBPolicyKind kind, size_t associativity, size_t set_count) {
    switch (kind) {
    case TLBPolicyKind::RAND: return std::make_unique<TLBPolicyRAND>(associativity);
    case TLBPolicyKind::LRU: return std::make_unique<TLBPolicyLRU>(associativity, set_count);
    case TLBPolicyKind::LFU: return std::make_unique<TLBPolicyLFU>(associativity, set_count);
    case TLBPolicyKind::PLRU: return std::make_unique<TLBPolicyPLRU>(associativity, set_count);
    }
    return std::make_unique<TLBPolicyLRU>(associativity, set_count);
}

} // namespace machine
