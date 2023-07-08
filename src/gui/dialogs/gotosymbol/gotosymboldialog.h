#ifndef GOTOSYMBOLDIALOG_H
#define GOTOSYMBOLDIALOG_H

#include "common/memory_ownership.h"
#include "machine/memory/address.h"
#include "ui_gotosymboldialog.h"

#include <QDialog>
#include <QList>
#include <QStringList>

class GoToSymbolDialog : public QDialog {
    Q_OBJECT

public:
    GoToSymbolDialog(QWidget *parent, const QStringList &symbol_names);
signals:
    void program_focus_addr(machine::Address);
    void memory_focus_addr(machine::Address);
    bool obtain_value_for_name(uint64_t &value, const QString &name) const;
public slots:
    void show_prog();
    void show_mem();

private:
    Box<Ui::GoToSymbolDialog> ui {};
};

#endif // GOTOSYMBOLDIALOG_H
