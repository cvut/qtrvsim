#ifndef PROGRAM_H
#define PROGRAM_H

#include "common/endian.h"
#include "memory/backend/memory.h"
#include "symboltable.h"

#include <QFile>
#include <cstdint>
#include <qstring.h>
#include <qvector.h>
#include <memory>
#include <elf++.hh>

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
    Address end();               // Return address after which there is no more code for
                                 // sure
    Address get_executable_entry() const;
    SymbolTable *get_symbol_table();

    Endian get_endian() const;

    /** Tells whether the executable is 32bit or 64bit. */
    ArchitectureType get_architecture_type() const;

private:
    QFile elf_file;
    std::unique_ptr<elf::elf> elf_parser;
    uint64_t entry_point {};
    size_t n_secs {}; // number of sections in elf program header
    ArchitectureType architecture_type;
    Endian endianness;
    bool is_64bit {};
};

} // namespace machine

#endif // PROGRAM_H
