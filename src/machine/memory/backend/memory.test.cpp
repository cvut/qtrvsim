#include "memory.test.h"

#include "common/endian.h"
#include "machine/machinedefs.h"
#include "machine/memory/backend/memory.h"
#include "machine/memory/memory_bus.h"
#include "machine/memory/memory_utils.h"
#include "tests/utils/integer_decomposition.h"

#include <cinttypes>

using namespace machine;

// Default memory testing data. Some tests may use other values, where it
// makes better sense.
constexpr array<Endian, 2> default_endians { BIG, LITTLE };
constexpr array<Offset, 5> default_addresses { 0x0, 0xFFFFFFFC, 0xFFFF0, 0xFFFF1, 0xFFFFFF };
constexpr array<Offset, 4> default_strides { 0, 1, 2, 3 };
constexpr array<uint64_t, 1> default_values { 0x4142434445464748 };

/**
 * Test the IntegerDecomposition util, that is used for testing later.
 */
void TestMemory::integer_decomposition() {
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
                    auto result_value
                        = (endian == BIG) ? (value << (stride * 8)) : (value >> (stride * 8));
                    QTest::addRow(
                        "endian=%s, address=0x%lx, stride=%ld, value=0x%" PRIx64, to_string(endian),
                        address, stride, value)
                        << endian << address << stride << IntegerDecomposition(value, endian)
                        << IntegerDecomposition(result_value, endian);
                }
            }
        }
    }
}

void TestMemory::memory_data() {
    prepare_data(default_endians, default_addresses, default_strides, default_values);
}

void TestMemory::memory() {
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
        QCOMPARE(memory_read_u32(&m, address + stride + 4 * i), result.u32.at(i));
    }
    for (size_t i = 0; i < 4; ++i) {
        QCOMPARE(memory_read_u16(&m, address + stride + 2 * i), result.u16.at(i));
    }
    for (size_t i = 0; i < 8; ++i) {
        QCOMPARE(memory_read_u8(&m, address + stride + i), result.u8.at(i));
    }
}

void TestMemory::memory_section_data() {
    constexpr array<Offset, 4> addresses { 0x0, 0xFFFFFFF4, 0xFFFF00, 0xFFFFF2 };
    prepare_data(default_endians, addresses, default_strides, default_values);
}

void TestMemory::memory_section() {
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
            QCOMPARE(memory_read_u16(s, section_offset + 2 * i), result.u16.at(i));
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

void TestMemory::memory_compare_data() {
    prepare_endian_test();
}

void TestMemory::memory_compare() {
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

void TestMemory::memory_write_ctl_data() {
    QTest::addColumn<AccessControl>("ctl");
    QTest::addColumn<Memory>("result");

    for (auto endian : default_endians) {
        Memory mem(endian);

        QTest::addRow("AC_NONE, endian=%s", to_string(endian)) << AC_NONE << mem;
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

void TestMemory::memory_write_ctl() {
    QFETCH(AccessControl, ctl);
    QFETCH(Memory, result);

    // Memory is not supposed to be read directly as it does not implement
    // frontend memory. TrivialBus was introduced to wrap Memory into the most
    // simple FrontendMemory with no additional functionality.
    Memory mem(result.simulated_machine_endian);
    TrivialBus bus(&mem);
    bus.write_ctl(ctl, 0x20_addr, (uint64_t)0x2324252627282930ULL);
    QCOMPARE(mem, result);
}

void TestMemory::memory_read_ctl_data() {
    prepare_data(default_endians, default_addresses, default_strides, default_values);
}

void TestMemory::memory_read_ctl() {
    QFETCH(Endian, endian);
    QFETCH(Offset, address);
    QFETCH(Offset, stride);
    QFETCH(IntegerDecomposition, value);
    QFETCH(IntegerDecomposition, result);

    Address frontend_address(address);
    Memory mem(endian);
    TrivialBus bus(&mem);

    bus.write_u64(frontend_address, value.u64);

    QCOMPARE(bus.read_ctl(AC_U64, frontend_address + stride).as_u64(), result.u64);

    for (size_t i = 0; i < 2; ++i) {
        QCOMPARE(
            bus.read_ctl(AC_U32, frontend_address + stride + 4 * i).as_u32(), result.u32.at(i));
    }
    for (size_t i = 0; i < 4; ++i) {
        QCOMPARE(
            bus.read_ctl(AC_U16, frontend_address + stride + 2 * i).as_u16(), result.u16.at(i));
    }
    for (size_t i = 0; i < 4; ++i) {
        QCOMPARE(
            bus.read_ctl(AC_I16, frontend_address + stride + 2 * i).as_i16(),
            (int16_t)result.u16.at(i));
    }
    for (size_t i = 0; i < 8; ++i) {
        QCOMPARE(bus.read_ctl(AC_U8, frontend_address + stride + i).as_u8(), result.u8.at(i));
    }
    for (size_t i = 0; i < 8; ++i) {
        QCOMPARE(
            bus.read_ctl(AC_I8, frontend_address + stride + i).as_i8(), (int8_t)result.u8.at(i));
    }
}

void TestMemory::memory_memtest_data() {
    QTest::addColumn<Endian>("endian");
    QTest::addColumn<Offset>("address");

    for (auto endian : default_endians) {
        for (auto address : default_addresses) {
            QTest::addRow("endian=%s, address=0x%lx", to_string(endian), address)
                << endian << address;
        }
    }
}

void TestMemory::memory_memtest() {
    QFETCH(Endian, endian);
    QFETCH(Offset, address);
    Address frontend_address(address);
    Memory mem(endian);
    TrivialBus bus(&mem);

    uint64_t range_to_test = 128 * 1024;

    if (frontend_address.get_raw() < 0x100000000 &&
        0x100000000 - range_to_test < frontend_address.get_raw())
        range_to_test = uint64_t(Address(0x100000000) - frontend_address);

    for (uint64_t o = 0; o < range_to_test; o += 4) {
        bus.write_u32(frontend_address + o, uint32_t(o));
    }

    for (uint64_t o = 0; o < range_to_test; o += 4) {
        QCOMPARE(bus.read_u32(frontend_address + o), uint32_t(o));
        bus.write_u32(frontend_address + o, uint32_t(o) ^ 0xaabbccdd);
    }

    for (uint64_t o = 0; o < range_to_test; o += 4) {
        QCOMPARE(bus.read_u32(frontend_address + o), uint32_t(o) ^ 0xaabbccdd);
    }
}

QTEST_APPLESS_MAIN(TestMemory)
