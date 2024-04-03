#include "simpleasm.h"

#include "machine/memory/address.h"
#include "machine/memory/memory_utils.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QString>

using namespace fixmatheval;
using machine::Address;
using ae = machine::AccessEffects; // For enum values, type is obvious from
                                   // context.

SymbolTableDb::SymbolTableDb(machine::SymbolTable *symbol_table) {
    this->symbol_table = symbol_table;
}

bool SymbolTableDb::getValue(fixmatheval::FmeValue &value, QString name) {
    SymbolValue val;
    if (!symbol_table->name_to_value(val, name)) { return false; }
    value = val;
    return true;
}

void SymbolTableDb::setSymbol(
    const QString &name,
    SymbolValue value,
    SymbolSize size,
    SymbolInfo info,
    SymbolOther other) {
    symbol_table->set_symbol(name, value, size, info, other);
}

uint64_t SimpleAsm::string_to_uint64(const QString &str, int base, int *chars_taken) {
    int i;
    int64_t val;
    char *p, *r;
    char cstr[str.count() + 1];
    for (i = 0; i < str.count(); i++) {
        cstr[i] = str.at(i).toLatin1();
    }
    cstr[i] = 0;
    p = cstr;
    val = std::strtoll(p, &r, base);
    if (chars_taken != nullptr) { *chars_taken = r - p; }
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

void SimpleAsm::setup(
    machine::FrontendMemory *mem,
    SymbolTableDb *symtab,
    machine::Address address,
    machine::Xlen xlen) {
    this->mem = mem;
    this->symtab = symtab;
    this->address = address;
    this->symtab->setSymbol("XLEN", static_cast<uint64_t>(xlen), sizeof(uint64_t));
}

static const machine::BitArg wordArg = { { { 32, 0 } }, 0 };

bool SimpleAsm::process_line(
    const QString &line,
    const QString &filename,
    int line_number,
    QString *error_ptr) {
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
    bool space_separated = false;
    int token_beg = -1;
    int token_last = -1;
    int operand_num = -1;

    for (pos = 0; pos <= line.count(); pos++) {
        QChar ch = ' ';
        if (pos >= line.count()) { final = true; }
        if (!final) { ch = line.at(pos); }
        if (!in_quotes) {
            if (ch == '#') {
                if (line.mid(pos).startsWith("#include")) {
                    if ((line.count() > pos + 8) && !line.at(pos + 8).isSpace()) { final = true; }
                } else if (line.mid(pos).startsWith("#pragma")) {
                    if ((line.count() > pos + 7) && !line.at(pos + 7).isSpace()) {
                        final = true;
                    } else {
                        space_separated = true;
                    }
                } else {
                    final = true;
                }
            }
            if (ch == ';') { final = true; }
            if (ch == '/') {
                if (pos + 1 < line.count()) {
                    if (line.at(pos + 1) == '/') { final = true; }
                }
            }
            separator
                = final || (maybe_label && (ch == ':'))
                  || ((operand_num >= 0)
                      && ((ch == ',') || (space_separated && ch.isSpace() && (token_beg != -1))));
            if (maybe_label && (ch == ':')) {
                maybe_label = false;
                if (token_beg == -1) {
                    error = "empty label";
                    emit report_message(
                        messagetype::MSG_ERROR, filename, line_number, pos, error, "");
                    error_occured = true;
                    if (error_ptr != nullptr) { *error_ptr = error; }
                    return false;
                }
                label = line.mid(token_beg, token_last - token_beg + 1);
                token_beg = -1;
            } else if (
                ((!ch.isSpace() && (token_beg >= 0) && (token_last < pos - 1)) || final)
                && (operand_num == -1)) {
                maybe_label = false;
                if (token_beg != -1) {
                    op = line.mid(token_beg, token_last - token_beg + 1).toLower();
                }
                token_beg = -1;
                operand_num = 0;
                if (ch == ',') {
                    error = "empty first operand";
                    emit report_message(
                        messagetype::MSG_ERROR, filename, line_number, pos, error, "");
                    error_occured = true;
                    if (error_ptr != nullptr) { *error_ptr = error; }
                    return false;
                }
            } else if (separator || final) {
                if (token_beg == -1) {
                    error = "empty operand";
                    emit report_message(
                        messagetype::MSG_ERROR, filename, line_number, pos, error, "");
                    error_occured = true;
                    if (error_ptr != nullptr) { *error_ptr = error; }
                    return false;
                }
                operands.append(line.mid(token_beg, token_last - token_beg + 1));
                token_beg = -1;
                operand_num++;
            }
            if (final) { break; }
            if (!ch.isSpace() && !separator) {
                if (token_beg == -1) { token_beg = pos; }
                token_last = pos;
            }
            backslash = false;
            if (ch == '"') {
                if (operand_num == -1) {
                    error = "unexpected quoted text";
                    emit report_message(
                        messagetype::MSG_ERROR, filename, line_number, pos, error, "");
                    error_occured = true;
                    if (error_ptr != nullptr) { *error_ptr = error; }
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
                if (error_ptr != nullptr) { *error_ptr = error; }
                return false;
            }
            if ((ch == '"') && !backslash) { in_quotes = false; }
            if ((ch == '\\') && !backslash) {
                backslash = true;
            } else {
                backslash = false;
            }
        }
    }

    if (!label.isEmpty()) { symtab->setSymbol(label, address.get_raw(), 4); }

    if (op.isEmpty()) {
        if (operands.count() != 0) {
            error = "operands for empty operation";
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            if (error_ptr != nullptr) { *error_ptr = error; }
            return false;
        }
        return true;
    }

    if (op == "#pragma") { return process_pragma(operands, filename, line_number, error_ptr); }
    if (op == "#include") {
        bool res = true;
        QString incname;
        if ((operands.count() != 1) || operands.at(0).isEmpty()) {
            error = "the single file has to be specified for include";
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            if (error_ptr != nullptr) { *error_ptr = error; }
            return false;
        }
        incname = operands.at(0);
        if (incname.at(0) == '"') { incname = incname.mid(1, incname.count() - 2); }
        QFileInfo fi(QFileInfo(filename).dir(), incname);
        incname = fi.filePath();
        include_stack.append(filename);
        if (include_stack.contains(incname)) {
            error = QString("recursive include of file: \"%1\"").arg(incname);
            res = false;
        } else {
            if (!process_file(incname, error_ptr)) {
                res = false;
                error = QString("error in included file: \"%1\"").arg(incname);
            }
        }
        if (!res) {
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            if (error_ptr != nullptr) {
                if (error_ptr->isEmpty()) { *error_ptr = error; }
            }
        }
        include_stack.removeLast();
        return res;
    }
    if ((op == ".text") || (op == ".data") || (op == ".bss") || (op == ".globl") || (op == ".end")
        || (op == ".ent") || (op == ".option")) {
        return true;
    }
    if (op == ".org") {
        bool ok;
        fixmatheval::FmeExpression expression;
        fixmatheval::FmeValue value;
        if (operands.count() != 1) {
            error = ".org unexpected number of operands";
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            if (error_ptr != nullptr) { *error_ptr = error; }
            return false;
        }
        ok = expression.parse(operands.at(0), error);
        if (!ok) {
            fatal_occured = true;
            error = tr(".orig %1 parse error.").arg(line);
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            if (error_ptr != nullptr) { *error_ptr = error; }
            return false;
        }
        ok = expression.eval(value, symtab, error, address);
        if (!ok) {
            fatal_occured = true;
            error = tr(".orig %1 evaluation error.").arg(line);
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            if (error_ptr != nullptr) { *error_ptr = error; }
            return false;
        }
        address = machine::Address(value);
        return true;
    }
    if ((op == ".space") || (op == ".skip")) {
        bool ok;
        fixmatheval::FmeExpression expression;
        fixmatheval::FmeValue value;
        fixmatheval::FmeValue fill = 0;
        if ((operands.count() != 1) && (operands.count() != 2)) {
            error = ".space/.skip unexpected number of operands";
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            if (error_ptr != nullptr) { *error_ptr = error; }
            return false;
        }
        if (operands.count() > 1) {
            ok = expression.parse(operands.at(1), error);
            if (!ok) {
                fatal_occured = true;
                error = tr(".space/.skip %1 parse error.").arg(line);
                emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
                error_occured = true;
                if (error_ptr != nullptr) { *error_ptr = error; }
                return false;
            }
            ok = expression.eval(fill, symtab, error, address);
            if (!ok) {
                fatal_occured = true;
                error = tr(".space/.skip %1 evaluation error.").arg(line);
                emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
                error_occured = true;
                if (error_ptr != nullptr) { *error_ptr = error; }
                return false;
            }
        }
        ok = expression.parse(operands.at(0), error);
        if (!ok) {
            fatal_occured = true;
            error = tr(".space/.skip %1 parse error.").arg(line);
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            if (error_ptr != nullptr) { *error_ptr = error; }
            return false;
        }
        ok = expression.eval(value, symtab, error, address);
        if (!ok) {
            fatal_occured = true;
            error = tr(".space/.skip %1 evaluation error.").arg(line);
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            if (error_ptr != nullptr) { *error_ptr = error; }
            return false;
        }
        while (value-- > 0) {
            if (!fatal_occured) { mem->write_u8(address, (uint8_t)fill, ae::INTERNAL); }
            address += 1;
        }
        return true;
    }
    if ((op == ".equ") || (op == ".set")) {
        if ((operands.count() > 2) || (operands.count() < 1)) {
            error = tr(".set or .equ incorrect arguments number.");
            emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
            error_occured = true;
            if (error_ptr != nullptr) { *error_ptr = error; }
            return false;
        }
        QString name = operands.at(0).trimmed();
        if ((name == "noat") || (name == "noreored")) { return true; }
        bool ok;
        fixmatheval::FmeValue value = 1;
        if (operands.count() > 1) {
            fixmatheval::FmeExpression expression;
            ok = expression.parse(operands.at(1), error);
            if (ok) { ok = expression.eval(value, symtab, error, address); }
            if (!ok) {
                error = tr(".set or .equ %1 parse error.").arg(operands.at(1));
                emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
                error_occured = true;
                if (error_ptr != nullptr) { *error_ptr = error; }
                return false;
            }
        }
        symtab->setSymbol(name, value, 0);
        return true;
    }
    if ((op == ".ascii") || (op == ".asciz")) {
        bool append_zero = op == ".asciz";
        for (QString s : operands) {
            if (s.count() < 2) {
                error = "ascii empty string";
                emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
                error_occured = true;
                if (error_ptr != nullptr) { *error_ptr = error; }
                return false;
            }
            if ((s.at(0) != '"') || (s.at(s.count() - 1) != '"')) {
                error = "ascii missing quotes";
                emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, error, "");
                error_occured = true;
                if (error_ptr != nullptr) { *error_ptr = error; }
                return false;
            }
            s = s.mid(1, s.count() - 2);
            for (pos = 0; pos < s.count(); pos++) {
                // target byte is in ASCII encoding
                uint8_t target_byte = 0x00;

                QChar host_char = s.at(pos);
                if (host_char == '\\') {
                    // if the host encoding recognizes this as a backslash (escape char)

                    // handle end of the string check
                    if (pos + 1 >= s.count()) {
                        error = "ascii - invalid escape sequence at the end of the string";
                        emit report_message(
                            messagetype::MSG_ERROR, filename, line_number, 0, error, "");
                        error_occured = true;
                        if (error_ptr != nullptr) { *error_ptr = error; }
                        return false;
                    }

                    // compare the host_char in host encoding but emit byte in ASCII encoding
                    host_char = s.at(++pos);
                    if (host_char == '0') {
                        target_byte = 0x00;
                    } else if (host_char == 'b') {
                        target_byte = 0x08;
                    } else if (host_char == 't') {
                        target_byte = 0x09;
                    } else if (host_char == 'n') {
                        target_byte = 0x0A;
                    } else if (host_char == 'r') {
                        target_byte = 0x0D;
                    } else if (host_char == '"') {
                        target_byte = 0x22;
                    } else if (host_char == '\\') {
                        target_byte = 0x5C;
                    } else {
                        error = QString("ascii - incorrect escape sequence '\\") + host_char + "'";
                        emit report_message(
                            messagetype::MSG_ERROR, filename, line_number, 0, error, "");
                        error_occured = true;
                        if (error_ptr != nullptr) { *error_ptr = error; }
                        return false;
                    }
                } else {
                    // otherwise convert it to ASCII and write it as-is
                    target_byte = host_char.toLatin1();
                }

                if (!fatal_occured) { mem->write_u8(address, target_byte, ae::INTERNAL); }
                address += 1;
            }
            if (append_zero) {
                if (!fatal_occured) { mem->write_u8(address, 0, ae::INTERNAL); }
                address += 1;
            }
        }
        return true;
    }
    if (op == ".byte") {
        bool ok;
        for (const QString &s : operands) {
            uint32_t val = 0;
            int chars_taken;
            val = string_to_uint64(s, 0, &chars_taken);
            if (chars_taken != s.size()) {
                fixmatheval::FmeExpression expression;
                fixmatheval::FmeValue value;
                ok = expression.parse(s, error);
                if (!ok) {
                    fatal_occured = true;
                    error = tr(".byte %1 parse error.").arg(line);
                    emit report_message(
                        messagetype::MSG_ERROR, filename, line_number, 0, error, "");
                    error_occured = true;
                    if (error_ptr != nullptr) { *error_ptr = error; }
                    return false;
                }
                ok = expression.eval(value, symtab, error, address);
                if (!ok) {
                    fatal_occured = true;
                    error = tr(".byte %1 evaluation error.").arg(line);
                    emit report_message(
                        messagetype::MSG_ERROR, filename, line_number, 0, error, "");
                    error_occured = true;
                    if (error_ptr != nullptr) { *error_ptr = error; }
                    return false;
                }
                val = (uint8_t)value;
            }
            if (!fatal_occured) { mem->write_u8(address, (uint8_t)val, ae::INTERNAL); }
            address += 1;
        }
        return true;
    }

    while (address.get_raw() & 3) {
        if (!fatal_occured) { mem->write_u8(address, 0, ae::INTERNAL); }
        address += 1;
    }

    if (op == ".word") {
        for (QString s : operands) {
            s = s.simplified();
            uint32_t val = 0;
            int chars_taken;
            val = string_to_uint64(s, 0, &chars_taken);
            if (chars_taken != s.size()) {
                val = 0;
                reloc.append(new machine::RelocExpression(
                    address, s, 0, -0xffffffff, 0xffffffff, &wordArg, filename, line_number));
            }
            if (!fatal_occured) { mem->write_u32(address, val, ae::INTERNAL); }
            address += 4;
        }
        return true;
    }

    uint32_t inst[2] = { 0, 0 };
    size_t size = 0;
    try {
        machine::TokenizedInstruction inst_tok { op, operands, address, filename,
                                                 static_cast<unsigned>(line_number) };
        size = machine::Instruction::code_from_tokens(inst, 8, inst_tok, &reloc);
    } catch (machine::Instruction::ParseError &e) {
        error = tr("instruction %1 parse error - %2.").arg(line, e.message);
        emit report_message(messagetype::MSG_ERROR, filename, line_number, 0, e.message, "");
        error_occured = true;
        if (error_ptr != nullptr) { *error_ptr = error; }
        return false;
    }
    uint32_t *p = inst;
    for (size_t l = 0; l < size; l += 4) {
        if (!fatal_occured) { mem->write_u32(address, *(p++), ae::INTERNAL); }
        address += 4;
    }
    return true;
}

bool SimpleAsm::process_file(const QString &filename, QString *error_ptr) {
    QString error;
    bool res = true;
    QFile srcfile(filename);
    if (!srcfile.open(QFile::ReadOnly | QFile::Text)) {
        error = QString("cannot open file: \"%1\"").arg(filename);
        emit report_message(messagetype::MSG_ERROR, "", 0, 0, error, "");
        error_occured = true;
        if (error_ptr != nullptr) { *error_ptr = error; }
        return false;
    }
    for (int ln = 1; !srcfile.atEnd(); ln++) {
        QString line = srcfile.readLine();
        if ((line.count() > 0) && (line.at(line.count() - 1) == '\n')) {
            line.truncate(line.count() - 1);
        }
        if (!process_line(line, filename, ln, error_ptr)) { res = false; }
    }
    srcfile.close();
    return res;
}

bool SimpleAsm::finish(QString *error_ptr) {
    bool error_reported = false;
    for (machine::RelocExpression *r : reloc) {
        QString error;
        fixmatheval::FmeExpression expression;
        if (!expression.parse(r->expression, error)) {
            error = tr("expression parse error %1 at line %2, expression %3.")
                        .arg(error, QString::number(r->line), expression.dump());
            emit report_message(messagetype::MSG_ERROR, r->filename, r->line, 0, error, "");
            if (error_ptr != nullptr && !error_reported) { *error_ptr = error; }
            error_occured = true;
            error_reported = true;
        } else {
            fixmatheval::FmeValue value;
            if (!expression.eval(value, symtab, error, r->location)) {
                error = tr("expression evalution error %1 at line %2 , "
                           "expression %3.")
                            .arg(error, QString::number(r->line), expression.dump());
                emit report_message(messagetype::MSG_ERROR, r->filename, r->line, 0, error, "");
                if (error_ptr != nullptr && !error_reported) { *error_ptr = error; }
                error_occured = true;
                error_reported = true;
            } else {
                if (false) {
                    emit report_message(
                        messagetype::MSG_INFO, r->filename, r->line, 0,
                        expression.dump() + " -> " + QString::number(value), "");
                }
                machine::Instruction inst(mem->read_u32(r->location, ae::INTERNAL));
                if (!inst.update(value, r)) {
                    error = tr("instruction update error %1 at line %2, "
                               "expression %3 -> value %4.")
                                .arg(
                                    error, QString::number(r->line), expression.dump(),
                                    QString::number(value));
                    emit report_message(messagetype::MSG_ERROR, r->filename, r->line, 0, error, "");
                    if (error_ptr != nullptr && !error_reported) { *error_ptr = error; }
                    error_occured = true;
                    error_reported = true;
                    // Remove address
                }
                if (!fatal_occured) {
                    mem->write_u32(Address(r->location), inst.data(), ae::INTERNAL);
                }
            }
        }
    }
    while (!reloc.isEmpty()) {
        delete reloc.takeFirst();
    }

    emit mem->external_change_notify(mem, Address::null(), Address(0xffffffff), ae::INTERNAL);

    return !error_occured;
}

bool SimpleAsm::process_pragma(
    QStringList &operands,
    const QString &filename,
    int line_number,
    QString *error_ptr) {
    (void)operands;
    (void)filename;
    (void)line_number;
    (void)error_ptr;
    return true;
}
