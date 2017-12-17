#ifndef PROGRAM_H
#define PROGRAM_H

#include <unistd.h>
#include <libelf.h>
#include <gelf.h>
#include <cstdint>
#include <qvector.h>
#include <qstring.h>
#include <memory.h>

namespace machine {

class ProgramLoader {
public:
    ProgramLoader(const char *file);
    ProgramLoader(QString file);
    ~ProgramLoader();

    void to_memory(Memory *mem); // Writes all loaded sections to memory
    std::uint32_t end(); // Return address after which there is no more code for sure
private:
    int fd;
    Elf *elf;
    GElf_Ehdr hdr; // elf file header
    size_t n_secs; // number of sections in elf program header
    Elf32_Phdr *phdrs; // program section headers
    QVector<size_t> map; // external index to phdrs index
};

}

#endif // PROGRAM_H
