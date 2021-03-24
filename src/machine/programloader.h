#ifndef PROGRAM_H
#define PROGRAM_H

#include "common/endian.h"
#include "memory/backend/memory.h"
#include "symboltable.h"

#include <QFile>
#include <cstdint>
#include <gelf.h>
#include <qstring.h>
#include <qvector.h>
#include <unistd.h>

namespace machine {

class ProgramLoader {
public:
    explicit ProgramLoader(const char *file);
    explicit ProgramLoader(const QString &file);
    ~ProgramLoader();

    void to_memory(Memory *mem); // Writes all loaded sections to memory TODO:
                                 // really to memory ???
    Address end(); // Return address after which there is no more code for
                   // sure
    Address get_executable_entry() const;
    SymbolTable *get_symbol_table();

    Endian get_endian() const;

private:
    QFile elf_file;
    Elf *elf;
    GElf_Ehdr hdr {};    // elf file header
    size_t n_secs {};    // number of sections in elf program header
    Elf32_Phdr *phdrs;   // program section headers
    QVector<size_t> map; // external index to phdrs index
    Address executable_entry;
};

} // namespace machine

#endif // PROGRAM_H
