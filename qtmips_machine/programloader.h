#ifndef PROGRAM_H
#define PROGRAM_H

#include <unistd.h>
#include <libelf.h>
#include <gelf.h>
#include <cstdint>
#include <qvector.h>


class ProgramLoader {
public:
    ProgramLoader(char *file);
    ~ProgramLoader();

    size_t get_nsec(); // Returns number of loadable sections
    std::uint32_t get_address(size_t sec); // Get target address for given section
    QVector<std::uint8_t> get_data(size_t sec); // Returns bytes of given section
private:
    int fd;
    Elf *elf;
    GElf_Ehdr hdr; // elf file header
    size_t n_secs; // number of sections in elf program header
    Elf32_Phdr *phdrs; // program section headers
    QVector<size_t> map; // external index to phdrs index
};

#endif // PROGRAM_H
