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

#include "memory/memory_bus.h"

using namespace machine;

PhysAddrSpace::PhysAddrSpace() {
    change_counter = 0;
}

PhysAddrSpace::~PhysAddrSpace() {
    while (!ranges_by_access.isEmpty()) {
        RangeDesc *p_range = ranges_by_addr.first();
        ranges_by_addr.remove(p_range->last_addr);
        ranges_by_access.remove(p_range->mem_acces);
        if (p_range->owned) {
            delete p_range->mem_acces;
        }
        delete p_range;
    }
}

bool PhysAddrSpace::wword(uint32_t address, uint32_t value) {
    bool changed;
    RangeDesc *p_range = find_range(address);
    if (p_range == nullptr) {
        return false;
    }
    changed
        = p_range->mem_acces->write_word(address - p_range->start_addr, value);
    if (changed) {
        change_counter++;
    }
    return changed;
}

uint32_t PhysAddrSpace::rword(uint32_t address, bool debug_access) const {
    const RangeDesc *p_range = find_range(address);
    if (p_range == nullptr) {
        return 0x00000000;
    }
    return p_range->mem_acces->read_word(
        address - p_range->start_addr, debug_access);
}

uint32_t PhysAddrSpace::get_change_counter() const {
    return change_counter;
}

enum LocationStatus PhysAddrSpace::location_status(uint32_t address) const {
    const RangeDesc *p_range = find_range(address);
    if (p_range == nullptr) {
        return LOCSTAT_ILLEGAL;
    }
    return p_range->mem_acces->location_status(address - p_range->start_addr);
}

PhysAddrSpace::RangeDesc *PhysAddrSpace::find_range(uint32_t address) const {
    PhysAddrSpace::RangeDesc *p_range;
    auto i = ranges_by_addr.lowerBound(address);
    if (i == ranges_by_addr.end()) {
        return nullptr;
    }
    p_range = i.value();
    if (address >= p_range->start_addr && address <= p_range->last_addr) {
        return p_range;
    }
    return nullptr;
}

bool PhysAddrSpace::insert_range(
    MemoryAccess *mem_acces,
    uint32_t start_addr,
    uint32_t last_addr,
    bool move_ownership) {
    RangeDesc *p_range
        = new RangeDesc(mem_acces, start_addr, last_addr, move_ownership);
    auto i = ranges_by_addr.lowerBound(start_addr);
    if (i != ranges_by_addr.end()) {
        if (i.value()->start_addr <= last_addr
            && i.value()->last_addr >= start_addr) {
            return false;
        }
    }
    ranges_by_addr.insert(last_addr, p_range);
    ranges_by_access.insert(mem_acces, p_range);
    connect(
        mem_acces, &MemoryAccess::external_change_notify, this,
        &PhysAddrSpace::range_external_change);
    return true;
}

bool PhysAddrSpace::remove_range(MemoryAccess *mem_acces) {
    RangeDesc *p_range = ranges_by_access.take(mem_acces);
    if (p_range == nullptr) {
        return false;
    }
    ranges_by_addr.remove(p_range->last_addr);
    if (p_range->owned) {
        delete p_range->mem_acces;
    }
    delete p_range;
    return true;
}

void PhysAddrSpace::clean_range(uint32_t start_addr, uint32_t last_addr) {
    auto i = ranges_by_addr.lowerBound(start_addr);
    while (i != ranges_by_addr.end()) {
        RangeDesc *p_range = i.value();
        i++;
        if (p_range->start_addr <= last_addr) {
            remove_range(p_range->mem_acces);
        } else {
            break;
        }
    }
}

void PhysAddrSpace::range_external_change(
    const MemoryAccess *mem_access,
    uint32_t start_addr,
    uint32_t last_addr,
    bool external) {
    if (external) {
        change_counter++;
    }
    auto i = ranges_by_access.find(const_cast<MemoryAccess *>(mem_access));
    while (i != ranges_by_access.end() && i.key() == mem_access) {
        RangeDesc *p_range = i.value();
        ++i;
        emit external_change_notify(
            this, start_addr + p_range->start_addr,
            last_addr + p_range->start_addr, external);
    }
}

PhysAddrSpace::RangeDesc::RangeDesc(
    MemoryAccess *mem_acces,
    uint32_t start_addr,
    uint32_t last_addr,
    bool owned) {
    this->mem_acces = mem_acces;
    this->start_addr = start_addr;
    this->last_addr = last_addr;
    this->owned = owned;
}
