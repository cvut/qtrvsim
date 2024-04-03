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
        machine::Address address,
        machine::Xlen xlen);
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
    machine::Address address {};

private:
    QStringList include_stack;
    machine::FrontendMemory *mem {};
    machine::RelocExpressionList reloc;
};

#endif /*SIMPLEASM_H*/
