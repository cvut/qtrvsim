// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

#include "mainwindow.h"
#include "aboutdialog.h"
#include "ossyscall.h"
#include "fontsize.h"
#include "gotosymboldialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    machine = nullptr;
    corescene = nullptr;
    settings = new QSettings("CTU", "QtMips");

    ui = new Ui::MainWindow();
    ui->setupUi(this);
    setWindowTitle("QtMips");

    // Prepare empty core view
    coreview  = new GraphicsView(this);
    this->setCentralWidget(coreview);
    // Create/prepare other widgets
    ndialog = new NewDialog(this, settings);
    registers = new RegistersDock(this);
    registers->hide();
    program = new ProgramDock(this, settings);
    program->hide();
    memory = new MemoryDock(this, settings);
    memory->hide();
    cache_program = new CacheDock(this, "Program");
    cache_program->hide();
    cache_data = new CacheDock(this, "Data");
    cache_data->hide();
    peripherals = new PeripheralsDock(this, settings);
    peripherals->hide();
    terminal = new TerminalDock(this, settings);
    terminal->hide();
    cop0dock = new Cop0Dock(this);
    cop0dock->hide();

    // Execution speed actions
    speed_group = new QActionGroup(this);
    speed_group->addAction(ui->ips1);
    speed_group->addAction(ui->ips2);
    speed_group->addAction(ui->ips5);
    speed_group->addAction(ui->ips10);
    speed_group->addAction(ui->ipsUnlimited);
    speed_group->addAction(ui->ipsMax);
    ui->ips1->setChecked(true);

    // Connect signals from menu
    connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(new_machine()));
    connect(ui->actionReload, SIGNAL(triggered(bool)), this, SLOT(machine_reload()));
    connect(ui->actionShow_Symbol, SIGNAL(triggered(bool)), this, SLOT(show_symbol_dialog()));
    connect(ui->actionRegisters, SIGNAL(triggered(bool)), this, SLOT(show_registers()));
    connect(ui->actionProgram_memory, SIGNAL(triggered(bool)), this, SLOT(show_program()));
    connect(ui->actionMemory, SIGNAL(triggered(bool)), this, SLOT(show_memory()));
    connect(ui->actionProgram_Cache, SIGNAL(triggered(bool)), this, SLOT(show_cache_program()));
    connect(ui->actionData_Cache, SIGNAL(triggered(bool)), this, SLOT(show_cache_data()));
    connect(ui->actionPeripherals, SIGNAL(triggered(bool)), this, SLOT(show_peripherals()));
    connect(ui->actionTerminal, SIGNAL(triggered(bool)), this, SLOT(show_terminal()));
    connect(ui->actionCop0State, SIGNAL(triggered(bool)), this, SLOT(show_cop0dock()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(about_qtmips()));
    connect(ui->actionAboutQt, SIGNAL(triggered(bool)), this, SLOT(about_qt()));
    connect(ui->ips1, SIGNAL(toggled(bool)), this, SLOT(set_speed()));
    connect(ui->ips2, SIGNAL(toggled(bool)), this, SLOT(set_speed()));
    connect(ui->ips5, SIGNAL(toggled(bool)), this, SLOT(set_speed()));
    connect(ui->ips10, SIGNAL(toggled(bool)), this, SLOT(set_speed()));
    connect(ui->ipsUnlimited, SIGNAL(toggled(bool)), this, SLOT(set_speed()));
    connect(ui->ipsMax, SIGNAL(toggled(bool)), this, SLOT(set_speed()));

    // Restore application state from settings
    restoreState(settings->value("windowState").toByteArray());
    restoreGeometry(settings->value("windowGeometry").toByteArray());
}

MainWindow::~MainWindow() {
    if (corescene != nullptr)
        delete corescene;
    if (coreview != nullptr)
        delete coreview;
    delete ndialog;
    delete registers;
    delete program;
    delete memory;
    delete cache_program;
    delete cache_data;
    delete peripherals;
    delete terminal;
    delete ui;
    if (machine != nullptr)
        delete machine;
    settings->sync();
    delete settings;
}

void MainWindow::start() {
    this->show();
    ndialog->show();
}

void MainWindow::create_core(const machine::MachineConfig &config) {
    // Create machine
    machine::QtMipsMachine *new_machine = new machine::QtMipsMachine(&config, true);

    // Remove old machine
    if (machine != nullptr)
        delete machine;
    machine = new_machine;

    // Create machine view
    if (corescene != nullptr)
        delete corescene;
    if (config.pipelined()) {
        corescene = new CoreViewScenePipelined(machine);
    } else {
        corescene = new CoreViewSceneSimple(machine);
    }
    coreview->setScene(corescene);

    set_speed(); // Update machine speed to current settings

#if 1
    osemu::OsSyscallExceptionHandler *osemu_handler = new osemu::OsSyscallExceptionHandler;
    machine->register_exception_handler(machine::EXCAUSE_SYSCALL, osemu_handler);
    connect(osemu_handler, SIGNAL(char_written(int,uint)), terminal, SLOT(tx_byte(int,uint)));
    connect(osemu_handler, SIGNAL(rx_byte_pool(int,uint&,bool&)),
            terminal, SLOT(rx_byte_pool(int,uint&,bool&)));
#endif

    // Connect machine signals and slots
    connect(ui->actionRun, SIGNAL(triggered(bool)), machine, SLOT(play()));
    connect(ui->actionPause, SIGNAL(triggered(bool)), machine, SLOT(pause()));
    connect(ui->actionStep, SIGNAL(triggered(bool)), machine, SLOT(step()));
    connect(ui->actionRestart, SIGNAL(triggered(bool)), machine, SLOT(restart()));
    connect(machine, SIGNAL(status_change(machine::QtMipsMachine::Status)), this, SLOT(machine_status(machine::QtMipsMachine::Status)));
    connect(machine, SIGNAL(program_exit()), this, SLOT(machine_exit()));
    connect(machine, SIGNAL(program_trap(machine::QtMipsException&)), this, SLOT(machine_trap(machine::QtMipsException&)));
    // Connect scene signals to actions
    connect(corescene, SIGNAL(request_registers()), this, SLOT(show_registers()));
    connect(corescene, SIGNAL(request_program_memory()), this, SLOT(show_program()));
    connect(corescene, SIGNAL(request_data_memory()), this, SLOT(show_memory()));
    connect(corescene, SIGNAL(request_jump_to_program_counter(std::uint32_t)), program, SIGNAL(jump_to_pc(std::uint32_t)));
    connect(corescene, SIGNAL(request_cache_program()), this, SLOT(show_cache_program()));
    connect(corescene, SIGNAL(request_cache_data()), this, SLOT(show_cache_data()));
    connect(corescene, SIGNAL(request_peripherals()), this, SLOT(show_peripherals()));
    connect(corescene, SIGNAL(request_terminal()), this, SLOT(show_terminal()));
    // Connect signal from break to machine pause
    connect(machine->core(), SIGNAL(stop_on_exception_reached()), machine, SLOT(pause()));


    // Setup docks
    registers->setup(machine);
    program->setup(machine);
    memory->setup(machine);
    cache_program->setup(machine->cache_program());
    cache_data->setup(machine->cache_data());
    terminal->setup(machine->serial_port());
    peripherals->setup(machine->peripheral_spi_led());
    cop0dock->setup(machine);

    // Connect signals for instruction address followup
    connect(machine->core(), SIGNAL(fetch_inst_addr_value(std::uint32_t)),
            program, SLOT(fetch_inst_addr(std::uint32_t)));
    connect(machine->core(), SIGNAL(decode_inst_addr_value(std::uint32_t)),
            program, SLOT(decode_inst_addr(std::uint32_t)));
    connect(machine->core(), SIGNAL(execute_inst_addr_value(std::uint32_t)),
            program, SLOT(execute_inst_addr(std::uint32_t)));
    connect(machine->core(), SIGNAL(memory_inst_addr_value(std::uint32_t)),
            program, SLOT(memory_inst_addr(std::uint32_t)));
    connect(machine->core(), SIGNAL(writeback_inst_addr_value(std::uint32_t)),
            program, SLOT(writeback_inst_addr(std::uint32_t)));

    // Set status to ready
    machine_status(machine::QtMipsMachine::ST_READY);
}

bool MainWindow::configured() {
    return (machine != nullptr);
}

void MainWindow::new_machine() {
    ndialog->show();
}

void MainWindow::machine_reload() {
    machine::MachineConfig cnf(&machine->config()); // We have to make local copy as create_core will delete current machine
    try {
        create_core(cnf);
    } catch (const machine::QtMipsExceptionInput &e) {
        QMessageBox msg(this);
        msg.setText(e.msg(false));
        msg.setIcon(QMessageBox::Critical);
        msg.setDetailedText(e.msg(true));
        msg.setWindowTitle("Error while initializing new machine");
        msg.exec();
    }
}

#define SHOW_HANDLER(NAME)  void MainWindow::show_##NAME() { \
        show_dockwidget(NAME); \
    } \

SHOW_HANDLER(registers)
SHOW_HANDLER(program)
SHOW_HANDLER(memory)
SHOW_HANDLER(cache_program)
SHOW_HANDLER(cache_data)
SHOW_HANDLER(peripherals)
SHOW_HANDLER(terminal)
SHOW_HANDLER(cop0dock)
#undef SHOW_HANDLER

void MainWindow::show_symbol_dialog(){
    if (machine == nullptr || machine->symbol_table() == nullptr)
        return;
    QStringList *symnames = machine->symbol_table()->names();
    GoToSymbolDialog *gotosyboldialog = new GoToSymbolDialog(this, *symnames);
    connect(gotosyboldialog, SIGNAL(program_focus_addr(std::uint32_t)),
            program, SIGNAL(focus_addr(std::uint32_t)));
    connect(gotosyboldialog, SIGNAL(memory_focus_addr(std::uint32_t)),
            memory, SIGNAL(focus_addr(std::uint32_t)));
    connect(gotosyboldialog, SIGNAL(obtain_value_for_name(std::uint32_t&,QString)),
            machine->symbol_table(), SLOT(name_to_value(std::uint32_t&,QString)));
    gotosyboldialog->exec();
    delete symnames;
}

void MainWindow::about_qtmips()
{
    AboutDialog *aboutdialog = new AboutDialog(this);
    aboutdialog->exec();
}

void MainWindow::about_qt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::set_speed() {
    if (machine == nullptr)
        return; // just ignore

    if (ui->ips1->isChecked())
        machine->set_speed(1000);
    else if (ui->ips2->isChecked())
        machine->set_speed(500);
    else if (ui->ips5->isChecked())
        machine->set_speed(200);
    else if (ui->ips10->isChecked())
        machine->set_speed(100);
    else if (ui->ipsMax->isChecked())
        machine->set_speed(0, 100);
    else
        machine->set_speed(0);
}

void MainWindow::closeEvent(QCloseEvent *event __attribute__((unused))) {
    settings->setValue("windowGeometry", saveGeometry());
    settings->setValue("windowState", saveState());
    settings->sync();
}

void MainWindow::show_dockwidget(QDockWidget *dw) {
    if (dw->isHidden()) {
        dw->show();
        addDockWidget(Qt::RightDockWidgetArea, dw);
    } else {
        dw->raise();
        dw->setFocus();
    }
}

void MainWindow::machine_status(enum machine::QtMipsMachine::Status st) {
    QString status;
    switch (st) {
    case machine::QtMipsMachine::ST_READY:
        ui->actionPause->setEnabled(false);
        ui->actionRun->setEnabled(true);
        ui->actionStep->setEnabled(true);
        status = "Ready";
        break;
    case machine::QtMipsMachine::ST_RUNNING:
        ui->actionPause->setEnabled(true);
        ui->actionRun->setEnabled(false);
        ui->actionStep->setEnabled(false);
        status = "Running";
        break;
    case machine::QtMipsMachine::ST_BUSY:
        // Busy is not interesting (in such case we should just be running
        return;
    case machine::QtMipsMachine::ST_EXIT:
        // machine_exit is called so we disable controls in that
        status = "Exited";
        break;
    case machine::QtMipsMachine::ST_TRAPPED:
        // machine_trap is called so we disable controls in that
        status = "Trapped";
        break;
    default:
        status = "Unknown";
        break;
    }
    ui->statusBar->showMessage(status);
}

void MainWindow::machine_exit() {
    ui->actionPause->setEnabled(false);
    ui->actionRun->setEnabled(false);
    ui->actionStep->setEnabled(false);
}

void MainWindow::machine_trap(machine::QtMipsException &e) {
    machine_exit();

    QMessageBox msg(this);
    msg.setText(e.msg(false));
    msg.setIcon(QMessageBox::Critical);
    msg.setDetailedText(e.msg(true));
    msg.setWindowTitle("Machine trapped");
    msg.exec();
}
