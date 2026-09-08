#include "programloader.h"

#include "common/endian.h"
#include "common/logging.h"
#include "simulator_exception.h"

#include <dwarf/dwarf++.hh>
#include <exception>
#include <map>
#include <stdexcept>
#include <sys/types.h>

// This is a workaround to ignore libelfin ref-counting cycle.
#ifdef __SANITIZE_ADDRESS__
    #include <sanitizer/lsan_interface.h>
#endif

LOG_CATEGORY("machine.ProgramLoader");

using namespace machine;

constexpr int EM_RISCV = 243;

class MemLoader : public elf::loader {
public:
    MemLoader(const QString &fname) : file(fname), mapped(nullptr), size(0) {
        if (!file.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
            throw SIMULATOR_EXCEPTION(
                Input, QString("Can't open input elf file for reading (") + fname + QString(")"),
                file.errorString());
        }
        size = file.size();
        mapped = file.map(0, size);
        if (mapped == nullptr) {
            throw SIMULATOR_EXCEPTION(
                Input, QString("Can't mmap input elf file (") + fname + QString(")"),
                file.errorString());
        }
    }

    ~MemLoader() override { close(); }

    void close() {
        if (mapped != nullptr) {
            file.unmap(mapped);
            mapped = nullptr;
        }
        if (file.isOpen()) { file.close(); }
    }

    const void *load(off_t offset, size_t len) override {
        if ((size_t)offset + len > (size_t)size) {
            throw SANITY_EXCEPTION("ELF loader requested offset exceeds file size");
        }
        return mapped + offset;
    }

private:
    QFile file;
    unsigned char *mapped;
    std::int64_t size;
};

ProgramLoader::ProgramLoader(const QString &file) {
    try {
#ifdef __SANITIZE_ADDRESS__
        __lsan_disable();
#endif
        elf_file = elf::elf(std::make_shared<MemLoader>(file));
#ifdef __SANITIZE_ADDRESS__
        __lsan_enable();
#endif
    } catch (const std::exception &e) {
#ifdef __SANITIZE_ADDRESS__
        __lsan_enable();
#endif
        throw SIMULATOR_EXCEPTION(Input, "Elf library initialization failed", e.what());
    }

    const auto &hdr = elf_file.get_hdr();
    executable_entry = Address(hdr.entry);

    if (hdr.type != elf::et::exec) {
        throw SIMULATOR_EXCEPTION(Input, "Invalid input file type", "");
    }

    if (hdr.machine != EM_RISCV) {
        throw SIMULATOR_EXCEPTION(Input, "Invalid input file architecture", "");
    }

    if (hdr.ei_class == elf::elfclass::_32) {
        LOG("Loaded executable: 32bit");
        architecture_type = ARCH32;
    } else if (hdr.ei_class == elf::elfclass::_64) {
        LOG("Loaded executable: 64bit");
        architecture_type = ARCH64;
        WARN("64bit simulation is not fully supported.");
    } else {
        throw SIMULATOR_EXCEPTION(
            Input,
            "Unsupported architecture type."
            "This simulator only supports 32bit and 64bit CPUs.",
            "");
    }

    for (const auto &seg : elf_file.segments()) {
        if (seg.get_hdr().type == elf::pt::load) { load_segments.push_back(seg); }
    }
}

ProgramLoader::ProgramLoader(const char *file) : ProgramLoader(QString::fromLocal8Bit(file)) {}

ProgramLoader::~ProgramLoader() {
    // This is a fix for upstream issue where libelf creates a ref-counting cycle.
    auto loader = elf_file.get_loader();
    if (loader) {
        auto mem_loader = std::dynamic_pointer_cast<MemLoader>(loader);
        if (mem_loader) { mem_loader->close(); }
    }
}

void ProgramLoader::to_memory(Memory *mem) {
    for (const auto &seg : load_segments) {
        uint64_t base_address = seg.get_hdr().vaddr;
        const char *data = (const char *)seg.data();
        size_t filesz = seg.get_hdr().filesz;
        for (size_t i = 0; i < filesz; i++) {
            memory_write_u8(mem, base_address + i, (uint8_t)data[i]);
        }
    }
}

Address ProgramLoader::end() {
    uint64_t last = 0;
    for (const auto &seg : load_segments) {
        uint64_t end_addr = seg.get_hdr().vaddr + seg.get_hdr().filesz;
        if (end_addr > last) { last = end_addr; }
    }
    return Address(last + 0x10); // We add offset so we are sure that also
                                 // pipeline is empty TODO propagate address
                                 // deeper
}

Address ProgramLoader::get_executable_entry() const {
    return executable_entry;
}

SymbolTable *ProgramLoader::get_symbol_table() {
    auto *p_st = new SymbolTable();
    for (const auto &sec : elf_file.sections()) {
        if (sec.get_hdr().type == elf::sht::symtab) {
            for (const auto &sym : sec.as_symtab()) {
                const auto &d = sym.get_data();
                p_st->add_symbol(sym.get_name().c_str(), d.value, d.size, d.info, d.other);
            }
        }
    }
    return p_st;
}

Endian ProgramLoader::get_endian() const {
    if (elf_file.get_hdr().ei_data == elf::elfdata::lsb) {
        return LITTLE;
    } else if (elf_file.get_hdr().ei_data == elf::elfdata::msb) {
        return BIG;
    } else {
        throw SIMULATOR_EXCEPTION(
            Input,
            "ELF header e_ident malformed."
            "Unknown value of the byte EI_DATA."
            "Expected value little (=1) or big (=2).",
            "");
    }
}

ArchitectureType ProgramLoader::get_architecture_type() const {
    return architecture_type;
}

void ProgramLoader::load_debug_info(debuginfo::DebugInfo &debug_info) {
#ifdef __SANITIZE_ADDRESS__
    // There is a ref-counting cycle in libelfin that causes memory leaks.
    __lsan_disable();
#endif
    try {
        dwarf::dwarf dwarf(dwarf::elf::create_loader(elf_file));

        for (auto &cu : dwarf.compilation_units()) {
            // Map from DWARF file index to debuginfo::FileId for the current CU
            std::map<unsigned, debuginfo::FileId> file_map;

            for (auto &entry : cu.get_line_table()) {
                if (entry.file && !entry.file->path.empty()) {
                    debuginfo::FileId file_id;
                    auto it = file_map.find(entry.file_index);
                    if (it != file_map.end()) {
                        file_id = it->second;
                    } else {
                        file_id = debug_info.get_file_id(entry.file->path);
                        file_map[entry.file_index] = file_id;
                    }
                    debug_info.add_line(entry.address, file_id, entry.line);
                }
            }
        }

        debug_info.finalize();
    } catch (const std::exception &e) {
        // It is not critical if we fail to load debug info
        WARN("Failed to load debug info: %s", e.what());
    }
#ifdef __SANITIZE_ADDRESS__
    __lsan_enable();
#endif
}
