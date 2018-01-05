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

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void cancel();
    void create();
    void browse_elf();
    void preset(bool);
    void set_custom_preset();

private:
    Ui::NewDialog *ui;
    QSettings *settings;

    void load_settings();
    void store_settings();
};

#endif // NEWDIALOG_H
