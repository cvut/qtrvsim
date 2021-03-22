#ifndef GOTOSYMBOLDIALOG_H
#define GOTOSYMBOLDIALOG_H

#include "qtmips_machine/memory/address.h"

#include <QDialog>
#include <QList>
#include <QStringList>

namespace Ui {
class GoToSymbolDialog;
}

class GoToSymbolDialog : public QDialog {
    Q_OBJECT

public:
    GoToSymbolDialog(QWidget *parent, const QStringList &symbol_names);
    ~GoToSymbolDialog() override;
signals:
    void program_focus_addr(machine::Address);
    void memory_focus_addr(machine::Address);
    bool obtain_value_for_name(uint64_t &value, const QString &name) const;
public slots:
    void show_prog();
    void show_mem();

private:
    Ui::GoToSymbolDialog *ui {};
};

#endif // GOTOSYMBOLDIALOG_H
