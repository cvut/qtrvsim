// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

#include "programloader.h"

#include "simulator_exception.h"

#include <cerrno>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <unistd.h>

#ifndef O_BINARY
    #define O_BINARY 0
#endif

using namespace machine;

ProgramLoader::ProgramLoader(const QString &file) : elf_file(file) {
    const GElf_Ehdr *elf_ehdr;
    // Initialize elf library
    if (elf_version(EV_CURRENT) == EV_NONE) {
        throw QTMIPS_EXCEPTION(
            Input, "Elf library initialization failed", elf_errmsg(-1));
    }
    // Open source file - option QIODevice::ExistingOnly cannot be used on Qt
    // <5.11
    if (!elf_file.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        throw QTMIPS_EXCEPTION(
            Input,
            QString("Can't open input elf file for reading (") + QString(file)
                + QString(")"),
            std::strerror(errno));
    }
    // Initialize elf
    if (!(this->elf = elf_begin(elf_file.handle(), ELF_C_READ, nullptr))) {
        throw QTMIPS_EXCEPTION(Input, "Elf read begin failed", elf_errmsg(-1));
    }
    // Check elf kind
    if (elf_kind(this->elf) != ELF_K_ELF) {
        throw QTMIPS_EXCEPTION(
            Input, "Invalid input file elf format, plain elf file expected",
            "");
    }

    elf_ehdr = gelf_getehdr(this->elf, &this->hdr);
    if (!elf_ehdr) {
        throw QTMIPS_EXCEPTION(
            Input, "Getting elf file header failed", elf_errmsg(-1));
    }

    executable_entry = Address(elf_ehdr->e_entry);
    // Check elf file format, executable expected, nothing else.
    if (this->hdr.e_type != ET_EXEC) {
        throw QTMIPS_EXCEPTION(Input, "Invalid input file type", "");
    }
    // Check elf file architecture, of course only mips is supported.
    // Note: This also checks that this is big endian as EM_MIPS is suppose to
    // be: MIPS R3000 big-endian
    if (this->hdr.e_machine != EM_MIPS) {
        throw QTMIPS_EXCEPTION(Input, "Invalid input file architecture", "");
    }
    // Check elf file class, only 32bit architecture is supported.
    int elf_class;
    if ((elf_class = gelf_getclass(this->elf)) == ELFCLASSNONE) {
        throw QTMIPS_EXCEPTION(
            Input, "Getting elf class failed", elf_errmsg(-1));
    }
    if (elf_class != ELFCLASS32) {
        throw QTMIPS_EXCEPTION(
            Input, "Only supported architecture is 32bit", "");
    }

    // Get number of program sections in elf file
    if (elf_getphdrnum(this->elf, &this->n_secs)) {
        throw QTMIPS_EXCEPTION(
            Input, "Elf program sections count query failed", elf_errmsg(-1));
    }
    // Get program sections headers
    if (!(this->phdrs = elf32_getphdr(this->elf))) {
        throw QTMIPS_EXCEPTION(
            Input, "Elf program sections get failed", elf_errmsg(-1));
    }
    // We want only LOAD sections so we create map of those sections
    for (unsigned i = 0; i < this->n_secs; i++) {
        if (phdrs[i].p_type != PT_LOAD) {
            continue;
        }
        this->map.push_back(i);
    }
    // TODO instead of direct access should we be using sections and elf_data?
    // And if so how to link program header and section?
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
    for (size_t phdrs_i : this->map) {
        uint32_t base_address = this->phdrs[phdrs_i].p_vaddr;
        char *f = elf_rawfile(this->elf, nullptr);
        for (unsigned y = 0; y < this->phdrs[phdrs_i].p_filesz; y++) {
            const auto buffer = (uint8_t)f[this->phdrs[phdrs_i].p_offset + y];
            memory_write_u8(mem, base_address + y, buffer);
        }
    }
}

Address ProgramLoader::end() {
    uint32_t last = 0;
    // Go trough all sections and found out last one
    for (size_t i : this->map) {
        Elf32_Phdr *phdr = &(this->phdrs[i]);
        if ((phdr->p_vaddr + phdr->p_filesz) > last) {
            last = phdr->p_vaddr + phdr->p_filesz;
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

    /* rettrieve the symbol names */
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
        throw QTMIPS_EXCEPTION(
            Input,
            "ELF header e_ident malformed."
            "Unknown value of the byte EI_DATA."
            "Expected value little (=1) or big (=2).",
            "");
    }
}
