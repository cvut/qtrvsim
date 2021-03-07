#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "utils.h"

#include <QMap>
#include <QMultiMap>
#include <QObject>
#include <QString>
#include <QStringList>

namespace machine {

using SymbolValue = uint64_t;
using SymbolSize = uint32_t;
using SymbolInfo = unsigned char;
using SymbolOther = unsigned char;

struct SymbolTableEntry {
    SymbolTableEntry(
        QString name,
        SymbolValue value,
        SymbolSize size,
        SymbolInfo info = 0,
        SymbolOther other = 0);

    const QString name;
    const SymbolValue value;
    const SymbolSize size;
    const SymbolInfo info;
    const SymbolOther other;
};

/**
 * ELF symbol table.
 *
 * Used tu jump to symbol and for integrated assembler.
 * To learn more, about ELF symbol table internal, I recommend this
 * [link](https://blogs.oracle.com/solaris/inside-elf-symbol-tables-v2).
 */
class SymbolTable : public QObject {
    Q_OBJECT
public:
    explicit SymbolTable(QObject *parent = nullptr);
    ~SymbolTable() override;

    void add_symbol(
        const QString &name,
        SymbolValue value,
        SymbolSize size,
        SymbolInfo info = 0,
        SymbolOther other = 0);

    void set_symbol(
        const QString &name,
        SymbolValue value,
        SymbolSize size,
        SymbolInfo info = 0,
        SymbolOther other = 0);

    void remove_symbol(const QString &name);

    QStringList names() const;
public slots:
    bool name_to_value(SymbolValue &value, const QString &name) const;
    /**
     * FIXME: This is design is flawed. The table can contain multiple names for
     * single location as it is multimap.
     */
    bool location_to_name(QString &name, SymbolValue value) const;

private:
    // QString cannot be made const, because it would not fit into QT gui API.
    QMap<QString, OWNED SymbolTableEntry *> map_name_to_symbol;
    QMultiMap<SymbolValue, SymbolTableEntry *> map_value_to_symbol;
};

} // namespace machine

#endif // SYMBOLTABLE_H
