#include "programloader.test.h"

#include "machine/instruction.h"
#include "machine/memory/memory_utils.h"
#include "machine/programloader.h"
#include "memory/backend/memory.h"

#include <QTemporaryFile>
#include <QtEndian>
#include <array>
#include <cstring>

using namespace machine;

namespace {

template<typename T>
QByteArray object_bytes(const T &object) {
    return QByteArray(reinterpret_cast<const char *>(&object), sizeof(object));
}

void append_u32(QByteArray &bytes, quint32 value) {
    bytes += object_bytes(qToLittleEndian(value));
}

template<typename T>
void append_dwarf_value(QByteArray &bytes, T value, Endian endian) {
    bytes += object_bytes(endian == LITTLE ? qToLittleEndian(value) : qToBigEndian(value));
}

QByteArray dwarf_unit(const QByteArray &body, bool dwarf64 = false, Endian endian = LITTLE) {
    QByteArray bytes;
    if (dwarf64) {
        append_dwarf_value(bytes, quint32(0xffffffff), endian);
        append_dwarf_value(bytes, quint64(body.size()), endian);
    } else {
        append_dwarf_value(bytes, quint32(body.size()), endian);
    }
    return bytes + body;
}

QByteArray dwarf4_info() {
    QByteArray body = QByteArray::fromHex("0400000000000401");
    body += "source.c";
    body += '\0';
    append_u32(body, 0); // DW_AT_stmt_list
    return dwarf_unit(body);
}

QByteArray dwarf5_info(quint8 type, bool dwarf64, Endian endian) {
    QByteArray body;
    append_dwarf_value(body, quint16(5), endian);
    body += char(type);
    body += '\x04';                            // Address size.
    body += QByteArray(dwarf64 ? 8 : 4, '\0'); // Abbreviation offset.
    if (type == 2 || type == 6) {
        append_dwarf_value(body, quint64(0x123456789abcdef0), endian); // Type signature.
        body += QByteArray(dwarf64 ? 8 : 4, '\0');                     // Type offset.
    } else if (type == 4 || type == 5) {
        append_dwarf_value(body, quint64(0x123456789abcdef0), endian); // DWO ID.
    }
    body += '\x01';
    body += "source.c";
    body += '\0';
    body += QByteArray(dwarf64 ? 8 : 4, '\0'); // DW_AT_stmt_list
    return dwarf_unit(body, dwarf64, endian);
}

QByteArray debug_elf(
    bool malformed = false,
    const QByteArray &info_override = {},
    const QByteArray &line_override = {},
    Endian endian = LITTLE) {
    // Two DWARF4 sequences, descending address order.
    QByteArray line_header = QByteArray::fromHex("010101fb0e0d000101010100000001000001");
    line_header += '\0'; // No include directories.
    line_header += "source.c";
    line_header += QByteArray(5, '\0'); // File terminator, metadata and end of file table.
    QByteArray line_body = QByteArray::fromHex("0400");
    append_u32(line_body, static_cast<quint32>(line_header.size()));
    line_body += line_header;
    for (quint32 address : { 0x200u, 0x100u }) {
        line_body += QByteArray::fromHex("000502"); // DW_LNE_set_address
        append_u32(line_body, address);
        line_body += '\x03'; // DW_LNS_advance_line
        line_body += address == 0x200 ? '\x13' : '\x09';
        line_body += QByteArray::fromHex("010204000101"); // Copy, advance PC, end sequence.
    }
    if (malformed) {
        // Truncate the last opcode.
        line_body[line_body.size() - 1] = '\x02';
    }

    const QByteArray abbrev = QByteArray::fromHex("01110003081017000000");

    const std::array<QByteArray, 4> names { ".shstrtab", ".debug_info", ".debug_abbrev",
                                            ".debug_line" };
    QByteArray strings(1, '\0');
    std::array<quint32, 4> name_offsets {};
    for (size_t i = 0; i < names.size(); ++i) {
        name_offsets[i] = static_cast<quint32>(strings.size());
        strings += names[i];
        strings += '\0';
    }
    const std::array<QByteArray, 4> sections {
        strings, info_override.isNull() ? dwarf4_info() : info_override, abbrev,
        line_override.isNull() ? dwarf_unit(line_body) : line_override
    };

    elf::Ehdr<elf::Elf32> header {};
    std::memcpy(header.ei_magic, "\177ELF", 4);
    header.ei_class = elf::elfclass::_32;
    header.ei_data = endian == LITTLE ? elf::elfdata::lsb : elf::elfdata::msb;
    header.ei_version = header.version = 1;
    header.type = elf::et::exec;
    header.machine = 243; // EM_RISCV
    header.ehsize = sizeof(header);
    header.phentsize = sizeof(elf::Phdr<elf::Elf32>);
    header.shentsize = sizeof(elf::Shdr<elf::Elf32>);
    header.shnum = sections.size() + 1;
    header.shstrndx = 1;

    QByteArray bytes(sizeof(header), '\0');
    std::array<elf::Shdr<elf::Elf32>, 5> section_headers {};
    for (size_t i = 0; i < sections.size(); ++i) {
        auto &section = section_headers[i + 1];
        section.name = name_offsets[i];
        section.type = i == 0 ? elf::sht::strtab : elf::sht::progbits;
        section.offset = static_cast<quint32>(bytes.size());
        section.size = static_cast<quint32>(sections[i].size());
        section.addralign = 1;
        bytes += sections[i];
    }
    bytes += QByteArray((4 - bytes.size() % 4) % 4, '\0');
    header.shoff = static_cast<quint32>(bytes.size());
    for (const auto &section : section_headers) {
        if (endian == LITTLE) {
            elf::Shdr<elf::Elf32, elf::byte_order::lsb> encoded {};
            encoded.from(section);
            bytes += object_bytes(encoded);
        } else {
            elf::Shdr<elf::Elf32, elf::byte_order::msb> encoded {};
            encoded.from(section);
            bytes += object_bytes(encoded);
        }
    }
    if (endian == LITTLE) {
        elf::Ehdr<elf::Elf32, elf::byte_order::lsb> encoded {};
        encoded.from(header);
        bytes.replace(0, sizeof(header), object_bytes(encoded));
    } else {
        elf::Ehdr<elf::Elf32, elf::byte_order::msb> encoded {};
        encoded.from(header);
        bytes.replace(0, sizeof(header), object_bytes(encoded));
    }
    return bytes;
}

} // namespace

// This is common program start (initial value of program counter)
#define PC_INIT 0x00000200

const char *EXECUTABLE_NAME = "data";

void TestProgramLoader::program_loader() {
    if (not QFile::exists(EXECUTABLE_NAME)) {
        QSKIP("Executable is not present, cannot test program loader.");
    }

    ProgramLoader pl(EXECUTABLE_NAME);
    Memory m(BIG);
    pl.to_memory(&m);

    // 	addi $1, $0, 6
    //    QCOMPARE(Instruction(memory_read_u32(&m, PC_INIT)), Instruction(8, 0, 1, 6));
    // j (8)0020000 (only 28 bits are used and they are logically shifted left
    // by 2)
    //    QCOMPARE(Instruction(memory_read_u32(&m, PC_INIT + 4)), Instruction(2, Address(0x20000 >>
    //    2)));
    // TODO add some more code to data and do more compares (for example more
    // sections)
}

void TestProgramLoader::debug_info_sequences() {
    QTemporaryFile file;
    QVERIFY(file.open());
    auto bytes = debug_elf();
    QCOMPARE(file.write(bytes), qint64(bytes.size()));
    QVERIFY(file.flush());

    ProgramLoader loader(file.fileName());
    debuginfo::DebugInfo info;
    QVERIFY(loader.load_debug_info(info).isEmpty());
    size_t hint = 0;
    for (uint64_t address : { 0x100, 0x103, 0x200, 0x203 }) {
        const auto *loc = info.find(address, &hint);
        QVERIFY(loc != nullptr);
        QCOMPARE(info.get_file_path(loc->file_id), std::string("source.c"));
        QCOMPARE(loc->line, address < 0x200 ? 10u : 20u);
    }
    for (uint64_t address : { 0xff, 0x104, 0x180, 0x204, 0x300 }) {
        QVERIFY(info.find(address) == nullptr);
        QVERIFY(info.find(address, &hint) == nullptr);
    }
}

void TestProgramLoader::debug_info_load_failure() {
    QTemporaryFile file;
    QVERIFY(file.open());
    auto bytes = debug_elf(true);
    QCOMPARE(file.write(bytes), qint64(bytes.size()));
    QVERIFY(file.flush());

    ProgramLoader loader(file.fileName());
    debuginfo::DebugInfo info;
    info.add_line(0x50, info.get_file_id("previous.c"), 1);
    info.finalize();
    QVERIFY(loader.load_debug_info(info).isEmpty());
    for (uint64_t address : { 0x50, 0x100, 0x200, 0x300 }) {
        QVERIFY(info.find(address) == nullptr);
    }
}

void TestProgramLoader::debug_info_dwarf5_data() {
    QTest::addColumn<QByteArray>("info_section");
    QTest::addColumn<QByteArray>("line_section");
    QTest::addColumn<bool>("big_endian");
    for (bool big : { false, true }) {
        for (bool dwarf64 : { false, true }) {
            for (quint8 type : { 1, 2, 3, 4, 5, 6 }) {
                const auto name = QString("%1-dwarf%2-type%3")
                                      .arg(big ? "big" : "little")
                                      .arg(dwarf64 ? 64 : 32)
                                      .arg(type);
                QTest::newRow(qPrintable(name))
                    << dwarf5_info(type, dwarf64, big ? BIG : LITTLE) << QByteArray() << big;
            }
        }
    }
    QTest::newRow("dwarf5-after-dwarf4")
        << (dwarf4_info() + dwarf5_info(1, false, LITTLE)) << QByteArray() << false;
    QTest::newRow("dwarf5-after-padding")
        << (QByteArray(4, '\0') + dwarf5_info(1, false, LITTLE)) << QByteArray() << false;
    QTest::newRow("dwarf4-cu-dwarf5-lines")
        << QByteArray() << dwarf_unit(QByteArray::fromHex("0500")) << false;
}

void TestProgramLoader::debug_info_dwarf5() {
    QFETCH(QByteArray, info_section);
    QFETCH(QByteArray, line_section);
    QFETCH(bool, big_endian);
    QTemporaryFile file;
    QVERIFY(file.open());
    const auto bytes = debug_elf(false, info_section, line_section, big_endian ? BIG : LITTLE);
    QCOMPARE(file.write(bytes), qint64(bytes.size()));
    QVERIFY(file.flush());

    ProgramLoader loader(file.fileName());
    debuginfo::DebugInfo info;
    info.add_line(0x50, info.get_file_id("previous.c"), 1);
    info.finalize();
    const auto warning = loader.load_debug_info(info);
    QVERIFY(warning.contains("DWARF 5"));
    QVERIFY(warning.contains("not supported"));
    QVERIFY(warning.contains("-gdwarf-4"));
    QVERIFY(warning.contains("can still run"));
    for (uint64_t address : { 0x50, 0x100, 0x200, 0x300 }) {
        QVERIFY(info.find(address) == nullptr);
    }
}

void TestProgramLoader::debug_info_header_failure_data() {
    QTest::addColumn<QByteArray>("info_section");
    QTest::newRow("truncated-length") << QByteArray::fromHex("010000");
    QTest::newRow("truncated-dwarf64-length") << QByteArray::fromHex("ffffffff01000000");
    QTest::newRow("reserved-length") << QByteArray::fromHex("f0ffffff");
    QTest::newRow("short-unit") << QByteArray::fromHex("0100000004");
    QTest::newRow("long-unit") << QByteArray::fromHex("100000000400");
}

void TestProgramLoader::debug_info_header_failure() {
    QFETCH(QByteArray, info_section);
    QTemporaryFile file;
    QVERIFY(file.open());
    const auto bytes = debug_elf(false, info_section);
    QCOMPARE(file.write(bytes), qint64(bytes.size()));
    QVERIFY(file.flush());
    ProgramLoader loader(file.fileName());
    debuginfo::DebugInfo info;
    info.add_line(0x50, info.get_file_id("previous.c"), 1);
    info.finalize();
    QVERIFY(loader.load_debug_info(info).isEmpty());
    QVERIFY(info.find(0x50) == nullptr);
    QVERIFY(info.find(0x200) == nullptr);
}

QTEST_APPLESS_MAIN(TestProgramLoader)