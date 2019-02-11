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

#ifndef MEMORY_H
#define MEMORY_H

#include <QObject>
#include <cstdint>
#include <qtmipsexception.h>
#include "machinedefs.h"

namespace machine {

// Virtual class for common memory access
class MemoryAccess : public QObject {
    Q_OBJECT
public:
    // Note: hword and word methods are throwing away lowest bits so unaligned access is ignored without error.
    bool write_byte(std::uint32_t offset, std::uint8_t value);
    bool write_hword(std::uint32_t offset, std::uint16_t value);
    bool write_word(std::uint32_t offset, std::uint32_t value);

    std::uint8_t read_byte(std::uint32_t offset, bool debug_access = false) const;
    std::uint16_t read_hword(std::uint32_t offset, bool debug_access = false) const;
    std::uint32_t read_word(std::uint32_t offset, bool debug_access = false) const;

    void write_ctl(enum AccessControl ctl, std::uint32_t offset, std::uint32_t value);
    std::uint32_t read_ctl(enum AccessControl ctl, std::uint32_t offset) const;

    virtual void sync();
    virtual enum LocationStatus location_status(std::uint32_t offset) const;

protected:
    virtual bool wword(std::uint32_t offset, std::uint32_t value) = 0;
    virtual std::uint32_t rword(std::uint32_t offset, bool debug_access = false) const = 0;

private:
    static int sh_nth(std::uint32_t offset);
};

class MemorySection : public MemoryAccess {
public:
    MemorySection(std::uint32_t length);
    MemorySection(const MemorySection&);
    ~MemorySection();

    bool wword(std::uint32_t offset, std::uint32_t value);
    std::uint32_t rword(std::uint32_t offsetbool, bool debug_access = false) const;
    void merge(MemorySection&);

    std::uint32_t length() const;
    const std::uint32_t* data() const;

    bool operator==(const MemorySection&) const;
    bool operator!=(const MemorySection&) const;

private:
    std::uint32_t len;
    std::uint32_t *dt;
};

union MemoryTree {
    union MemoryTree *mt;
    MemorySection *sec;
};

class Memory : public MemoryAccess {
    Q_OBJECT
public:
    Memory();
    Memory(const Memory&);
    ~Memory();
    void reset(); // Reset whole content of memory (removes old tree and creates new one)
    void reset(const Memory&);

    MemorySection *get_section(std::uint32_t address, bool create) const; // returns section containing given address
    bool wword(std::uint32_t address, std::uint32_t value);
    std::uint32_t rword(std::uint32_t address, bool debug_access = false) const;

    bool operator==(const Memory&) const;
    bool operator!=(const Memory&) const;

    const union MemoryTree *get_memorytree_root() const;

    inline std::uint32_t get_change_counter() const {
        return change_counter;
    }

private:
    union MemoryTree *mt_root;
    std::uint32_t change_counter;
    std::uint32_t write_counter;
    static union MemoryTree *allocate_section_tree();
    static void free_section_tree(union MemoryTree*, size_t depth);
    static bool compare_section_tree(const union MemoryTree*, const union MemoryTree*, size_t depth);
    static bool is_zero_section_tree(const union MemoryTree*, size_t depth);
    static union MemoryTree *copy_section_tree(const union MemoryTree*, size_t depth);
};

}

Q_DECLARE_METATYPE(machine::AccessControl)
Q_DECLARE_METATYPE(machine::Memory)

#endif // MEMORY_H
