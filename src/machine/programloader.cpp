#include "programloader.h"

#include "common/endian.h"
#include "common/logging.h"
#include "simulator_exception.h"

#include <cerrno>
#include <cstring>
#include <exception>

LOG_CATEGORY("machine.ProgramLoader");

// TODO - document
#ifndef O_BINARY
    #define O_BINARY 0
#endif

using namespace machine;

ProgramLoader::ProgramLoader(const QString &file) : elf_file(file) {
    const GElf_Ehdr *elf_ehdr;
    // Initialize elf library
    if (elf_version(EV_CURRENT) == EV_NONE) {
        throw SIMULATOR_EXCEPTION(
            Input, "Elf library initialization failed", elf_errmsg(-1));
    }
    // Open source file - option QIODevice::ExistingOnly cannot be used on Qt
    // <5.11
    if (!elf_file.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        throw SIMULATOR_EXCEPTION(
            Input,
            QString("Can't open input elf file for reading (") + QString(file)
                + QString(")"),
            std::strerror(errno));
    }
    // Initialize elf
    if (!(this->elf = elf_begin(elf_file.handle(), ELF_C_READ, nullptr))) {
        throw SIMULATOR_EXCEPTION(
            Input, "Elf read begin failed", elf_errmsg(-1));
    }
    // Check elf kind
    if (elf_kind(this->elf) != ELF_K_ELF) {
        throw SIMULATOR_EXCEPTION(
            Input, "Invalid input file elf format, plain elf file expected",
            "");
    }

    elf_ehdr = gelf_getehdr(this->elf, &this->hdr);
    if (!elf_ehdr) {
        throw SIMULATOR_EXCEPTION(
            Input, "Getting elf file header failed", elf_errmsg(-1));
    }

    executable_entry = Address(elf_ehdr->e_entry);
    // Check elf file format, executable expected, nothing else.
    if (this->hdr.e_type != ET_EXEC) {
        throw SIMULATOR_EXCEPTION(Input, "Invalid input file type", "");
    }
    // Check elf file architecture, of course only mips is supported.
    // Note: This also checks that this is big endian as EM_MIPS is suppose to
    // be: MIPS R3000 big-endian
    if (this->hdr.e_machine != EM_RISCV) {
        throw SIMULATOR_EXCEPTION(Input, "Invalid input file architecture", "");
    }
    // Check elf file class, only 32bit architecture is supported.
    int elf_class;
    if ((elf_class = gelf_getclass(this->elf)) == ELFCLASSNONE) {
        throw SIMULATOR_EXCEPTION(
            Input, "Getting elf class failed", elf_errmsg(-1));
    }
    // Get number of program sections in elf file
    if (elf_getphdrnum(this->elf, &this->n_secs)) {
        throw SIMULATOR_EXCEPTION(
            Input, "Elf program sections count query failed", elf_errmsg(-1));
    }

    if (elf_class == ELFCLASS32) {
        LOG("Loaded executable: 32bit");
        architecture_type = ARCH32;
        // Get program sections headers
        if (!(sections_headers.arch32 = elf32_getphdr(elf))) {
            throw SIMULATOR_EXCEPTION(Input, "Elf program sections get failed", elf_errmsg(-1));
        }
        // We want only LOAD sections so we create load_sections_indexes of those sections
        for (unsigned i = 0; i < n_secs; i++) {
            if (sections_headers.arch32[i].p_type != PT_LOAD) { continue; }
            indexes_of_load_sections.push_back(i);
        }
    } else if (elf_class == ELFCLASS64) {
        LOG("Loaded executable: 64bit");
        architecture_type = ARCH64;
        WARN("64bit simulation is not fully supported.");
        // Get program sections headers
        if (!(sections_headers.arch64 = elf64_getphdr(elf))) {
            throw SIMULATOR_EXCEPTION(Input, "Elf program sections get failed", elf_errmsg(-1));
        }
        // We want only LOAD sections so we create load_sections_indexes of those sections
        for (unsigned i = 0; i < this->n_secs; i++) {
            if (sections_headers.arch64[i].p_type != PT_LOAD) { continue; }
            this->indexes_of_load_sections.push_back(i);
        }

    } else {
        WARN("Unsupported elf class: %d", elf_class);
        throw SIMULATOR_EXCEPTION(
            Input,
            "Unsupported architecture type."
            "This simulator only supports 32bit and 64bit CPUs.",
            "");
    }
}

ProgramLoader::ProgramLoader(const char *file)
    : ProgramLoader(QString::fromLocal8Bit(file)) {}

ProgramLoader::~ProgramLoader() {
    // Close elf
    elf_end(this->elf);
    // Close file
    elf_file.close();
}

void ProgramLoader::to_memory(Memory *mem) {
    // Load program to memory (just dump it byte by byte)
    if (architecture_type == ARCH32) {
        for (size_t phdrs_i : this->indexes_of_load_sections) {
            uint32_t base_address = this->sections_headers.arch32[phdrs_i].p_vaddr;
            char *f = elf_rawfile(this->elf, nullptr);
            for (unsigned y = 0; y < this->sections_headers.arch32[phdrs_i].p_filesz; y++) {
                const auto buffer = (uint8_t)f[this->sections_headers.arch32[phdrs_i].p_offset + y];
                memory_write_u8(mem, base_address + y, buffer);
            }
        }
    } else if (architecture_type == ARCH64) {
        for (size_t phdrs_i : this->indexes_of_load_sections) {
            uint32_t base_address = this->sections_headers.arch64[phdrs_i].p_vaddr;
            char *f = elf_rawfile(this->elf, nullptr);
            for (unsigned y = 0; y < this->sections_headers.arch64[phdrs_i].p_filesz; y++) {
                const auto buffer = (uint8_t)f[this->sections_headers.arch64[phdrs_i].p_offset + y];
                memory_write_u8(mem, base_address + y, buffer);
            }
        }
    }
}

Address ProgramLoader::end() {
    uint32_t last = 0;
    // Go trough all sections and found out last one
    if (architecture_type == ARCH32) {
        for (size_t i : this->indexes_of_load_sections) {
            Elf32_Phdr *phdr = &(this->sections_headers.arch32[i]);
            if ((phdr->p_vaddr + phdr->p_filesz) > last) { last = phdr->p_vaddr + phdr->p_filesz; }
        }
    } else if (architecture_type == ARCH64) {
        for (size_t i : this->indexes_of_load_sections) {
            Elf64_Phdr *phdr = &(this->sections_headers.arch64[i]);
            if ((phdr->p_vaddr + phdr->p_filesz) > last) { last = phdr->p_vaddr + phdr->p_filesz; }
        }
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
    Elf_Scn *scn = nullptr;
    GElf_Shdr shdr;
    Elf_Data *data;
    int count, ii;

    elf_version(EV_CURRENT);

    while (true) {
        if ((scn = elf_nextscn(this->elf, scn)) == nullptr) {
            return p_st;
        }
        gelf_getshdr(scn, &shdr);
        if (shdr.sh_type == SHT_SYMTAB) {
            /* found a symbol table, go print it. */
            break;
        }
    }

    data = elf_getdata(scn, nullptr);
    count = shdr.sh_size / shdr.sh_entsize;

    /* retrieve the symbol names */
    for (ii = 0; ii < count; ++ii) {
        GElf_Sym sym;
        gelf_getsym(data, ii, &sym);
        p_st->add_symbol(
            elf_strptr(elf, shdr.sh_link, sym.st_name), sym.st_value,
            sym.st_size, sym.st_info, sym.st_other);
    }

    return p_st;
}
Endian ProgramLoader::get_endian() const {
    // Reading elf endian_id_byte according to the ELF specs.
    unsigned char endian_id_byte = this->hdr.e_ident[EI_DATA];
    if (endian_id_byte == ELFDATA2LSB) {
        return LITTLE;
    } else if (endian_id_byte == ELFDATA2MSB) {
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
