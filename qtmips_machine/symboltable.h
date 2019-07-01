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

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <QString>
#include <QObject>
#include <QMap>
#include <QMultiMap>
#include <QStringList>

namespace machine {

class SymbolTableEntry {
    friend class SymbolTable;
    SymbolTableEntry(QString name, std::uint32_t value, std::uint32_t size,
                     unsigned char info = 0, unsigned char other = 0);
protected:
    QString name;
    std::uint32_t value;
    std::uint32_t size;
    unsigned char info;
    unsigned char other;
};

class SymbolTable : public QObject
{
    Q_OBJECT
public:
    SymbolTable(QObject *parent = 0);
    ~SymbolTable();

    void add_symbol(QString name, std::uint32_t value, std::uint32_t size,
              unsigned char info = 0, unsigned char other = 0);
    void remove_symbol(QString name);
    QStringList *names() const;
public slots:
    bool name_to_value(std::uint32_t &value, QString name) const;
    bool value_to_name(QString &name, std::uint32_t value) const;
signals:


private:
    QMultiMap<std::uint32_t, SymbolTableEntry*> map_value_to_symbol;
    QMap<QString, SymbolTableEntry*> map_name_to_symbol;
};

}

#endif // SYMBOLTABLE_H
