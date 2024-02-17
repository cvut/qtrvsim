#include "memory/cache/cache.h"

#include "memory/cache/cache_types.h"

#include <cstddef>

using ae = machine::AccessEffects; // For enum values, type is obvious from
                                   // context.

namespace machine {

Cache::Cache(
    FrontendMemory *memory,
    const CacheConfig *config,
    uint32_t memory_access_penalty_r,
    uint32_t memory_access_penalty_w,
    uint32_t memory_access_penalty_b,
    bool memory_access_enable_b)
    : FrontendMemory(memory->simulated_machine_endian)
    , cache_config(config)
    , mem(memory)
    , uncached_start(0xf0000000_addr)
    , uncached_last(0xfffffffe_addr)
    , access_pen_r(memory_access_penalty_r)
    , access_pen_w(memory_access_penalty_w)
    , access_pen_b(memory_access_penalty_b)
    , access_ena_b(memory_access_enable_b)
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

WriteResult Cache::write(
    Address destination,
    const void *source,
    size_t size,
    WriteOptions options) {
    if (!cache_config.enabled() || is_in_uncached_area(destination)
        || is_in_uncached_area(destination + size)) {
        mem_writes++;
        emit memory_writes_update(mem_writes);
        update_all_statistics();
        return mem->write(destination, source, size, options);
    }

    // FIXME: Get rid of the cast
    // access is mostly the same for read and write but one needs to write
    // to the address
    const bool changed
        = access(destination, const_cast<void *>(source), size, WRITE);

    if (cache_config.write_policy() != CacheConfig::WP_BACK) {
        mem_writes++;
        emit memory_writes_update(mem_writes);
        update_all_statistics();
        return mem->write(destination, source, size, options);
    }

    return { .n_bytes = size, .changed = changed };
}

ReadResult Cache::read(
    void *destination,
    Address source,
    size_t size,
    ReadOptions options) const {
    if (!cache_config.enabled() || is_in_uncached_area(source)
        || is_in_uncached_area(source + size)) {
        mem_reads++;
        emit memory_reads_update(mem_reads);
        update_all_statistics();
        return mem->read(destination, source, size, options);
    }

    if (options.type == ae::INTERNAL) {
        if (!(location_status(source) & LOCSTAT_CACHED)) {
            mem->read(destination, source, size, options);
        } else {
            internal_read(source, destination, size);
        }
        return {};
    }

    access(source, destination, size, READ);

    return {};
}
bool Cache::is_in_uncached_area(Address source) const {
    return (source >= uncached_start && source <= uncached_last);
}

void Cache::flush() {
    if (!cache_config.enabled()) {
        return;
    }

    for (size_t assoc_index = 0; assoc_index < cache_config.associativity();
         assoc_index += 1) {
        for (size_t set_index = 0; set_index < cache_config.set_count();
             set_index += 1) {
            if (dt[assoc_index][set_index].valid) {
                kick(assoc_index, set_index);
                emit cache_update(
                    assoc_index, set_index, 0, false, false, 0, nullptr, false);
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
        for (size_t assoc_index = 0; assoc_index < cache_config.associativity();
             assoc_index++) {
            for (size_t set_index = 0; set_index < cache_config.set_count();
                 set_index++) {
                emit cache_update(
                    assoc_index, set_index, 0, false, false, 0, nullptr, false);
            }
        }
    }
}

void Cache::internal_read(Address source, void *destination, size_t size) const {
    CacheLocation loc = compute_location(source);
    for (size_t assoc_index = 0; assoc_index < cache_config.associativity();
         assoc_index++) {
        if (dt[assoc_index][loc.row].valid
            && dt[assoc_index][loc.row].tag == loc.tag) {
            memcpy(
                destination,
                (byte *)&dt[assoc_index][loc.row].data[loc.col] + loc.byte,
                size);
            return;
        }
    }
    memset(destination, 0, size); // TODO is this correct
}

bool Cache::access(
    Address address,
    void *buffer,
    size_t size,
    AccessType access_type) const {
    const CacheLocation loc = compute_location(address);
    size_t way = find_block_index(loc);

    // check for zero because else last_affected_col can became
    // ULONG_MAX / BLOCK_ITEM_SIZE and update can take forever
    if (size == 0)
        return false;

    // search failed - cache miss
    if (way >= cache_config.associativity()) {
        // if write through we do not need to allocate cache line does not
        // allocate
        if (access_type == WRITE
            && cache_config.write_policy() == CacheConfig::WP_THROUGH_NOALLOC) {
            miss_write++;
            emit miss_update(get_miss_count());
            update_all_statistics();

            const size_t size_overflow
                = calculate_overflow_to_next_blocks(size, loc);
            if (size_overflow > 0) {
                const size_t size_within_block = size - size_overflow;
                return access(
                    address + size_within_block,
                    (byte *)buffer + size_within_block, size_overflow,
                    access_type);
            } else {
                return false;
            }
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
        if (access_type == WRITE) {
            hit_write++;
        } else {
            hit_read++;
        }
        emit hit_update(get_hit_count());
        update_all_statistics();
    } else {
        if (access_type == WRITE) {
            miss_write++;
        } else {
            miss_read++;
        }
        emit miss_update(get_miss_count());

        mem->read(
            cd.data.data(), calc_base_address(loc.tag, loc.row),
            cache_config.block_size() * BLOCK_ITEM_SIZE,
            { .type = ae::REGULAR });

        cd.valid = true;
        cd.dirty = false;
        cd.tag = loc.tag;

        change_counter += cache_config.block_size();
        mem_reads += cache_config.block_size();
        burst_reads += cache_config.block_size() - 1;
        emit memory_reads_update(mem_reads);
        update_all_statistics();
    }

    replacement_policy->update_stats(way, loc.row, cd.valid);

    const size_t size_overflow = calculate_overflow_to_next_blocks(size, loc);
    const size_t size_within_block = size - size_overflow;

    bool changed = false;

    if (access_type == READ) {
        memcpy(buffer, (byte *)&cd.data[loc.col] + loc.byte, size_within_block);
    } else if (access_type == WRITE) {
        cd.dirty = true;
        changed = memcmp(
                      (byte *)&cd.data[loc.col] + loc.byte, buffer,
                      size_within_block)
                  != 0;
        if (changed) {
            memcpy(
                ((byte *)&cd.data[loc.col]) + loc.byte, buffer,
                size_within_block);
            change_counter++;
        }
    }
    const auto last_affected_col
        = (loc.col * BLOCK_ITEM_SIZE + loc.byte + size_within_block - 1) / BLOCK_ITEM_SIZE;
    for (auto col = loc.col; col <= last_affected_col; col++) {
        emit cache_update(
            way, loc.row, col, cd.valid, cd.dirty, cd.tag, cd.data.data(),
            access_type);
    }

    if (size_overflow > 0) {
        // If access overlaps single cache row, perform access to next row.
        changed |= access(
            address + size_within_block, (byte *)buffer + size_within_block,
            size_overflow, access_type);
    }

    return changed;
}
size_t Cache::calculate_overflow_to_next_blocks(
    size_t access_size,
    const CacheLocation &loc) const {
    return std::max(
        (ptrdiff_t)(loc.col * BLOCK_ITEM_SIZE + loc.byte + access_size)
            - (ptrdiff_t)(cache_config.block_size() * BLOCK_ITEM_SIZE),
        { 0 });
}

size_t Cache::find_block_index(const CacheLocation &loc) const {
    uint32_t index = 0;
    while (
        index < cache_config.associativity()
        and (!dt[index][loc.row].valid or dt[index][loc.row].tag != loc.tag)) {
        index++;
    }
    return index;
}

void Cache::kick(size_t way, size_t row) const {
    struct CacheLine &cd = dt[way][row];
    if (cd.dirty && cache_config.write_policy() == CacheConfig::WP_BACK) {
        mem->write(
            calc_base_address(cd.tag, row), cd.data.data(),
            cache_config.block_size() * BLOCK_ITEM_SIZE, {});
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

Address Cache::calc_base_address(size_t tag, size_t row) const {
    return Address(
        (tag * cache_config.set_count() + row) * cache_config.block_size()
        * BLOCK_ITEM_SIZE);
}

CacheLocation Cache::compute_location(Address address) const {
    // Get address in multiples of 4 bytes (basic storage unit size) and get the
    // reminder to address individual byte within basic storage unit.
    auto word_index = address.get_raw() / BLOCK_ITEM_SIZE;
    auto byte = address.get_raw() % BLOCK_ITEM_SIZE;
    // Associativity does not influence location (hit will be on the
    // same place in each way). Lets therefore assume associativity degree 1.
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

enum LocationStatus Cache::location_status(Address address) const {
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
    st_cycles += (miss_read + miss_write) * cache_config.block_size();
    if (access_ena_b) {
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
    if (access_ena_b) {
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
