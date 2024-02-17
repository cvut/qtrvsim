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

namespace machine {

enum ArchitectureType {
    ARCH32,
    ARCH64,
};

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

    /** Tells whether the executable is 32bit or 64bit. */
    ArchitectureType get_architecture_type() const;

private:
    QFile elf_file;
    Elf *elf;
    GElf_Ehdr hdr {}; // elf file header
    size_t n_secs {}; // number of sections in elf program header
    ArchitectureType architecture_type;

private:
    union {
        Elf32_Phdr *arch32;
        Elf64_Phdr *arch64;
    } sections_headers{};
    QVector<size_t> indexes_of_load_sections; // external index to sections_headers index
    Address executable_entry;
};

} // namespace machine

#endif // PROGRAM_H
