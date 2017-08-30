#include "programloader.h"
#include <exception>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include <cstring>
#include "qtmipsexception.h"

ProgramLoader::ProgramLoader(char *file) {
    // Initialize elf library
    if (elf_version(EV_CURRENT) == EV_NONE)
        throw QTMIPS_EXCEPTION(Input, "Elf library initialization failed", elf_errmsg(-1));
    // Open source file
    if ((this->fd = open(file, O_RDONLY, 0)) < 0)
        throw QTMIPS_EXCEPTION(Input, "Can't open input elf file for reading", std::strerror(errno));
    // Initialize elf
    if (!(this->elf = elf_begin(this->fd, ELF_C_READ, NULL)))
        throw QTMIPS_EXCEPTION(Input, "Elf read begin failed", elf_errmsg(-1));
    // Check elf kind
    if (elf_kind(this->elf) != ELF_K_ELF)
        throw QTMIPS_EXCEPTION(Input, "Invalid input file elf format, plain elf file expected", "");

    if (!gelf_getehdr(this->elf, &this->hdr))
        throw QTMIPS_EXCEPTION(Input, "Getting elf file header failed", elf_errmsg(-1));
    // Check elf file format, executable expected, nothing else.
    if (this->hdr.e_type != ET_EXEC)
        throw QTMIPS_EXCEPTION(Input, "Invalid input file type", "");
    // Check elf file architecture, of course only mips is supported.
    if (this->hdr.e_machine != EM_MIPS)
        throw QTMIPS_EXCEPTION(Input, "Invalid input file architecture", "");
    // Check elf file class, only 32bit architecture is supported.
    int elf_class;
    if ((elf_class = gelf_getclass(this->elf)) == ELFCLASSNONE)
        throw QTMIPS_EXCEPTION(Input, "Getting elf class failed", elf_errmsg(-1));
    if (elf_class != ELFCLASS32)
        throw QTMIPS_EXCEPTION(Input, "Only supported architecture is 32bit", "");
    // TODO check endianity!

    // Get number of program sections in elf file
    if (elf_getphdrnum(this->elf, &this->n_secs))
        throw QTMIPS_EXCEPTION(Input, "Elf program sections count query failed", elf_errmsg(-1));
    // Get program sections headers
    if (!(this->phdrs = elf32_getphdr(this->elf)))
        throw QTMIPS_EXCEPTION(Input, "Elf program sections get failed", elf_errmsg(-1));
    // We want only LOAD sections so we create map of those sections
    for (unsigned i = 1; i < this->n_secs; i++) {
        // TODO handle endianity
        if (this->phdrs[i].p_type != PT_LOAD)
            continue;
        this->map.push_back(i);
    }
    // TODO instead of direct access should we be using sections and elf_data? And if so how to link program header and section?
}

ProgramLoader::~ProgramLoader() {
    // Close elf
    elf_end(this->elf);
    // Close file
    close(this->fd);
}

size_t ProgramLoader::get_nsec() {
    return this->map.size();
}

std::uint32_t ProgramLoader::get_address(size_t sec) {
    SANITY_ASSERT(sec > this->get_nsec(), "Requesting too big section");
    return this->phdrs[this->map[sec]].p_vaddr;
}

std::vector<std::uint8_t> ProgramLoader::get_data(size_t sec) {
    SANITY_ASSERT(sec > this->get_nsec(), "Requesting too big section");
    std::vector<std::uint8_t> d;
    char *f = elf_rawfile(this->elf, NULL);
    size_t phdrs_i = this->map[sec];
    for (unsigned i = 0; i < this->phdrs[phdrs_i].p_filesz; i++) {
        d.push_back((std::uint8_t) f[this->phdrs[phdrs_i].p_offset + i]);
    }
    return d;
}
