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

#include <QObject>

#include "simpleasm.h"

using namespace fixmatheval;


SymbolTableDb::SymbolTableDb(machine::SymbolTable *symtab) {
    this->symtab = symtab;
}

bool SymbolTableDb::getValue(fixmatheval::FmeValue &value, QString name) {
    std::uint32_t val;
    if (!symtab->name_to_value(val, name))
        return false;
    value = val;
    return true;
}

void SymbolTableDb::setSymbol(QString name, std::uint32_t value, std::uint32_t size,
          unsigned char info, unsigned char other) {
    symtab->set_symbol(name, value, size, info, other);
}


std::uint64_t SimpleAsm::string_to_uint64(QString str, int base,
                                      int *chars_taken) {
    int i;
    std::int64_t val;
    char *p, *r;
    char cstr[str.count() + 1];
    for (i = 0; i < str.count(); i++)
        cstr[i] = str.at(i).toLatin1();
    cstr[i] = 0;
    p = cstr;
    val = std::strtoll(p, &r, base);
    if (chars_taken != nullptr)
        *chars_taken = r - p;
    return val;
}

SimpleAsm::SimpleAsm(QObject *parent) : Super(parent) {
    clear();
}

SimpleAsm::~SimpleAsm() {
    clear();
}

void SimpleAsm::clear() {
    symtab = nullptr;
    mem = nullptr;
    while (!reloc.isEmpty()) {
        delete reloc.takeFirst();
    }
    error_occured = false;
    fatal_occured = false;
}

void SimpleAsm::setup(machine::MemoryAccess *mem, SymbolTableDb *symtab,
                      std::uint32_t address) {
    this->mem = mem;
    this->symtab = symtab;
    this->address = address;
}

bool SimpleAsm::process_line(QString line, QString filename,
                             int line_number, QString *error_ptr) {
    QString error;
    int pos;
    QString label = "";
    pos = line.indexOf("#");
    if (pos >= 0)
        line.truncate(pos);
    pos = line.indexOf(";");
    if (pos >= 0)
        line.truncate(pos);
    pos = line.indexOf("//");
    if (pos >= 0)
        line.truncate(pos);
    line = line.simplified();
    pos = line.indexOf(":");
    if (pos >= 0) {
        label = line.mid(0, pos);
        line = line.mid(pos + 1).trimmed();
        symtab->setSymbol(label, address, 4);
    }
    if (line.isEmpty())
        return true;
    int k = 0, l;
    while (k < line.count()) {
        if (!line.at(k).isSpace())
            break;
        k++;
    }
    l = k;
    while (l < line.count()) {
        if (!line.at(l).isLetterOrNumber() && !(line.at(l) == '.'))
            break;
        l++;
    }
    QString op = line.mid(k, l - k).toUpper();
    if ((op == ".DATA") || (op == ".TEXT") ||
        (op == ".GLOBL") || (op == ".END") ||
        (op == ".ENT")) {
        return true;
    }
    if (op == ".ORG") {
        bool ok;
        fixmatheval::FmeExpression expression;
        fixmatheval::FmeValue value;
        ok = expression.parse(line.mid(op.size()), error);
        if (ok)
            ok = expression.eval(value, symtab, error);
        if (!ok) {
            error = tr("line %1 .orig %2 parse error.")
                    .arg(QString::number(line_number), line);
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            fatal_occured = true;
            if (error_ptr != nullptr)
                *error_ptr = error;
            return false;
        }
        address = value;
        return true;
    }
    if ((op == ".EQU") || (op == ".SET")) {
        QStringList operands = line.mid(op.size()).split(",");
        if ((operands.count() > 2) || (operands.count() < 1)) {
            error = tr("line %1 .set or .equ incorrect arguments number.")
                    .arg(QString::number(line_number));
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            if (error_ptr != nullptr)
                *error_ptr = error;
            return false;
        }
        QString name = operands.at(0).trimmed();
        if ((name == "noat") || (name == "noreored"))
            return true;
        bool ok;
        fixmatheval::FmeValue value = 1;
        if (operands.count() > 1) {
            fixmatheval::FmeExpression expression;
            ok = expression.parse(operands.at(1), error);
            if (ok)
                ok = expression.eval(value, symtab, error);
            if (!ok) {
                error = tr("line %1 .set or .equ %2 parse error.")
                           .arg(QString::number(line_number), operands.at(1));
                emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
                error_occured = true;
                if (error_ptr != nullptr)
                    *error_ptr = error;
                return false;
            }
        }
        symtab->setSymbol(name, value, 0);
        return true;
    }
    if (op == ".WORD") {
        foreach (QString s, line.mid(op.size()).split(",")) {
            s = s.simplified();
            std::uint32_t val = 0;
            int chars_taken;
            val = string_to_uint64(s, 0, &chars_taken);
            if (chars_taken != s.size()) {
                val = 0;
                reloc.append(new machine::RelocExpression(address, s, 0,
                             -0xffffffff, 0xffffffff, 0, 32, 0, filename, line_number, 0));
            }
            if (fatal_occured)
                mem->write_word(address, val);
            address += 4;
        }
        return true;
    }

    std::uint32_t inst[2] = {0, 0};
    ssize_t size = machine::Instruction::code_from_string(inst, 8, line, error,
                                             address, &reloc, filename, line_number, true);
    if (size < 0) {
        error = tr("line %1 instruction %2 parse error - %3.")
                .arg(QString::number(line_number), line, error);
        emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
        error_occured = true;
        if (error_ptr != nullptr)
            *error_ptr = error;
        return false;
    }
    std::uint32_t *p = inst;
    for (ssize_t l = 0; l < size; l += 4) {
        if (!fatal_occured)
            mem->write_word(address, *(p++));
        address += 4;
    }
    return true;
}

bool SimpleAsm::finish(QString *error_ptr) {
    bool error_reported = false;
    foreach(machine::RelocExpression *r, reloc) {
        QString error;
        fixmatheval::FmeExpression expression;
        if (!expression.parse(r->expression, error)) {
            error = tr("expression parse error %1 at line %2, expression %3.")
                    .arg(error, QString::number(r->line), expression.dump());
            emit report_message(messagetype::MSG_ERROR, r->filename, r->line, 0, error, "");
            if (error_ptr != nullptr && !error_reported)
                *error_ptr = error;
            error_occured = true;
            error_reported = true;
        } else {
            fixmatheval::FmeValue value;
            if (!expression.eval(value, symtab, error)) {
                error = tr("expression evalution error %1 at line %2 , expression %3.")
                        .arg(error, QString::number(r->line), expression.dump());
                emit report_message(messagetype::MSG_ERROR, r->filename, r->line, 0, error, "");
                if (error_ptr != nullptr && !error_reported)
                    *error_ptr = error;
                error_occured = true;
                error_reported = true;
            } else {
                if (false)
                    emit report_message(messagetype::MSG_INFO, r->filename, r->line, 0,
                                   expression.dump() + " -> " + QString::number(value), "");
                machine::Instruction inst(mem->read_word(r->location, true));
                if (!inst.update(value, r)) {
                    error = tr("instruction update error %1 at line %2, expression %3 -> value %4.")
                            .arg(error, QString::number(r->line), expression.dump(), QString::number(value));
                    emit report_message(messagetype::MSG_ERROR, r->filename, r->line, 0, error, "");
                    if (error_ptr != nullptr && !error_reported)
                        *error_ptr = error;
                    error_occured = true;
                    error_reported = true;
                }
                if (!fatal_occured)
                    mem->write_word(r->location, inst.data());
            }
        }
    }
    while (!reloc.isEmpty()) {
        delete reloc.takeFirst();
    }

    emit mem->external_change_notify(mem, 0, 0xffffffff, true);

    return !error_occured;
}
