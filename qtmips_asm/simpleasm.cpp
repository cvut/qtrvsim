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
    QString label = "";
    QString op = "";
    QStringList operands;
    int pos;
    bool in_quotes = false;
    bool backslash = false;
    bool maybe_label = true;
    bool final = false;
    bool separator;
    int token_beg = -1;
    int token_last = -1;
    int operand_num = -1;

    for (pos = 0; pos <= line.count(); pos++) {
        QChar ch = ' ';
        if (pos >= line.count())
            final = true;
        if (!final)
            ch = line.at(pos);
        if (!in_quotes) {
            if (ch == '#')
                final = true;
            if (ch == ';')
                final = true;
            if (ch == '/') {
                if (pos + 1 < line.count())
                    if (line.at(pos + 1) == '/')
                        final = true;
            }
            separator = final || (maybe_label && (ch == ':')) || ((operand_num >= 0) && (ch == ','));
            if (maybe_label && (ch == ':')) {
                maybe_label = false;
                if (token_beg == -1) {
                    error = "empty label";
                    emit report_message(messagetype::MSG_ERROR, filename, line_number, pos, error, "");
                    error_occured = true;
                    if (error_ptr != nullptr)
                        *error_ptr = error;
                    return false;
                }
                label = line.mid(token_beg, token_last - token_beg + 1);
                token_beg = -1;
            } else if (((!ch.isSpace() && (token_beg >= 0) && (token_last < pos - 1)) ||
                    final) && (operand_num == -1)) {
                maybe_label = false;
                if (token_beg != -1)
                    op = line.mid(token_beg, token_last - token_beg + 1).toUpper();
                token_beg = -1;
                operand_num = 0;
                if (ch == ',') {
                    error = "empty first operand";
                    emit report_message(messagetype::MSG_ERROR, filename, line_number, pos, error, "");
                    error_occured = true;
                    if (error_ptr != nullptr)
                        *error_ptr = error;
                    return false;
                }
            } else if (separator || final) {
                if (token_beg == -1) {
                    error = "empty operand";
                    emit report_message(messagetype::MSG_ERROR, filename, line_number, pos, error, "");
                    error_occured = true;
                    if (error_ptr != nullptr)
                        *error_ptr = error;
                    return false;
                }
                operands.append(line.mid(token_beg, token_last - token_beg + 1));
                token_beg = -1;
                operand_num++;
            }
            if (final)
                break;
            if (!ch.isSpace() && !separator) {
                if (token_beg == -1)
                    token_beg = pos;
                token_last = pos;
            }
            backslash = false;
            if (ch == '"') {
                if (operand_num == -1) {
                    error = "unexpected quoted text";
                    emit report_message(messagetype::MSG_ERROR, filename, line_number, pos, error, "");
                    error_occured = true;
                    if (error_ptr != nullptr)
                        *error_ptr = error;
                    return false;
                }
                in_quotes = true;
            }
        } else {
            token_last = pos;
            if (final) {
                error = "unterminated quoted text";
                emit report_message(messagetype::MSG_ERROR, filename, line_number, pos, error, "");
                error_occured = true;
                if (error_ptr != nullptr)
                    *error_ptr = error;
                return false;
            }
            if ((ch == '"') && !backslash)
                in_quotes = false;
            if ((ch == '\\')  && !backslash)
                 backslash = true;
            else
                backslash = false;
        }
    }

    if (!label.isEmpty()) {
        symtab->setSymbol(label, address, 4);
    }

    if (op.isEmpty()) {
        if (operands.count() != 0) {
            error = "operands for empty operation";
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            if (error_ptr != nullptr)
                *error_ptr = error;
            return false;
        }
        return true;
    }

    if ((op == ".DATA") || (op == ".TEXT") ||
        (op == ".GLOBL") || (op == ".END") ||
        (op == ".ENT")) {
        return true;
    }
    if (op == ".ORG") {
        bool ok;
        fixmatheval::FmeExpression expression;
        fixmatheval::FmeValue value;
        if (operands.count() != 1) {
            error = ".org unexpected number of operands";
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            if (error_ptr != nullptr)
                *error_ptr = error;
            return false;
        }
        ok = expression.parse(operands.at(0), error);
        if (!ok) {
            fatal_occured = true;
            error = tr(".orig %1 parse error.").arg(line);
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            if (error_ptr != nullptr)
                *error_ptr = error;
            return false;
        }
        ok = expression.eval(value, symtab, error);
        if (!ok) {
            fatal_occured = true;
            error = tr(".orig %1 evaluation error.").arg(line);
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            if (error_ptr != nullptr)
                *error_ptr = error;
            return false;
        }
        address = value;
        return true;
    }
    if ((op == ".EQU") || (op == ".SET")) {
        if ((operands.count() > 2) || (operands.count() < 1)) {
            error = tr(".set or .equ incorrect arguments number.");
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
                error = tr(".set or .equ %1 parse error.")
                           .arg(operands.at(1));
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
    if ((op == ".ASCII") || (op == ".ASCIZ")) {
        bool append_zero = op == ".ASCIZ";
        for (QString s: operands) {
            if (s.count() < 2) {
                error = "ascii empty string";
                emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
                error_occured = true;
                if (error_ptr != nullptr)
                    *error_ptr = error;
                return false;
            }
            if ((s.at(0) != '"') || (s.at(s.count() - 1) != '"')) {
                error = "ascii missing quotes";
                emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
                error_occured = true;
                if (error_ptr != nullptr)
                    *error_ptr = error;
                return false;
            }
            s = s.mid(1, s.count() - 2);
            for (pos = 0; pos < s.count(); pos++) {
                QChar ch = s.at(pos);
                if (ch == '\\') {
                    ch = 0;
                    if (pos + 1 < s.count()) switch(s.at(++pos).toLatin1()) {
                        case '\\':
                            ch = '\\';
                            break;
                        case 'r':
                            ch = 0x0d;
                            break;
                        case 'n':
                            ch = 0x0a;
                            break;
                        case 't':
                            ch = 0x09;
                            break;
                        case 'b':
                            ch = 0x08;
                            break;
                        case '"':
                            ch = '"';
                            break;
                        default:
                            ch = 0;
                    }
                    if (ch == '\0') {
                        error = "ascii - incorrect escape sequence";
                        emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
                        error_occured = true;
                        if (error_ptr != nullptr)
                            *error_ptr = error;
                        return false;

                    }
                }
                if (!fatal_occured)
                    mem->write_byte(address, (uint8_t)ch.toLatin1());
                address += 1;
            }
            if (append_zero) {
                if (!fatal_occured)
                    mem->write_byte(address, 0);
                address += 1;
            }
        }
        return true;
    }
    if (op == ".BYTE") {
        bool ok;
        for (QString s: operands) {
            std::uint32_t val = 0;
            int chars_taken;
            val = string_to_uint64(s, 0, &chars_taken);
            if (chars_taken != s.size()) {
                fixmatheval::FmeExpression expression;
                fixmatheval::FmeValue value;
                ok = expression.parse(s, error);
                if (!ok) {
                    fatal_occured = true;
                    error = tr(".byte %1 parse error.").arg(line);
                    emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
                    error_occured = true;
                    if (error_ptr != nullptr)
                        *error_ptr = error;
                    return false;
                }
                ok = expression.eval(value, symtab, error);
                if (!ok) {
                    fatal_occured = true;
                    error = tr(".byte %1 evaluation error.").arg(line);
                    emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
                    error_occured = true;
                    if (error_ptr != nullptr)
                        *error_ptr = error;
                    return false;
                }
                val = (uint8_t)value;
            }
            if (!fatal_occured)
                mem->write_byte(address, (uint8_t)val);
            address += 1;
        }
        return true;
    }

    while (address & 3) {
        if (!fatal_occured)
            mem->write_byte(address, 0);
        address += 1;
    }

    if (op == ".WORD") {
        for (QString s: operands) {
            s = s.simplified();
            std::uint32_t val = 0;
            int chars_taken;
            val = string_to_uint64(s, 0, &chars_taken);
            if (chars_taken != s.size()) {
                val = 0;
                reloc.append(new machine::RelocExpression(address, s, 0,
                             -0xffffffff, 0xffffffff, 0, 32, 0, filename, line_number, 0));
            }
            if (!fatal_occured)
                mem->write_word(address, val);
            address += 4;
        }
        return true;
    }

    std::uint32_t inst[2] = {0, 0};
    ssize_t size = machine::Instruction::code_from_string(inst, 8, op, operands, error,
                                             address, &reloc, filename, line_number, true);

    if (size < 0) {
        error = tr("instruction %1 parse error - %2.")
                .arg(line, error);
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
