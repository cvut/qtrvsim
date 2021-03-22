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

#include "symboltable.h"

#include <utility>

using namespace machine;

SymbolTableEntry::SymbolTableEntry(
    QString name,
    SymbolValue value,
    SymbolSize size,
    SymbolInfo info,
    SymbolOther other)
    : name(std::move(name))
    , value(value)
    , size(size)
    , info(info)
    , other(other) {}

SymbolTable::SymbolTable(QObject *parent)
    : QObject(parent)
    , map_name_to_symbol()
    , map_value_to_symbol() {}

SymbolTable::~SymbolTable() {
    map_name_to_symbol.clear(); // Does not own data.
    auto iter = map_value_to_symbol.begin();
    while (iter != map_value_to_symbol.end()) {
        const SymbolTableEntry *p_entry = iter.value();
        iter = map_value_to_symbol .erase(iter); // Advances iterator.
        delete p_entry;
    }
}

void SymbolTable::add_symbol(
    const QString &name,
    SymbolValue value,
    uint32_t size,
    unsigned char info,
    unsigned char other) {
    auto *p_entry = new SymbolTableEntry(name, value, size, info, other);
    map_value_to_symbol.insert(value, p_entry);
    map_name_to_symbol.insert(name, p_entry);
}

void SymbolTable::remove_symbol(const QString &name) {
    auto *p_entry = map_name_to_symbol.take(name);
    if (p_entry == nullptr) {
        return;
    }
    map_value_to_symbol.remove(p_entry->value, p_entry);
    delete p_entry;
}

void SymbolTable::set_symbol(
    const QString &name,
    SymbolValue value,
    uint32_t size,
    unsigned char info,
    unsigned char other) {
    remove_symbol(name);
    add_symbol(name, value, size, info, other);
}

// TODO cpp17 - return optional
bool SymbolTable::name_to_value(SymbolValue &value, const QString &name) const {
    auto *p_entry = map_name_to_symbol.value(name);
    if (p_entry == nullptr) {
        value = 0;
        return false;
    }
    value = p_entry->value;
    return true;
}

// TODO cpp17 - return optional
bool SymbolTable::location_to_name(QString &name, SymbolValue value) const {
    auto *p_entry = map_value_to_symbol.value(value);
    if (p_entry == nullptr) {
        name = "";
        return false;
    }
    name = p_entry->name;
    return true;
}

QStringList SymbolTable::names() const {
    return map_name_to_symbol.keys();
}
