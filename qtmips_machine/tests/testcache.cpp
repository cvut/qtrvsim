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

#include "qtmips_machine/cache.h"
#include "tst_machine.h"

using namespace machine;

void MachineTests::cache_data() {
    QTest::addColumn<MachineConfigCache>("cache_c");
    QTest::addColumn<unsigned>("hit");
    QTest::addColumn<unsigned>("miss");

    MachineConfigCache cache_c;
    cache_c.set_write_policy(MachineConfigCache::WP_THROUGH_ALLOC);
    cache_c.set_enabled(true);
    cache_c.set_sets(8);
    cache_c.set_blocks(1);
    cache_c.set_associativity(1);
    QTest::newRow("Directly mapped") << cache_c << (unsigned)3 << (unsigned)7;
    cache_c.set_sets(1);
    cache_c.set_blocks(8);
    QTest::newRow("Wide") << cache_c << (unsigned)5 << (unsigned)5;
    cache_c.set_sets(4);
    cache_c.set_blocks(4);
    QTest::newRow("Square") << cache_c << (unsigned)4 << (unsigned)6;
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
        QCOMPARE(cch.read_word(0x200), (uint32_t)0x24);
        QCOMPARE(cch.read_word(0x204), (uint32_t)0x66);
        QCOMPARE(cch.read_word(0x21c), (uint32_t)0x12);
        QCOMPARE(cch.read_word(0x300), (uint32_t)0x32);
    }

    // Test writes //
    cch.write_word(0x700, 0x24);
    QCOMPARE(m.read_word(0x700), (uint32_t)0x24);
    cch.write_word(0x700, 0x23);
    QCOMPARE(m.read_word(0x700), (uint32_t)0x23);

    // Verify counts
    QCOMPARE(cch.hit(), hit);
    QCOMPARE(cch.miss(), miss);
}
