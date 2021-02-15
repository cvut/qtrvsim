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

#include "machinedefs.h"
#include "qtmipsexception.h"

#include <QObject>
#include <cstdint>

namespace machine {

// Virtual class for common memory access
class MemoryAccess : public QObject {
    Q_OBJECT
public:
    // Note: hword and word methods are throwing away lowest bits so unaligned
    // access is ignored without error.
    bool write_byte(uint32_t offset, uint8_t value);
    bool write_hword(uint32_t offset, uint16_t value);
    bool write_word(uint32_t offset, uint32_t value);

    uint8_t read_byte(uint32_t offset, bool debug_access = false) const;
    uint16_t read_hword(uint32_t offset, bool debug_access = false) const;
    uint32_t read_word(uint32_t offset, bool debug_access = false) const;

    void write_ctl(enum AccessControl ctl, uint32_t offset, uint32_t value);
    uint32_t read_ctl(enum AccessControl ctl, uint32_t offset) const;

    virtual void sync();
    virtual enum LocationStatus location_status(uint32_t offset) const;
    virtual uint32_t get_change_counter() const = 0;

signals:
    void external_change_notify(
        const MemoryAccess *mem_access,
        uint32_t start_addr,
        uint32_t last_addr,
        bool external) const;

protected:
    virtual bool wword(uint32_t offset, uint32_t value) = 0;
    virtual uint32_t rword(uint32_t offset, bool debug_access = false) const = 0;

private:
    static int sh_nth(uint32_t offset);
};

class MemorySection : public MemoryAccess {
public:
    MemorySection(uint32_t length);
    MemorySection(const MemorySection &);
    ~MemorySection() override;

    bool wword(uint32_t offset, uint32_t value) override;
    uint32_t
    rword(uint32_t offsetbool, bool debug_access = false) const override;
    uint32_t get_change_counter() const override;
    void merge(MemorySection &);

    uint32_t length() const;
    const uint32_t *data() const;

    bool operator==(const MemorySection &) const;
    bool operator!=(const MemorySection &) const;

private:
    uint32_t len;
    uint32_t *dt;
};

union MemoryTree {
    union MemoryTree *mt;
    MemorySection *sec;
};

class Memory : public MemoryAccess {
    Q_OBJECT
public:
    Memory();
    Memory(const Memory &);
    ~Memory() override;
    void reset(); // Reset whole content of memory (removes old tree and creates
                  // new one)
    void reset(const Memory &);

    MemorySection *get_section(uint32_t address, bool create) const; // returns
                                                                     // section
                                                                     // containing
                                                                     // given
                                                                     // address
    bool wword(uint32_t address, uint32_t value) override;
    uint32_t rword(uint32_t address, bool debug_access = false) const override;
    uint32_t get_change_counter() const override;

    bool operator==(const Memory &) const;
    bool operator!=(const Memory &) const;

    const union MemoryTree *get_memorytree_root() const;

private:
    union MemoryTree *mt_root;
    uint32_t change_counter {};
    uint32_t write_counter {};
    static union MemoryTree *allocate_section_tree();
    static void free_section_tree(union MemoryTree *, size_t depth);
    static bool compare_section_tree(
        const union MemoryTree *,
        const union MemoryTree *,
        size_t depth);
    static bool is_zero_section_tree(const union MemoryTree *, size_t depth);
    static union MemoryTree *
    copy_section_tree(const union MemoryTree *, size_t depth);
};

} // namespace machine

Q_DECLARE_METATYPE(machine::AccessControl)
Q_DECLARE_METATYPE(machine::Memory)
Q_DECLARE_METATYPE(machine::LocationStatus)

#endif // MEMORY_H
