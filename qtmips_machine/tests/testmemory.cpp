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

#include "qtmips_machine/machinedefs.h"
#include "qtmips_machine/memory/backend/memory.h"
#include "qtmips_machine/memory/memory_bus.h"
#include "qtmips_machine/memory/memory_utils.h"
#include "tst_machine.h"
#include "utils/integer_decomposition.h"

using namespace machine;

// Default memory testing data. Some tests may use other values, where it
// makes better sense.
constexpr array<Endian, 2> default_endians { BIG, LITTLE };
constexpr array<Offset, 5> default_addresses { 0x0, 0xFFFFFFFC, 0xFFFF0,
                                               0xFFFF1, 0xFFFFFF };
constexpr array<Offset, 4> default_strides { 0, 1, 2, 3 };
constexpr array<uint64_t, 1> default_values { 0x4142434445464748 };

/**
 * Test the IntegerDecomposition util, that is used for testing later.
 */
void MachineTests::integer_decomposition() {
    const uint64_t value = 0x0102030405060708;

    IntegerDecomposition big_endian(value, BIG);

    // Expected sub-values done by hand.
    array<uint32_t, 2> be_u32 { 0x01020304, 0x05060708 };
    array<uint16_t, 4> be_u16 { 0x0102, 0x0304, 0x0506, 0x0708 };
    array<uint8_t, 8> be_u8 { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };

    QCOMPARE(big_endian.u64, (uint64_t)0x0102030405060708);
    QCOMPARE(big_endian.u32, be_u32);
    QCOMPARE(big_endian.u16, be_u16);
    QCOMPARE(big_endian.u8, be_u8);

    IntegerDecomposition little_endian(value, LITTLE);

    // Little endian arrays are reversed to big endian ones.
    array<uint32_t, 2> le_u32 = be_u32;
    std::reverse(le_u32.begin(), le_u32.end());
    array<uint16_t, 4> le_u16 = be_u16;
    std::reverse(le_u16.begin(), le_u16.end());
    array<uint8_t, 8> le_u8 = be_u8;
    std::reverse(le_u8.begin(), le_u8.end());

    // Const cast is just for types to match, this way reverse is simpler.
    QCOMPARE(little_endian.u64, (uint64_t)0x0102030405060708);
    QCOMPARE(little_endian.u32, (const array<uint32_t, 2>)le_u32);
    QCOMPARE(little_endian.u16, (const array<uint16_t, 4>)le_u16);
    QCOMPARE(little_endian.u8, (const array<uint8_t, 8>)le_u8);
}

/**
 * Common testing dataset generator.
 *
 * @tparam N1   size of first argument array
 * @tparam N2   size of second argument array
 * @tparam N3   size of third argument array
 * @tparam N4   size of fourth argument array
 * @param simulated_endians     array of endians to test
 * @param accessed_addresses    array of addresses to test at
 * @param strides   array of offsets of reads to the previous write (because
 *                  of endian differences), only tested on 64b write
 * @param values    values to use for memory access, for smaller accesses
 *                  part of the value is use
 */
template<size_t N1, size_t N2, size_t N3, size_t N4>
constexpr void prepare_data(
    const array<Endian, N1> &simulated_endians,
    const array<Offset, N2> &accessed_addresses,
    const array<Offset, N3> &strides,
    const array<uint64_t, N4> &values) {
    QTest::addColumn<Endian>("endian");
    QTest::addColumn<Offset>("address");
    QTest::addColumn<Offset>("stride");
    QTest::addColumn<IntegerDecomposition>("value");
    QTest::addColumn<IntegerDecomposition>("result");

    for (auto endian : simulated_endians) {
        for (auto address : accessed_addresses) {
            for (auto stride : strides) {
                for (auto value : values) {
                    // Result when read has `stride` bytes offset to the write.
                    auto result_value = (endian == BIG)
                                            ? (value << (stride * 8))
                                            : (value >> (stride * 8));
                    QTest::addRow(
                        "endian=%s, address=0x%lx, stride=%ld, value=0x%lx",
                        to_string(endian), address, stride, value)
                        << endian << address << stride
                        << IntegerDecomposition(value, endian)
                        << IntegerDecomposition(result_value, endian);
                }
            }
        }
    }
}

void MachineTests::memory_data() {
    prepare_data(
        default_endians, default_addresses, default_strides, default_values);
}

void MachineTests::memory() {
    QFETCH(Endian, endian);
    QFETCH(Offset, address);
    QFETCH(Offset, stride);
    QFETCH(IntegerDecomposition, value);
    QFETCH(IntegerDecomposition, result);

    Memory m(endian);

    // Uninitialized memory should read as zero
    QCOMPARE(memory_read_u8(&m, address + stride), (uint8_t)0);
    QCOMPARE(memory_read_u16(&m, address + stride), (uint16_t)0);
    QCOMPARE(memory_read_u32(&m, address + stride), (uint32_t)0);
    QCOMPARE(memory_read_u64(&m, address + stride), (uint64_t)0);

    // Writes value and reads it in all possible ways (by parts).

    // It is sufficient to test reads with stride to write of largest write. For
    // smaller writes, correct result is much harder to prepare.
    if (stride == 0) {
        memory_write_u8(&m, address, value.u8.at(0));
        QCOMPARE(memory_read_u8(&m, address), result.u8.at(0));

        memory_write_u16(&m, address, value.u16.at(0));
        QCOMPARE(memory_read_u16(&m, address), result.u16.at(0));
        for (size_t i = 0; i < 2; ++i) {
            QCOMPARE(memory_read_u8(&m, address + i), result.u8.at(i));
        }

        memory_write_u32(&m, address, value.u32.at(0));
        QCOMPARE(memory_read_u32(&m, address), result.u32.at(0));
        for (size_t i = 0; i < 2; ++i) {
            QCOMPARE(memory_read_u16(&m, address + 2 * i), result.u16.at(i));
        }
        for (size_t i = 0; i < 4; ++i) {
            QCOMPARE(memory_read_u8(&m, address + i), result.u8.at(i));
        }
    }

    memory_write_u64(&m, address, value.u64);
    QCOMPARE(memory_read_u64(&m, address + stride), result.u64);
    for (size_t i = 0; i < 2; ++i) {
        QCOMPARE(
            memory_read_u32(&m, address + stride + 4 * i), result.u32.at(i));
    }
    for (size_t i = 0; i < 4; ++i) {
        QCOMPARE(
            memory_read_u16(&m, address + stride + 2 * i), result.u16.at(i));
    }
    for (size_t i = 0; i < 8; ++i) {
        QCOMPARE(memory_read_u8(&m, address + stride + i), result.u8.at(i));
    }
}

void MachineTests::memory_section_data() {
    constexpr array<Offset, 4> addresses { 0x0, 0xFFFFFFF4, 0xFFFF00,
                                           0xFFFFF2 };
    prepare_data(default_endians, addresses, default_strides, default_values);
}

void MachineTests::memory_section() {
    QFETCH(Endian, endian);
    QFETCH(Offset, address);
    QFETCH(Offset, stride);
    QFETCH(IntegerDecomposition, value);
    QFETCH(IntegerDecomposition, result);

    Memory m(endian);

    // First section shouldn't exists
    QCOMPARE(m.get_section(address, false), (MemorySection *)nullptr);

    // Create section
    MemorySection *s = m.get_section(address, true);
    QVERIFY(s != nullptr);

    // Writes value and reads it in all possible ways (by parts).

    // It is sufficient to test reads with stride to write of largest write. For
    // smaller writes, correct result is much harder to prepare.
    if (stride == 0) {
        Offset section_offset = address % MEMORY_SECTION_SIZE;

        memory_write_u8(&m, address, value.u8.at(0));
        QCOMPARE(memory_read_u8(s, section_offset), result.u8.at(0));

        memory_write_u16(&m, address, value.u16.at(0));
        QCOMPARE(memory_read_u16(s, section_offset), result.u16.at(0));
        for (size_t i = 0; i < 2; ++i) {
            QCOMPARE(memory_read_u8(s, section_offset + i), result.u8.at(i));
        }

        memory_write_u32(&m, address, value.u32.at(0));
        QCOMPARE(memory_read_u32(s, section_offset), result.u32.at(0));
        for (size_t i = 0; i < 2; ++i) {
            QCOMPARE(
                memory_read_u16(s, section_offset + 2 * i), result.u16.at(i));
        }
        for (size_t i = 0; i < 4; ++i) {
            QCOMPARE(memory_read_u8(s, section_offset + i), result.u8.at(i));
        }
    }

    Offset section_offset = (address + stride) % MEMORY_SECTION_SIZE;

    memory_write_u64(&m, address, value.u64);
    QCOMPARE(memory_read_u64(s, section_offset), result.u64);
    for (size_t i = 0; i < 2; ++i) {
        QCOMPARE(memory_read_u32(s, section_offset + 4 * i), result.u32.at(i));
    }
    for (size_t i = 0; i < 4; ++i) {
        QCOMPARE(memory_read_u16(s, section_offset + 2 * i), result.u16.at(i));
    }
    for (size_t i = 0; i < 8; ++i) {
        QCOMPARE(memory_read_u8(s, section_offset + i), result.u8.at(i));
    }
}

void prepare_endian_test() {
    QTest::addColumn<Endian>("endian");
    for (auto endian : default_endians) {
        QTest::addRow("endian=%s", to_string(endian)) << endian;
    }
}

void MachineTests::memory_compare_data() {
    prepare_endian_test();
}

void MachineTests::memory_compare() {
    QFETCH(Endian, endian);

    Memory m1(endian), m2(endian);
    QCOMPARE(m1, m2);
    memory_write_u8(&m1, 0x20, 0x0);
    QVERIFY(m1 != m2); // This should not be equal as this identifies also
    // memory write (difference between no write and zero
    // write)
    memory_write_u8(&m1, 0x20, 0x24);
    QVERIFY(m1 != m2);
    memory_write_u8(&m2, 0x20, 0x23);
    QVERIFY(m1 != m2);
    memory_write_u8(&m2, 0x20, 0x24);
    QCOMPARE(m1, m2);
    // Do the same with some other section
    memory_write_u8(&m1, 0xFFFF20, 0x24);
    QVERIFY(m1 != m2);
    memory_write_u8(&m2, 0xFFFF20, 0x24);
    QCOMPARE(m1, m2);
    // And also check memory copy
    Memory m3(m1);
    QCOMPARE(m1, m3);
    memory_write_u8(&m3, 0x18, 0x22);
    QVERIFY(m1 != m3);
}

void MachineTests::memory_write_ctl_data() {
    QTest::addColumn<AccessControl>("ctl");
    QTest::addColumn<Memory>("result");

    for (auto endian : default_endians) {
        Memory mem(endian);

        QTest::addRow("AC_NONE, endian=%s", to_string(endian))
            << AC_NONE << mem;
        memory_write_u8(&mem, 0x20, 0x30);
        QTest::addRow("AC_I8, endian=%s", to_string(endian)) << AC_I8 << mem;
        QTest::addRow("AC_U8, endian=%s", to_string(endian)) << AC_U8 << mem;
        memory_write_u16(&mem, 0x20, 0x2930);
        QTest::addRow("AC_I16, endian=%s", to_string(endian)) << AC_I16 << mem;
        QTest::addRow("AC_U16, endian=%s", to_string(endian)) << AC_U16 << mem;
        memory_write_u32(&mem, 0x20, 0x27282930);
        QTest::addRow("AC_I32, endian=%s", to_string(endian)) << AC_I32 << mem;
        QTest::addRow("AC_U32, endian=%s", to_string(endian)) << AC_U32 << mem;
        memory_write_u64(&mem, 0x20, 0x2324252627282930ULL);
        QTest::addRow("AC_I64, endian=%s", to_string(endian)) << AC_I64 << mem;
        QTest::addRow("AC_U64, endian=%s", to_string(endian)) << AC_U64 << mem;
    }
}

void MachineTests::memory_write_ctl() {
    QFETCH(AccessControl, ctl);
    QFETCH(Memory, result);

    // Memory is not supposed to be read directly as it does not implement
    // frontend memory. TrivialBus was introduced to wrap Memory into the most
    // simple FrontendMemory with no additional functionality.
    Memory mem(result.simulated_machine_endian);
    TrivialBus bus(&mem);
    bus.write_ctl(ctl, 0x20_addr, (uint64_t)0x2324252627282930ULL);
    printf("DIRECT %lx\n", memory_read_u64(&mem, 0x20));
    printf("%lx\n", bus.read_ctl(ctl, 0x20_addr).as_u64());
    QCOMPARE(mem, result);
}

void MachineTests::memory_read_ctl_data() {
    prepare_data(
        default_endians, default_addresses, default_strides, default_values);
}

void MachineTests::memory_read_ctl() {
    QFETCH(Endian, endian);
    QFETCH(Offset, address);
    QFETCH(Offset, stride);
    QFETCH(IntegerDecomposition, value);
    QFETCH(IntegerDecomposition, result);

    Address frontend_address(address);
    Memory mem(endian);
    TrivialBus bus(&mem);

    bus.write_u64(frontend_address, value.u64);

    QCOMPARE(bus.read_ctl(AC_U64, frontend_address + stride), result.u64);

    for (size_t i = 0; i < 2; ++i) {
        QCOMPARE(
            bus.read_ctl(AC_U32, frontend_address + stride + 4 * i),
            result.u32.at(i));
    }
    for (size_t i = 0; i < 4; ++i) {
        QCOMPARE(
            bus.read_ctl(AC_U16, frontend_address + stride + 2 * i),
            result.u16.at(i));
    }
    for (size_t i = 0; i < 4; ++i) {
        QCOMPARE(
            bus.read_ctl(AC_I16, frontend_address + stride + 2 * i),
            (int16_t)result.u16.at(i));
    }
    for (size_t i = 0; i < 8; ++i) {
        QCOMPARE(
            bus.read_ctl(AC_U8, frontend_address + stride + i),
            result.u8.at(i));
    }
    for (size_t i = 0; i < 8; ++i) {
        QCOMPARE(
            bus.read_ctl(AC_I8, frontend_address + stride + i),
            (int8_t)result.u8.at(i));
    }
}
