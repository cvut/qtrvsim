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

#include "data/cache_test_performance_data.h"
#include "qtmips_machine/memory/backend/memory.h"
#include "qtmips_machine/memory/cache/cache.h"
#include "qtmips_machine/memory/cache/cache_policy.h"
#include "qtmips_machine/memory/memory_bus.h"
#include "tst_machine.h"

#include <tests/utils/integer_decomposition.h>
#include <unordered_map>

using namespace machine;
using std::array;
using std::pair;
using std::size_t;
using std::tuple;
using Testcase = tuple<
    Endian,
    Address,
    size_t,
    IntegerDecomposition,
    IntegerDecomposition,
    CacheConfig>;

/*
 * Testing data for memory accesses
 * (all combinations are tested)
 */
constexpr array<Endian, 2> simulated_endians { BIG, LITTLE };
constexpr array<uint64_t, 5> accessed_addresses {
    0x0, 0xFFFF0, 0xFFFF1, 0xFFFFFF, 0xFFFFFFCC,
};
constexpr array<size_t, 3> strides { 0, 1, 2 };
constexpr array<uint64_t, 1> values { 0x4142434445464748 };

/*
 * Cache configuration parameters for testing
 * (all combinations are tested)
 */
constexpr array<CacheConfig::ReplacementPolicy, 3> replacement_policies {
    CacheConfig::RP_RAND, CacheConfig::RP_LFU, CacheConfig::RP_LRU
};
constexpr array<CacheConfig::WritePolicy, 3> write_policies {
    CacheConfig::WP_THROUGH_NOALLOC, // THIS IS BROKEN IN CACHE FOR STRIDE 1
    CacheConfig::WP_THROUGH_ALLOC, CacheConfig::WP_BACK
};
constexpr array<pair<unsigned, unsigned>, 3> organizations { {
    { 8, 1 },
    { 1, 8 },
    { 4, 4 },
} };
constexpr array<unsigned, 3> associativity_degrees { 1, 2, 4 };

// + 1 is  for disabled cache
constexpr size_t CONFIG_COUNT
    = (replacement_policies.size() * write_policies.size()
       * organizations.size() * associativity_degrees.size())
      + 1;

/**
 * Creates cache_config objects from all combinations of the above given
 * parameters.
 */
array<CacheConfig, CONFIG_COUNT> get_testing_cache_configs() {
    array<CacheConfig, CONFIG_COUNT> configs;
    configs.at(0).set_enabled(false);
    size_t i = 1;
    for (auto replacement_policy : replacement_policies) {
        for (auto write_policy : write_policies) {
            for (auto organization : organizations) {
                for (auto associativity : associativity_degrees) {
                    configs.at(i).set_enabled(true);
                    configs.at(i).set_replacement_policy(replacement_policy);
                    configs.at(i).set_write_policy(write_policy);
                    configs.at(i).set_set_count(organization.first);
                    configs.at(i).set_block_size(organization.second);
                    configs.at(i).set_associativity(associativity);
                    i += 1;
                }
            }
        }
    }
    Q_ASSERT(i == CONFIG_COUNT);
    return configs;
}

void MachineTests::cache_data() {
    QTest::addColumn<CacheConfig>("cache_c");
    QTest::addColumn<unsigned>("hit");
    QTest::addColumn<unsigned>("miss");

    CacheConfig cache_c;
    cache_c.set_write_policy(CacheConfig::WP_THROUGH_ALLOC);
    cache_c.set_enabled(true);
    cache_c.set_set_count(8);
    cache_c.set_block_size(1);
    cache_c.set_associativity(1);
    QTest::newRow("Directly mapped") << cache_c << (unsigned)3 << (unsigned)7;
    cache_c.set_set_count(1);
    cache_c.set_block_size(8);
    QTest::newRow("Wide") << cache_c << (unsigned)5 << (unsigned)5;
    cache_c.set_set_count(4);
    cache_c.set_block_size(4);
    QTest::newRow("Square") << cache_c << (unsigned)4 << (unsigned)6;
}

void MachineTests::cache() {
    QFETCH(CacheConfig, cache_c);
    QFETCH(unsigned, hit);
    QFETCH(unsigned, miss);

    Memory m(BIG);
    TrivialBus m_frontend(&m);
    Cache cache(&m_frontend, &cache_c);

    // Test reads //
    memory_write_u32(&m, 0x200, 0x24);
    memory_write_u32(&m, 0x204, 0x66);
    memory_write_u32(&m, 0x21c, 0x12);
    memory_write_u32(&m, 0x300, 0x32);
    for (int i = 0; i < 2; i++) {
        QCOMPARE(cache.read_u32(0x200_addr), (uint32_t)0x24);
        QCOMPARE(cache.read_u32(0x204_addr), (uint32_t)0x66);
        QCOMPARE(cache.read_u32(0x21c_addr), (uint32_t)0x12);
        QCOMPARE(cache.read_u32(0x300_addr), (uint32_t)0x32);
    }

    // Test writes //
    cache.write_u32(0x700_addr, 0x24);
    QCOMPARE(memory_read_u32(&m, 0x700), (uint32_t)0x24);
    cache.write_u32(0x700_addr, 0x23);
    QCOMPARE(memory_read_u32(&m, 0x700), (uint32_t)0x23);

    // Verify counts
    QCOMPARE(cache.get_hit_count(), hit);
    QCOMPARE(cache.get_miss_count(), miss);
}

void MachineTests::cache_correctness_data() {
    QTest::addColumn<Endian>("endian");
    QTest::addColumn<Address>("address");
    QTest::addColumn<size_t>("stride");
    QTest::addColumn<IntegerDecomposition>("value");
    QTest::addColumn<IntegerDecomposition>("result");
    QTest::addColumn<CacheConfig>("cache_config");
    // Used for cache hit/miss performance checking.
    QTest::addColumn<std::size_t>("case_number");

    size_t i = 0;
    for (auto cache_config : get_testing_cache_configs()) {
        for (auto endian : simulated_endians) {
            for (auto address : accessed_addresses) {
                for (auto stride : strides) {
                    for (auto value : values) {
                        // Result when read has `stride` bytes offset to the
                        // write.
                        auto result_value = (endian == BIG)
                                                ? (value << (stride * 8))
                                                : (value >> (stride * 8));
                        QTest::addRow(
                            "endian=%s, address=0x%lx, stride=%ld, "
                            "value=0x%lx, cache_config={ %d, "
                            "r=%d, wr=%d, s=%d, b=%d, a=%d }",
                            to_string(endian), address, stride, value,
                            cache_config.enabled(),
                            cache_config.replacement_policy(),
                            cache_config.write_policy(),
                            cache_config.set_count(), cache_config.block_size(),
                            cache_config.associativity())
                            << endian << Address(address) << stride
                            << IntegerDecomposition(value, endian)
                            << IntegerDecomposition(result_value, endian)
                            << cache_config << i;
                        i += 1;
                    }
                }
            }
        }
    }
}

void MachineTests::cache_correctness() {
    QFETCH(Endian, endian);
    QFETCH(Address, address);
    QFETCH(size_t, stride);
    QFETCH(IntegerDecomposition, value);
    QFETCH(IntegerDecomposition, result);
    QFETCH(CacheConfig, cache_config);
    QFETCH(size_t, case_number);

    // Prepare memory hierarchy
    Memory mem(endian);
    MemoryDataBus bus(endian);
    bus.insert_device_to_range(&mem, 0_addr, 0xffffffff_addr, false);
    Cache cache(&bus, &cache_config);

    // Uninitialized memory should read as zero
    QCOMPARE(cache.read_u8(address + stride), (uint8_t)0);
    QCOMPARE(cache.read_u16(address + stride), (uint16_t)0);
    QCOMPARE(cache.read_u32(address + stride), (uint32_t)0);
    QCOMPARE(cache.read_u64(address + stride), (uint64_t)0);

    // Writes value and reads it in all possible ways (by parts).

    // It is sufficient to test reads with stride to write of largest write. For
    // smaller writes, correct result is much harder to prepare.
    if (stride == 0) {
        cache.write_u8(address, value.u8.at(0));
        QCOMPARE(cache.read_u8(address), result.u8.at(0));

        cache.write_u16(address, value.u16.at(0));
        QCOMPARE(cache.read_u16(address), result.u16.at(0));
        for (size_t i = 0; i < 2; ++i) {
            QCOMPARE(cache.read_u8(address + i), result.u8.at(i));
        }

        cache.write_u32(address, value.u32.at(0));
        QCOMPARE(cache.read_u32(address), result.u32.at(0));
        for (size_t i = 0; i < 2; ++i) {
            QCOMPARE(cache.read_u16(address + 2 * i), result.u16.at(i));
        }
        for (size_t i = 0; i < 4; ++i) {
            QCOMPARE(cache.read_u8(address + i), result.u8.at(i));
        }
    }

    cache.write_u64(address, value.u64);
    QCOMPARE(cache.read_u64(address + stride), result.u64);
    for (size_t i = 0; i < 2; ++i) {
        QCOMPARE(cache.read_u32(address + stride + 4 * i), result.u32.at(i));
    }
    for (size_t i = 0; i < 4; ++i) {
        QCOMPARE(cache.read_u16(address + stride + 2 * i), result.u16.at(i));
    }
    for (size_t i = 0; i < 8; ++i) {
        QCOMPARE(cache.read_u8(address + stride + i), result.u8.at(i));
    }

    tuple<unsigned, unsigned> performance { cache.get_hit_count(),
                                            cache.get_miss_count() };

    // CODE USED FOR PERFORMANCE DATA GENERATION
    //        fprintf(
    //            stderr, "{ %d, %d }, ", cache.get_hit_count(),
    //            cache.get_miss_count());

    if (cache_config.replacement_policy() != CacheConfig::RP_RAND) {
        // Performance of random policy is implementation dependant and
        // meaningless.
        QCOMPARE(performance, cache_test_performance_data.at(case_number));
    }
}
