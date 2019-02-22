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

using namespace machine;

SymbolTableEntry::SymbolTableEntry(QString name, std::uint32_t value,
            std::uint32_t size, unsigned char info, unsigned char other) {
    this->name = name;
    this->value = value;
    this->size = size;
    this->info = info;
    this->other = other;
}

SymbolTable::SymbolTable(QObject *parent) : QObject(parent),
                          map_value_to_symbol(), map_name_to_symbol() {
}

SymbolTable::~SymbolTable() {
    while (!map_value_to_symbol.isEmpty()) {
        SymbolTableEntry *p_ste = map_value_to_symbol.first();
        p_ste = map_value_to_symbol.take(p_ste->value);
        map_name_to_symbol.remove(p_ste->name);
        delete p_ste;
    }
}

void SymbolTable::add_symbol(QString name, std::uint32_t value, std::uint32_t size,
          unsigned char info, unsigned char other) {
    SymbolTableEntry *p_ste = new SymbolTableEntry(name, value, size, info, other);
    map_value_to_symbol.insert(value, p_ste);
    map_name_to_symbol.insert(name, p_ste);
}

bool SymbolTable::name_to_value(std::uint32_t &value, QString name) const {
    SymbolTableEntry *p_ste = map_name_to_symbol.value(name);
    if (p_ste == nullptr) {
        value = 0;
        return false;
    }
    value = p_ste->value;
    return true;
}

bool SymbolTable::value_to_name(QString &name, std::uint32_t value) const {
    SymbolTableEntry *p_ste = map_value_to_symbol.value(value);
    if (p_ste == nullptr) {
        name = "";
        return false;
    }
    name = p_ste->name;
    return true;
}

QStringList *SymbolTable::names() const {
    QStringList *l = new QStringList();

    auto i = map_name_to_symbol.begin();
    while (i != map_name_to_symbol.end()) {
        l->append(i.value()->name);
        i++;
    }
    return l;
}
