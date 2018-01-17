#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_NewDialog.h"
#include "ui_NewDialogCache.h"
#include "machineconfig.h"

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
    void set_preset();
    void pipelined_change(bool);
    void delay_slot_change(bool);
    void hazard_unit_change();
    void mem_protec_exec_change(bool);
    void mem_protec_write_change(bool);

    void cache_data_change(bool);
    void cache_program_change(bool);

private:
    Ui::NewDialog *ui;
    Ui::NewDialogCache *ui_cache_p, *ui_cache_d;
    QSettings *settings;

    machine::MachineConfig *config;
    void config_gui(); // Apply configuration to gui

    void load_settings();
    void store_settings();
};

#endif // NEWDIALOG_H
