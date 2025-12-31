#include "machine.test.h"

#include "machine/machine.h"
#include "machine/machineconfig.h"
#include "machine/memory/memory_utils.h"

#include <QTemporaryFile>
#include <QtEndian>
#include <array>
#include <cstring>
#include <elf/elf++.hh>

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

void TestMachine::restart_restores_elf_debug_info() {
    QTemporaryFile file;
    QVERIFY(file.open());
    const auto bytes = debug_elf();
    QCOMPARE(file.write(bytes), qint64(bytes.size()));
    QVERIFY(file.flush());

    MachineConfig config;
    config.set_elf(file.fileName());
    Machine machine(config, false, true);

    QVERIFY(machine.executable_loaded());
    auto *debug_info = machine.get_debug_info();
    QVERIFY(debug_info != nullptr);

    const auto *loc = debug_info->find(0x200);
    QVERIFY(loc != nullptr);
    QCOMPARE(loc->line, 20u);
    QCOMPARE(QString::fromStdString(debug_info->get_file_path(loc->file_id)), QString("source.c"));

    debug_info->clear();
    const auto custom_file_id = debug_info->get_file_id("custom.s");
    debug_info->add_line(0x300, custom_file_id, 42);
    debug_info->add_end_sequence(0x304);
    debug_info->finalize();

    QVERIFY(debug_info->find(0x200) == nullptr);
    const auto *custom_loc = debug_info->find(0x300);
    QVERIFY(custom_loc != nullptr);
    QCOMPARE(custom_loc->line, 42u);

    machine.restart();

    const auto *restored_loc = machine.get_debug_info()->find(0x200);
    QVERIFY(restored_loc != nullptr);
    QCOMPARE(restored_loc->line, 20u);
    QCOMPARE(
        QString::fromStdString(machine.get_debug_info()->get_file_path(restored_loc->file_id)),
        QString("source.c"));
    QVERIFY(machine.get_debug_info()->find(0x300) == nullptr);
}

void TestMachine::restart_retains_internal_debug_info() {
    MachineConfig config;
    Machine machine(config, false, false);

    QVERIFY(!machine.executable_loaded());
    auto *debug_info = machine.get_debug_info();
    QVERIFY(debug_info != nullptr);

    const auto file_id = debug_info->get_file_id("internal.s");
    debug_info->add_line(0x200, file_id, 15);
    debug_info->add_end_sequence(0x204);
    debug_info->finalize();

    const auto *loc = debug_info->find(0x200);
    QVERIFY(loc != nullptr);
    QCOMPARE(loc->line, 15u);

    machine.restart();

    const auto *after_restart = machine.get_debug_info()->find(0x200);
    QVERIFY(after_restart != nullptr);
    QCOMPARE(after_restart->line, 15u);
}

void TestMachine::keep_memory_preserves_debug_info() {
    MachineConfig config;
    Machine machine1(config, false, false);

    auto *debug_info1 = machine1.get_debug_info();
    QVERIFY(debug_info1 != nullptr);
    const auto file_id = debug_info1->get_file_id("app.s");
    debug_info1->add_line(0x200, file_id, 99);
    debug_info1->add_end_sequence(0x204);
    debug_info1->finalize();

    memory_write_u32(machine1.memory_rw(), 0x200u, 0x00100093u); // addi x1, x0, 1

    Machine machine2(config, false, false);
    machine2.memory_rw()->reset(*machine1.memory());
    if (machine1.get_debug_info() != nullptr && machine2.get_debug_info() != nullptr) {
        *machine2.get_debug_info() = *machine1.get_debug_info();
    }

    QCOMPARE(memory_read_u32(machine2.memory(), 0x200u), 0x00100093u);
    const auto *loc = machine2.get_debug_info()->find(0x200);
    QVERIFY(loc != nullptr);
    QCOMPARE(loc->line, 99u);
    QCOMPARE(
        QString::fromStdString(machine2.get_debug_info()->get_file_path(loc->file_id)),
        QString("app.s"));

    machine2.restart();
    const auto *after_restart = machine2.get_debug_info()->find(0x200);
    QVERIFY(after_restart != nullptr);
    QCOMPARE(after_restart->line, 99u);
}

QTEST_MAIN(TestMachine)
