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

#ifndef SIMPLEASM_H
#define SIMPLEASM_H

#include "fixmatheval.h"
#include "machine/machine.h"
#include "machine/memory/frontend_memory.h"
#include "messagetype.h"

#include <QString>
#include <QStringList>

using machine::SymbolInfo;
using machine::SymbolOther;
using machine::SymbolSize;
using machine::SymbolValue;

class SymbolTableDb : public fixmatheval::FmeSymbolDb {
public:
    explicit SymbolTableDb(machine::SymbolTable *symbol_table);
    bool getValue(fixmatheval::FmeValue &value, QString name) override;
    void setSymbol(
        const QString &name,
        SymbolValue value,
        SymbolSize size,
        SymbolInfo info = 0,
        SymbolOther other = 0);

private:
    machine::SymbolTable *symbol_table;
};

class SimpleAsm : public QObject {
    Q_OBJECT

    using Super = QObject;

signals:
    void report_message(
        messagetype::Type type,
        QString file,
        int line,
        int column,
        QString text,
        QString hint);

public:
    explicit SimpleAsm(QObject *parent = nullptr);
    ~SimpleAsm() override;

public:
    static uint64_t
    string_to_uint64(const QString &str, int base, int *chars_taken = nullptr);
    void clear();
    void setup(
        machine::FrontendMemory *mem,
        SymbolTableDb *symtab,
        machine::Address address);
    bool process_line(
        const QString &line,
        const QString &filename = "",
        int line_number = 0,
        QString *error_ptr = nullptr);
    virtual bool
    process_file(const QString &filename, QString *error_ptr = nullptr);
    bool finish(QString *error_ptr = nullptr);

protected:
    virtual bool process_pragma(
        QStringList &operands,
        const QString &filename = "",
        int line_number = 0,
        QString *error_ptr = nullptr);
    bool error_occured {};
    bool fatal_occured {};
    SymbolTableDb *symtab {};

private:
    QStringList include_stack;
    machine::FrontendMemory *mem {};
    machine::RelocExpressionList reloc;
    machine::Address address {};
};

#endif /*SIMPLEASM_H*/
