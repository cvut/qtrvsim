#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_NewDialog.h"
#include "qtmipsexception.h"

class NewDialog : public QDialog {
    Q_OBJECT
public:
    NewDialog(QWidget *parent, QSettings *settings);
    ~NewDialog();

public slots:
    void cancel();
    void create();
    void browse_elf();
    void preset(bool);
    void set_custom_preset();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::NewDialog *ui;
    QSettings *settings;
    QFileDialog *elf_dialog;

    void load_settings();
    void store_settings();
};

#endif // NEWDIALOG_H
