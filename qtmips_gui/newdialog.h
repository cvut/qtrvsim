#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_NewDialog.h"
#include "ui_NewDialogCache.h"
#include "machineconfig.h"

class NewDialogCacheHandler;

class NewDialog : public QDialog {
    Q_OBJECT
public:
    NewDialog(QWidget *parent, QSettings *settings);
    ~NewDialog();

	void switch2custom();

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

private:
    Ui::NewDialog *ui;
    Ui::NewDialogCache *ui_cache_p, *ui_cache_d;
    QSettings *settings;

    machine::MachineConfig *config;
    void config_gui(); // Apply configuration to gui

    unsigned preset_number();
    void load_settings();
    void store_settings();
	NewDialogCacheHandler *cache_handler_p, *cache_handler_d;
};

class NewDialogCacheHandler : QObject {
	Q_OBJECT
public:
	NewDialogCacheHandler(NewDialog *nd, Ui::NewDialogCache *ui);

    void set_config(machine::MachineConfigCache *config);

    void config_gui();

private slots:
	void enabled(bool);
	void numsets();
	void blocksize();
	void degreeassociativity();
	void replacement(int);
	void writeback(int);

private:
	NewDialog *nd;
	Ui::NewDialogCache *ui;
	machine::MachineConfigCache *config;
};

#endif // NEWDIALOG_H
