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
    [[nodiscard]] virtual size_t select_way_to_evict(size_t row) const = 0;

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

    [[nodiscard]] size_t select_way_to_evict(size_t row) const final;

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

    [[nodiscard]] size_t select_way_to_evict(size_t row) const final;

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

    [[nodiscard]] size_t select_way_to_evict(size_t row) const final;

    void update_stats(size_t way, size_t row, bool is_valid) final;

private:
    size_t associativity;
};

/**
 * Pseudo Last recently used policy
 *
 * Hardware-Friendly LRU Implementation
 */
class CachePolicyPLRU final : public CachePolicy {
public:
    /**
     * @param associativity     degree of assiciaivity
     * @param set_count         number of blocks / rows in a way (or sets in
     * cache)
     */
    CachePolicyPLRU(size_t associativity, size_t set_count);

    [[nodiscard]] size_t select_way_to_evict(size_t row) const final;

    void update_stats(size_t way, size_t row, bool is_valid) final;

private:
    /**
     * Pointer to Least Recently Used Block
     */
    std::vector<std::vector<uint32_t>> plru_ptr;
    const size_t associativity;
    const size_t associativityCLog2;
};

} // namespace machine

#endif // CACHE_POLICY_H
