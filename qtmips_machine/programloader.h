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
    std::uint32_t get_executable_entry();
private:
    int fd;
    Elf *elf;
    GElf_Ehdr hdr; // elf file header
    size_t n_secs; // number of sections in elf program header
    Elf32_Phdr *phdrs; // program section headers
    QVector<size_t> map; // external index to phdrs index
    std::uint32_t executable_entry;
};

}

#endif // PROGRAM_H
