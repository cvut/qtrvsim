#ifndef PROGRAM_H
#define PROGRAM_H

#include "common/endian.h"
#include "memory/backend/memory.h"
#include "symboltable.h"

#include <QFile>
#include <cstdint>
#include <elf/elf++.hh>
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
    Address end();               // Return address after which there is no more code for
                                 // sure
    Address get_executable_entry() const;
    SymbolTable *get_symbol_table();

    Endian get_endian() const;

    /** Tells whether the executable is 32bit or 64bit. */
    ArchitectureType get_architecture_type() const;

private:
    elf::elf elf_file;
    ArchitectureType architecture_type;
    Address executable_entry;
    std::vector<elf::segment> load_segments;
};

} // namespace machine

#endif // PROGRAM_H
