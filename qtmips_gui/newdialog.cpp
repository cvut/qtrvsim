#include "newdialog.h"
#include "mainwindow.h"

NewDialog::NewDialog(QWidget *parent, QSettings *settings) : QDialog(parent) {
    ui = new Ui::NewDialog();
    ui->setupUi(this);
    setWindowTitle("New machine");

    this->settings = settings;

    QObject::connect(ui->pushButton_load, SIGNAL(clicked(bool)), this, SLOT(create()));
    QObject::connect(ui->pushButton_cancel, SIGNAL(clicked(bool)), this, SLOT(cancel()));
    // Signals on Base tab
    QObject::connect(ui->preset_no_pipeline, SIGNAL(toggled(bool)), this, SLOT(preset(bool)));
    QObject::connect(ui->preset_pipelined, SIGNAL(toggled(bool)), this, SLOT(preset(bool)));
    QObject::connect(ui->pushButton_browse, SIGNAL(clicked(bool)), this, SLOT(browse_elf()));
#define CUSTOM_PRESET(UI)  QObject::connect(UI, SIGNAL(clicked(bool)), this, SLOT(set_custom_preset()))
    // Signals on Core tab
    CUSTOM_PRESET(ui->pipelined);
    CUSTOM_PRESET(ui->hazard);
    // Signals on Memory tab
    CUSTOM_PRESET(ui->mem_protec_write);
    CUSTOM_PRESET(ui->mem_protec_exec);
    CUSTOM_PRESET(ui->cache);
    CUSTOM_PRESET(ui->cache_associative);
#undef CUSTOM_PRESET

    // Load setting after signals are configured so that we can have correct settings
    load_settings();
}

NewDialog::~NewDialog() {
    delete ui;
    // Settings is freed by parent
}

void NewDialog::cancel() {
    this->close();
}

void NewDialog::create() {
    MainWindow *prnt = (MainWindow*)parent();

    machine::MachineConfig mc;
    mc.set_elf(ui->elf_file->text());
    mc.set_pipelined(ui->pipelined->isChecked());
    // TODO other settings

    try {
        prnt->create_core(mc);
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
    if (elf_dialog.exec())
        ui->elf_file->setText(elf_dialog.selectedFiles()[0]);
}

void NewDialog::preset(bool value) {
    if (value) {
        bool pip = ui->preset_pipelined->isChecked();
        // Core settings
        ui->pipelined->setChecked(pip);
        ui->hazard->setChecked(pip);
        // Memory settings
        ui->mem_protec_write->setChecked(true);
        ui->mem_protec_exec->setChecked(true);
        ui->cache->setChecked(pip);
        ui->cache_associative->setChecked(true);
    } // Else custom so do no changes
}

void NewDialog::set_custom_preset() {
    ui->preset_custom->setChecked(true);
}

void NewDialog::closeEvent(QCloseEvent *) {
    load_settings(); // Reset from settings
    // Close main window if not already configured
    MainWindow *prnt = (MainWindow*)parent();
    if (!prnt->configured())
        prnt->close();
}

#define LOAD_BUTTON(NAME, DEF) ui->NAME->setChecked(settings->value(#NAME, DEF).toBool())
#define LOAD_LINE(NAME, DEF) ui->NAME->setText(settings->value(#NAME, DEF).toString())

#define STORE_BUTTON(NAME) settings->setValue(#NAME, ui->NAME->isChecked())
#define STORE_LINE(NAME) settings->setValue(#NAME, ui->NAME->text())

void NewDialog::load_settings() {
    // Core tab
    LOAD_BUTTON(pipelined, false);
    LOAD_BUTTON(hazard, false);
    // Memory tab
    LOAD_BUTTON(mem_protec_write, true);
    LOAD_BUTTON(mem_protec_exec, true);
    LOAD_BUTTON(cache, false);
    LOAD_BUTTON(cache_associative, true);
    // Base tab
    // We are doing this last so presets can reset previous configuration to somethin valid
    LOAD_BUTTON(preset_no_pipeline, true);
    LOAD_BUTTON(preset_pipelined, false);
    LOAD_BUTTON(preset_custom, false);
    LOAD_LINE(elf_file, "");
}

void NewDialog::store_settings() {
    // Core tab
    STORE_BUTTON(pipelined);
    STORE_BUTTON(hazard);
    // Memory tab
    STORE_BUTTON(mem_protec_write);
    STORE_BUTTON(mem_protec_exec);
    STORE_BUTTON(cache);
    STORE_BUTTON(cache_associative);
    // Base tab
    STORE_BUTTON(preset_no_pipeline);
    STORE_BUTTON(preset_pipelined);
    STORE_BUTTON(preset_custom);
    STORE_LINE(elf_file);
}
