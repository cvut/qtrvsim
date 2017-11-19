#include "tst_machine.h"
#include "memory.h"

void MachineTests::memory_data() {
    QTest::addColumn<std::uint32_t>("address");

    QTest::newRow("memory begin") << (std::uint32_t)0x00;
    QTest::newRow("memory end") << (std::uint32_t)0xFFFFFFFC;
    QTest::newRow("memory midle start") << (std::uint32_t)0xFFFF00;
    QTest::newRow("memory midle end") << (std::uint32_t)0xFFFFFF;
}

void MachineTests::memory() {
    Memory m;

    QFETCH(std::uint32_t, address);

    // Uninitialize memory should read as zero
    QCOMPARE(m.read_byte(address), (std::uint8_t)0);
    QCOMPARE(m.read_hword(address), (std::uint16_t)0);
    QCOMPARE(m.read_word(address), (std::uint32_t)0);
    // Just a byte
    m.write_byte(address, 0x42);
    QCOMPARE(m.read_byte(address), (std::uint8_t)0x42);
    // Half word
    m.write_hword(address, 0x4243);
    QCOMPARE(m.read_hword(address), (std::uint16_t)0x4243);
    // Word
    m.write_word(address, 0x42434445);
    QCOMPARE(m.read_word(address), (std::uint32_t)0x42434445);
}

void MachineTests::memory_section_data() {
    QTest::addColumn<std::uint32_t>("address");

    QTest::newRow("memory begin") << (std::uint32_t)0x00;
    QTest::newRow("memory end") << (std::uint32_t)0xFFFFFFFF;
    QTest::newRow("memory midle start") << (std::uint32_t)0xFFFF00;
    QTest::newRow("memory midle end") << (std::uint32_t)0xFFFFFF;
}

void MachineTests::memory_section() {
    Memory m;

    QFETCH(std::uint32_t, address);

    // First section shouldn't exists
    QCOMPARE(m.get_section(address, false), (MemorySection*)nullptr);
    // Create section
    MemorySection *s = m.get_section(address, true);
    QVERIFY(s != nullptr);

    // Write some data to memory
    m.write_byte(address, 0x42);
    // Read it trough section (mask bits outside of the memory section)
    QCOMPARE(s->read_byte(address & ((1 << MEMORY_SECTION_BITS)  - 1)), (std::uint8_t)0x42);
    // Write some other data trough section
    s->write_byte(address & ((1 << MEMORY_SECTION_BITS)  - 1), 0x66);
    // Read trough memory
    QCOMPARE(m.read_byte(address), (std::uint8_t)0x66);
}

void MachineTests::memory_endian() {
    Memory m;

    // Memory should be bit endian so write bytes from most significant byte
    m.write_byte(0x00, 0x12);
    m.write_byte(0x01, 0x34);
    m.write_byte(0x02, 0x56);
    m.write_byte(0x03, 0x78);
    QCOMPARE(m.read_hword(0x00), (std::uint16_t)0x1234);
    QCOMPARE(m.read_word(0x00), (std::uint32_t)0x12345678);

    m.write_hword(0x80, 0x1234);
    QCOMPARE(m.read_byte(0x80), (std::uint8_t)0x12);
    QCOMPARE(m.read_byte(0x81), (std::uint8_t)0x34);

    m.write_word(0xF0, 0x12345678);
    QCOMPARE(m.read_byte(0xF0), (std::uint8_t)0x12);
    QCOMPARE(m.read_byte(0xF1), (std::uint8_t)0x34);
    QCOMPARE(m.read_byte(0xF2), (std::uint8_t)0x56);
    QCOMPARE(m.read_byte(0xF3), (std::uint8_t)0x78);
}
