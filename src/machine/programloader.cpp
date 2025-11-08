#include "programloader.h"

#include "common/endian.h"
#include "common/logging.h"
#include "simulator_exception.h"

#include <elf++.hh>
#include <cerrno>
#include <cstring>
#include <exception>
#include <fstream>

LOG_CATEGORY("machine.ProgramLoader");

// TODO - document
#ifndef O_BINARY
    #define O_BINARY 0
#endif

using namespace machine;

// Custom loader class that reads from QFile
class QFileLoader : public elf::loader {
public:
    explicit QFileLoader(QFile *file) : file(file) {}
    
    const void *load(off_t offset, size_t size) override {
        if (!file->seek(offset)) {
            throw std::runtime_error("Failed to seek in ELF file");
        }
        
        // Allocate a buffer that will persist
        char *buf = new char[size];
        buffers.emplace_back(buf);
        
        qint64 bytes_read = file->read(buf, size);
        if (bytes_read != static_cast<qint64>(size)) {
            throw std::runtime_error("Failed to read from ELF file");
        }
        
        return buf;
    }
    
    ~QFileLoader() override {
        for (char *buf : buffers) {
            delete[] buf;
        }
    }
    
private:
    QFile *file;
    std::vector<char*> buffers;
};

ProgramLoader::ProgramLoader(const QString &file) : elf_file(file) {
    // Open source file
    if (!elf_file.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        throw SIMULATOR_EXCEPTION(
            Input,
            QString("Can't open input elf file for reading (") + QString(file) + QString(")"),
            std::strerror(errno));
    }

    try {
        // Create loader and parse the ELF file
        auto loader = std::make_shared<QFileLoader>(&elf_file);
        elf_parser = std::make_unique<elf::elf>(loader);
        
        const auto &hdr = elf_parser->get_hdr();
        
        // Get entry point
        entry_point = hdr.entry;
        
        // Check elf file format, executable expected
        if (hdr.type != elf::et::exec) {
            throw SIMULATOR_EXCEPTION(Input, "Invalid input file type, executable expected", "");
        }
        
        // Check elf file architecture - RISC-V (EM_RISCV = 243)
        constexpr uint16_t EM_RISCV = 243;
        if (hdr.machine != EM_RISCV) {
            throw SIMULATOR_EXCEPTION(Input, "Invalid input file architecture, RISC-V expected", "");
        }
        
        // Check elf class (32-bit or 64-bit)
        if (hdr.ei_class == elf::elfclass::_32) {
            LOG("Loaded executable: 32bit");
            architecture_type = ARCH32;
            is_64bit = false;
        } else if (hdr.ei_class == elf::elfclass::_64) {
            LOG("Loaded executable: 64bit");
            architecture_type = ARCH64;
            is_64bit = true;
            WARN("64bit simulation is not fully supported.");
        } else {
            throw SIMULATOR_EXCEPTION(
                Input,
                "Unsupported architecture type. "
                "This simulator only supports 32bit and 64bit CPUs.",
                "");
        }
        
        // Get endianness
        if (hdr.ei_data == elf::elfdata::lsb) {
            endianness = LITTLE;
        } else if (hdr.ei_data == elf::elfdata::msb) {
            endianness = BIG;
        } else {
            throw SIMULATOR_EXCEPTION(
                Input,
                "ELF header malformed. Unknown endianness.",
                "");
        }
        
        // Count number of program headers (segments)
        n_secs = 0;
        for (const auto &seg : elf_parser->segments()) {
            (void)seg; // unused, just counting
            n_secs++;
        }
        
    } catch (const std::exception &e) {
        throw SIMULATOR_EXCEPTION(Input, "Failed to parse ELF file", e.what());
    }
}

ProgramLoader::ProgramLoader(const char *file) : ProgramLoader(QString::fromLocal8Bit(file)) {}

ProgramLoader::~ProgramLoader() {
    // Close file
    elf_file.close();
}

void ProgramLoader::to_memory(Memory *mem) {
    // Load program to memory (just dump it byte by byte)
    for (const auto &seg : elf_parser->segments()) {
        // Only load PT_LOAD segments
        if (seg.get_hdr().type != elf::pt::load) {
            continue;
        }
        
        const auto &phdr = seg.get_hdr();
        uint64_t base_address = phdr.vaddr;
        const void *data_ptr = seg.data();
        const char *segment_data = static_cast<const char*>(data_ptr);
        
        // Load the file size bytes (not memory size - the rest is zero-initialized)
        for (uint64_t i = 0; i < phdr.filesz; i++) {
            const auto buffer = static_cast<uint8_t>(segment_data[i]);
            memory_write_u8(mem, base_address + i, buffer);
        }
    }
}

Address ProgramLoader::end() {
    uint64_t last = 0;
    
    // Go through all LOAD segments and find the last one
    for (const auto &seg : elf_parser->segments()) {
        if (seg.get_hdr().type != elf::pt::load) {
            continue;
        }
        
        const auto &phdr = seg.get_hdr();
        uint64_t seg_end = phdr.vaddr + phdr.filesz;
        if (seg_end > last) {
            last = seg_end;
        }
    }
    
    return Address(last + 0x10); // We add offset so we are sure that also
                                 // pipeline is empty TODO propagate address deeper
}

Address ProgramLoader::get_executable_entry() const {
    return Address(entry_point);
}

SymbolTable *ProgramLoader::get_symbol_table() {
    auto *p_st = new SymbolTable();
    
    try {
        // Iterate through all sections to find symbol tables
        for (const auto &sec : elf_parser->sections()) {
            const auto &shdr = sec.get_hdr();
            
            // Check if this is a symbol table section
            if (shdr.type == elf::sht::symtab || shdr.type == elf::sht::dynsym) {
                // Get the symtab
                try {
                    elf::symtab symtab = sec.as_symtab();
                    
                    // Iterate through symbols
                    auto it_begin = symtab.begin();
                    auto it_end = symtab.end();
                    for (auto it = it_begin; it != it_end; ) {
                        const elf::sym &symbol = *it;
                        const elf::Sym<> &sym_data = symbol.get_data();
                        
                        // Add symbol to table
                        p_st->add_symbol(
                            symbol.get_name().c_str(),
                            sym_data.value,
                            sym_data.size,
                            sym_data.info,
                            sym_data.other);
                        
                        ++it;
                    }
                    
                    // We found a symbol table, that's enough
                    break;
                } catch (const elf::section_type_mismatch &) {
                    // Not a valid symbol table, continue
                    continue;
                }
            }
        }
    } catch (const std::exception &e) {
        // If we fail to parse symbols, just return empty table
        LOG("Failed to parse symbol table: %s", e.what());
    }
    
    return p_st;
}
Endian ProgramLoader::get_endian() const {
    return endianness;
}
ArchitectureType ProgramLoader::get_architecture_type() const {
    return architecture_type;
}
