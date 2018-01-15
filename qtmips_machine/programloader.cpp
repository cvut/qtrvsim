#include "programloader.h"
#include <exception>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include "qtmipsexception.h"

using namespace machine;

ProgramLoader::ProgramLoader(const char *file) {
    // Initialize elf library
    if (elf_version(EV_CURRENT) == EV_NONE)
        throw QTMIPS_EXCEPTION(Input, "Elf library initialization failed", elf_errmsg(-1));
    // Open source file
    if ((this->fd = open(file, O_RDONLY, 0)) < 0)
        throw QTMIPS_EXCEPTION(Input, QString("Can't open input elf file for reading (") + QString(file) + QString(")"), std::strerror(errno));
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
    // TODO We should check in what endianity elf file is coded in

    // Get number of program sections in elf file
    if (elf_getphdrnum(this->elf, &this->n_secs))
        throw QTMIPS_EXCEPTION(Input, "Elf program sections count query failed", elf_errmsg(-1));
    // Get program sections headers
    if (!(this->phdrs = elf32_getphdr(this->elf)))
        throw QTMIPS_EXCEPTION(Input, "Elf program sections get failed", elf_errmsg(-1));
    // We want only LOAD sections so we create map of those sections
    for (unsigned i = 1; i < this->n_secs; i++) {
        if (phdrs[i].p_type != PT_LOAD)
            continue;
        this->map.push_back(i);
    }
    // TODO instead of direct access should we be using sections and elf_data? And if so how to link program header and section?
}

ProgramLoader::ProgramLoader(QString file) : ProgramLoader(file.toStdString().c_str()) { }

ProgramLoader::~ProgramLoader() {
    // Close elf
    elf_end(this->elf);
    // Close file
    close(this->fd);
}

void ProgramLoader::to_memory(Memory *mem) {
    // Load program to memory (just dump it byte by byte)
    for (int i = 0; i < this->map.size(); i++) {
        std::uint32_t base_address = this->phdrs[this->map[i]].p_vaddr;
        char *f = elf_rawfile(this->elf, NULL);
        size_t phdrs_i = this->map[i];
        for (unsigned y = 0; y < this->phdrs[phdrs_i].p_filesz; y++) {
            mem->write_byte(base_address + y, (std::uint8_t) f[this->phdrs[phdrs_i].p_offset + y]);
        }
    }
}

std::uint32_t ProgramLoader::end() {
    std::uint32_t last = 0;
    // Go trough all sections and found out last one
    for (int i = 0; i < this->map.size(); i++) {
        Elf32_Phdr *phdr = &(this->phdrs[this->map[i]]);
        if ((phdr->p_vaddr + phdr->p_filesz) > last)
            last = phdr->p_vaddr + phdr->p_filesz;
    }
    return last + 0x10; // We add offset so we are sure that also pipeline is empty
}
