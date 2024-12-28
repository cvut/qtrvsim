#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include "common/memory_ownership.h"
#include "machine/machineconfig.h"
#include "predictor_types.h"
#include "ui_NewDialog.h"
#include "ui_NewDialogCache.h"

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

class NewDialogCacheHandler;

class NewDialog : public QDialog {
    Q_OBJECT
public:
    NewDialog(QWidget *parent, QSettings *settings);

    void switch2custom();

protected:
    void closeEvent(QCloseEvent *) override;

private slots:
    void cancel();
    void create();
    void create_empty();
    void create_example();
    void browse_elf();
    void elf_change(QString val);
    void set_preset();
    void xlen_64bit_change(bool);
    void isa_atomic_change(bool);
    void isa_multiply_change(bool);
    void pipelined_change(bool);
    void delay_slot_change(bool);
    void hazard_unit_change();
    void mem_protec_exec_change(bool);
    void mem_protec_write_change(bool);
    void mem_time_read_change(int);
    void mem_time_write_change(int);
    void mem_enable_burst_change(bool);
    void mem_time_burst_change(int);
    void mem_time_level2_change(int);
    void osemu_enable_change(bool);
    void osemu_known_syscall_stop_change(bool);
    void osemu_unknown_syscall_stop_change(bool);
    void osemu_interrupt_stop_change(bool);
    void osemu_exception_stop_change(bool);
    void browse_osemu_fs_root();
    void osemu_fs_root_change(QString val);
    void reset_at_compile_change(bool);
    void switch2page(QTreeWidgetItem *current, QTreeWidgetItem *previous = nullptr);

    // Branch Predictor
    void bp_toggle_widgets();
    void bp_enabled_change(bool);
    void bp_type_change(void);
    void bp_init_state_change(void);
    void bp_btb_addr_bits_change(int);
    void bp_bht_bhr_bits_change(int);
    void bp_bht_addr_bits_change(int);

private:
    Box<Ui::NewDialog> ui {};
    Box<Ui::NewDialogCache> ui_cache_p {}, ui_cache_d {}, ui_cache_l2 {};
    QSettings *settings;

    Box<machine::MachineConfig> config;
    void config_gui(); // Apply configuration to gui
    void bp_bht_bits_texts_update(void);

    unsigned preset_number();
    void load_settings();
    void store_settings();
    NewDialogCacheHandler *cache_handler_p {}, *cache_handler_d {}, *cache_handler_l2 {};
};

class NewDialogCacheHandler : public QObject {
    Q_OBJECT

    using Super = QObject;

public:
    NewDialogCacheHandler(NewDialog *nd, Ui::NewDialogCache *ui);

    void set_config(machine::CacheConfig *cache_config);

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
    Ui::NewDialogCache *ui {};
    machine::CacheConfig *config;
};

#endif // NEWDIALOG_H
