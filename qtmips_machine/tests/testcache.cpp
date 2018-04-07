#include "tst_machine.h"
#include "cache.h"

using namespace machine;

void MachineTests::cache_data() {
    QTest::addColumn<MachineConfigCache>("cache_c");
    QTest::addColumn<unsigned>("hit");
    QTest::addColumn<unsigned>("miss");

    MachineConfigCache cache_c;
    cache_c.set_write_policy(MachineConfigCache::WP_TROUGH);
    cache_c.set_enabled(true);
    cache_c.set_sets(8);
    cache_c.set_blocks(1);
    cache_c.set_associativity(1);
    QTest::newRow("Directly mapped") << cache_c \
                                     << (unsigned)3 \
                                     << (unsigned)7;
    cache_c.set_sets(1);
    cache_c.set_blocks(8);
    QTest::newRow("Wide") << cache_c \
                                     << (unsigned)5 \
                                     << (unsigned)5;
    cache_c.set_sets(4);
    cache_c.set_blocks(4);
    QTest::newRow("Square") << cache_c \
                                     << (unsigned)4 \
                                     << (unsigned)6;
}

void MachineTests::cache() {
    QFETCH(MachineConfigCache, cache_c);
    QFETCH(unsigned, hit);
    QFETCH(unsigned, miss);

    Memory m;
    Cache cch(&m, &cache_c);

    // Test reads //
    m.write_word(0x200, 0x24);
    m.write_word(0x204, 0x66);
    m.write_word(0x21c, 0x12);
    m.write_word(0x300, 0x32);
    for (int i = 0; i < 2; i++) {
        QCOMPARE(cch.read_word(0x200), (std::uint32_t)0x24);
        QCOMPARE(cch.read_word(0x204), (std::uint32_t)0x66);
        QCOMPARE(cch.read_word(0x21c), (std::uint32_t)0x12);
        QCOMPARE(cch.read_word(0x300), (std::uint32_t)0x32);
    }

    // Test writes //
    cch.write_word(0x700, 0x24);
    QCOMPARE(m.read_word(0x700), (std::uint32_t)0x24);
    cch.write_word(0x700, 0x23);
    QCOMPARE(m.read_word(0x700), (std::uint32_t)0x23);

    // Verify counts
    QCOMPARE(cch.hit(), hit);
    QCOMPARE(cch.miss(), miss);
}
