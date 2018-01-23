#include "newdialog.h"
#include "mainwindow.h"
#include "qtmipsexception.h"

NewDialog::NewDialog(QWidget *parent, QSettings *settings) : QDialog(parent) {
    setWindowTitle("New machine");

    this->settings = settings;
    config = nullptr;

    ui = new Ui::NewDialog();
    ui->setupUi(this);
    ui_cache_p = new Ui::NewDialogCache();
    ui_cache_p->setupUi(ui->tab_cache_program);
    ui_cache_p->writeback_policy->hide();
    ui_cache_p->label_writeback->hide();
    ui_cache_d = new Ui::NewDialogCache();
    ui_cache_d->setupUi(ui->tab_cache_data);

    connect(ui->pushButton_load, SIGNAL(clicked(bool)), this, SLOT(create()));
    connect(ui->pushButton_cancel, SIGNAL(clicked(bool)), this, SLOT(cancel()));
    connect(ui->pushButton_browse, SIGNAL(clicked(bool)), this, SLOT(browse_elf()));
    //connect(ui->preset_box, SIGNAL(clicked(bool)), this, SLOT(set_preset()));
    connect(ui->preset_no_pipeline, SIGNAL(toggled(bool)), this, SLOT(set_preset()));
    connect(ui->preset_pipelined, SIGNAL(toggled(bool)), this, SLOT(set_preset()));

    connect(ui->pipelined, SIGNAL(clicked(bool)), this, SLOT(pipelined_change(bool)));
    connect(ui->delay_slot, SIGNAL(clicked(bool)), this, SLOT(delay_slot_change(bool)));
    connect(ui->hazard_unit, SIGNAL(clicked(bool)), this, SLOT(hazard_unit_change()));
    connect(ui->hazard_stall, SIGNAL(clicked(bool)), this, SLOT(hazard_unit_change()));
    connect(ui->hazard_stall_forward, SIGNAL(clicked(bool)), this, SLOT(hazard_unit_change()));
    connect(ui->mem_protec_exec, SIGNAL(clicked(bool)), this, SLOT(mem_protec_exec_change(bool)));
    connect(ui->mem_protec_write, SIGNAL(clicked(bool)), this, SLOT(mem_protec_write_change(bool)));

    connect(ui_cache_p->enabled, SIGNAL(clicked(bool)), this, SLOT(cache_program_change(bool)));
    connect(ui_cache_d->enabled, SIGNAL(clicked(bool)), this, SLOT(cache_data_change(bool)));

    load_settings(); // Also configures gui
}

NewDialog::~NewDialog() {
    delete ui_cache_d;
    delete ui_cache_p;
    delete ui;
    // Settings is freed by parent
    delete config;
}

void NewDialog::closeEvent(QCloseEvent *) {
    load_settings(); // Reset from settings
    // Close main window if not already configured
    MainWindow *prnt = (MainWindow*)parent();
    if (!prnt->configured())
        prnt->close();
}

void NewDialog::cancel() {
    this->close();
}

void NewDialog::create() {
    MainWindow *prnt = (MainWindow*)parent();

    try {
        prnt->create_core(*config);
    } catch (const machine::QtMipsExceptionInput &e) {
        QMessageBox msg(this);
        msg.setText(e.msg(false));
        msg.setIcon(QMessageBox::Critical);
        msg.setToolTip("Please check that ELF executable really exists and is in correct format.");
        msg.setDetailedText(e.msg(true));
        msg.setWindowTitle("Error while initializing new machine");
        msg.exec();
        return;
    }

    store_settings(); // Save to settings
    this->close();
}

void NewDialog::browse_elf() {
    QFileDialog elf_dialog(this);
    elf_dialog.setFileMode(QFileDialog::ExistingFile);
    if (elf_dialog.exec()) {
        QString path = elf_dialog.selectedFiles()[0];
        ui->elf_file->setText(path);
        config->set_elf(path);
    }
    // Elf shouldn't have any other effect so we skip config_gui here
}

void NewDialog::set_preset() {
    if (ui->preset_no_pipeline->isChecked())
        config->preset(machine::CP_SINGLE);
    else if (ui->preset_pipelined->isChecked())
        config->preset(machine::CP_PIPE_WITH_CACHE);
    else
        // Skip apply configuration as we changed nothing.
        return;
    config_gui();
}

// Common end section of *_change slots
#define CHANGE_COMMON do { \
        ui->preset_custom->setChecked(true); \
        config_gui(); \
    } while(false)

void NewDialog::pipelined_change(bool val) {
    config->set_pipelined(val);
    CHANGE_COMMON;
}

void NewDialog::delay_slot_change(bool val) {
    config->set_delay_slot(val);
    CHANGE_COMMON;
}

void NewDialog::hazard_unit_change() {
    if (ui->hazard_unit->isChecked())
        config->set_hazard_unit(ui->hazard_stall->isChecked() ? machine::MachineConfig::HU_STALL : machine::MachineConfig::HU_STALL_FORWARD);
    else
        config->set_hazard_unit(machine::MachineConfig::HU_NONE);
    CHANGE_COMMON;
}

void NewDialog::mem_protec_exec_change(bool v) {
    config->set_memory_execute_protection(v);
    CHANGE_COMMON;
}

void NewDialog::mem_protec_write_change(bool v) {
    config->set_memory_write_protection(v);
    CHANGE_COMMON;
}

void NewDialog::cache_data_change(bool v) {
    config->access_cache_data()->set_enabled(v);
    CHANGE_COMMON;
}

void NewDialog::cache_program_change(bool v) {
    config->access_cache_program()->set_enabled(v);
    CHANGE_COMMON;
}

void NewDialog::config_gui() {
    // Set values
    ui->elf_file->setText(config->elf());
    ui->pipelined->setChecked(config->pipelined());
    ui->delay_slot->setChecked(config->delay_slot());
    ui->hazard_unit->setChecked(config->hazard_unit() != machine::MachineConfig::HU_NONE);
    ui->hazard_stall->setChecked(config->hazard_unit() == machine::MachineConfig::HU_STALL);
    ui->hazard_stall_forward->setChecked(config->hazard_unit() == machine::MachineConfig::HU_STALL_FORWARD);
    ui->mem_protec_exec->setChecked(config->memory_execute_protection());
    ui->mem_protec_write->setChecked(config->memory_write_protection());
    ui->mem_time_read->setValue(config->memory_access_time_read());
    ui->mem_time_write->setValue(config->memory_access_time_write());
    ui_cache_p->enabled->setChecked(config->cache_program().enabled());
    ui_cache_d->enabled->setChecked(config->cache_data().enabled());
    // Disable various sections according to configuration
    ui->delay_slot->setEnabled(!config->pipelined());
    ui->hazard_unit->setEnabled(config->pipelined());
}

void NewDialog::load_settings() {
    if (config != nullptr)
        delete config;

    // Load config
    config = new machine::MachineConfig(settings);

    // Load preset
    unsigned preset = settings->value("Preset", 1).toUInt();
    if (preset != 0) {
        enum machine::ConfigPresets p = (enum machine::ConfigPresets)(preset - 1);
        config->preset(p);
        switch (p) {
        case machine::CP_SINGLE:
            ui->preset_no_pipeline->setChecked(true);
            break;
        case machine::CP_PIPE_WITH_CACHE:
            ui->preset_pipelined->setChecked(true);
            break;
        }
    } else
        ui->preset_custom->setChecked(true);

    config_gui();
}

void NewDialog::store_settings() {
    config->store(settings);

    if (ui->preset_custom->isChecked())
        settings->setValue("Preset", 0);
    else
        settings->setValue("Preset", ui->preset_no_pipeline->isChecked() ? machine::CP_SINGLE + 1 : machine::CP_PIPE_WITH_CACHE + 1);
}
