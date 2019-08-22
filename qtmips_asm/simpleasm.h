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

#ifndef  SIMPLEASM_H
#define  SIMPLEASM_H

#include <QString>
#include <QStringList>
#include "fixmatheval.h"
#include "qtmipsmachine.h"
#include "messagetype.h"

class SymbolTableDb : public fixmatheval::FmeSymbolDb {
public:
    SymbolTableDb(machine::SymbolTable *symtab);
    virtual bool getValue(fixmatheval::FmeValue &value, QString name) override;
    void setSymbol(QString name, std::uint32_t value, std::uint32_t size,
              unsigned char info = 0, unsigned char other = 0);
private:
    machine::SymbolTable *symtab;
};

class SimpleAsm : public QObject {
    Q_OBJECT

    using Super = QObject;

signals:
    void report_message(messagetype::Type type, QString file, int line,
                                   int column, QString text, QString hint);

public:
    SimpleAsm(QObject *parent = nullptr);
    ~SimpleAsm();
public:
    static std::uint64_t string_to_uint64(QString str, int base,
                                          int *chars_taken = nullptr);
    void clear();
    void setup(machine::MemoryAccess *mem, SymbolTableDb *symtab, std::uint32_t address);
    bool process_line(QString line, QString filename = "",
                      int line_number = 0, QString *error_ptr = nullptr);
    virtual bool process_file(QString filename, QString *error_ptr = nullptr);
    bool finish(QString *error_ptr = nullptr);
protected:
    bool error_occured;
    bool fatal_occured;
private:
    QStringList include_stack;
    SymbolTableDb *symtab;
    machine::MemoryAccess *mem;
    machine::RelocExpressionList reloc;
    std::uint32_t address;
};

#endif /*SIMPLEASM_H*/
