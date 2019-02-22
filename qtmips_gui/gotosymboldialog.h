#ifndef GOTOSYMBOLDIALOG_H
#define GOTOSYMBOLDIALOG_H

#include <QDialog>
#include <QList>
#include <QStringList>

namespace Ui {
class GoToSymbolDialog;
}

class GoToSymbolDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GoToSymbolDialog(QWidget *parent, QStringList &symlist);
    ~GoToSymbolDialog();
signals:
    void program_focus_addr(std::uint32_t);
    void memory_focus_addr(std::uint32_t);
    bool obtain_value_for_name(std::uint32_t &value, QString name) const;
public slots:
    void show_prog();
    void show_mem();
private:
    Ui::GoToSymbolDialog *ui;
};

#endif // GOTOSYMBOLDIALOG_H
