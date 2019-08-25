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

#include <QtWidgets>
#ifdef QTMIPS_WITH_PRINTING
#include <QPrinter>
#include <QPrintDialog>
#endif
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextDocument>
#include <QMetaObject>
#include <iostream>

#include "mainwindow.h"
#include "aboutdialog.h"
#include "ossyscall.h"
#include "fontsize.h"
#include "gotosymboldialog.h"
#include "fixmatheval.h"
#include "simpleasm.h"
#include "extprocess.h"
#include "savechangeddialog.h"
#include "textsignalaction.h"

#ifdef __EMSCRIPTEN__
#include <QFileInfo>
#include "qhtml5file.h"
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    ignore_unsaved = false;
    machine = nullptr;
    corescene = nullptr;
    current_srceditor = nullptr;
    coreview_shown = true;
    settings = new QSettings("CTU", "QtMips");

    ui = new Ui::MainWindow();
    ui->setupUi(this);
    setWindowTitle("QtMips");

    central_window = new QTabWidget(this);
    this->setCentralWidget(central_window);

    // Prepare empty core view
    coreview  = new GraphicsView(this);
    central_window->addTab(coreview, "Core");
    // Create/prepare other widgets
    ndialog = new NewDialog(this, settings);
    registers = new RegistersDock(this);
    registers->hide();
    program = new ProgramDock(this, settings);
    addDockWidget(Qt::LeftDockWidgetArea, program);
    program->show();
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
    lcd_display = new LcdDisplayDock(this, settings);
    lcd_display->hide();
    cop0dock = new Cop0Dock(this);
    cop0dock->hide();
    messages = new MessagesDock(this, settings);
    messages->hide();

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
    connect(ui->actionNewMachine, SIGNAL(triggered(bool)), this, SLOT(new_machine()));
    connect(ui->actionReload, SIGNAL(triggered(bool)), this, SLOT(machine_reload()));
    connect(ui->actionPrint, SIGNAL(triggered(bool)), this, SLOT(print_action()));
    connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(new_source()));
    connect(ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(open_source()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(save_source()));
    connect(ui->actionSaveAs, SIGNAL(triggered(bool)), this, SLOT(save_source_as()));
    connect(ui->actionClose, SIGNAL(triggered(bool)), this, SLOT(close_source_check()));
    connect(ui->actionMnemonicRegisters, SIGNAL(triggered(bool)), this, SLOT(view_mnemonics_registers(bool)));
    connect(ui->actionCompileSource, SIGNAL(triggered(bool)), this, SLOT(compile_source()));
    connect(ui->actionBuildExe, SIGNAL(triggered(bool)), this, SLOT(build_execute()));
    connect(ui->actionShow_Symbol, SIGNAL(triggered(bool)), this, SLOT(show_symbol_dialog()));
    connect(ui->actionRegisters, SIGNAL(triggered(bool)), this, SLOT(show_registers()));
    connect(ui->actionProgram_memory, SIGNAL(triggered(bool)), this, SLOT(show_program()));
    connect(ui->actionMemory, SIGNAL(triggered(bool)), this, SLOT(show_memory()));
    connect(ui->actionProgram_Cache, SIGNAL(triggered(bool)), this, SLOT(show_cache_program()));
    connect(ui->actionData_Cache, SIGNAL(triggered(bool)), this, SLOT(show_cache_data()));
    connect(ui->actionPeripherals, SIGNAL(triggered(bool)), this, SLOT(show_peripherals()));
    connect(ui->actionTerminal, SIGNAL(triggered(bool)), this, SLOT(show_terminal()));
    connect(ui->actionLcdDisplay, SIGNAL(triggered(bool)), this, SLOT(show_lcd_display()));
    connect(ui->actionCop0State, SIGNAL(triggered(bool)), this, SLOT(show_cop0dock()));
    connect(ui->actionCore_View_show, SIGNAL(triggered(bool)), this, SLOT(show_hide_coreview(bool)));
    connect(ui->actionMessages, SIGNAL(triggered(bool)), this, SLOT(show_messages()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(about_qtmips()));
    connect(ui->actionAboutQt, SIGNAL(triggered(bool)), this, SLOT(about_qt()));
    connect(ui->ips1, SIGNAL(toggled(bool)), this, SLOT(set_speed()));
    connect(ui->ips2, SIGNAL(toggled(bool)), this, SLOT(set_speed()));
    connect(ui->ips5, SIGNAL(toggled(bool)), this, SLOT(set_speed()));
    connect(ui->ips10, SIGNAL(toggled(bool)), this, SLOT(set_speed()));
    connect(ui->ipsUnlimited, SIGNAL(toggled(bool)), this, SLOT(set_speed()));
    connect(ui->ipsMax, SIGNAL(toggled(bool)), this, SLOT(set_speed()));

    connect(this, SIGNAL(report_message(messagetype::Type,QString,int,int,QString,QString)),
            messages, SLOT(insert_line(messagetype::Type,QString,int,int,QString,QString)));
    connect(this, SIGNAL(clear_messages()), messages, SLOT(clear_messages()));
    connect(messages, SIGNAL(message_selected(messagetype::Type,QString,int,int,QString,QString)),
            this, SLOT(message_selected(messagetype::Type,QString,int,int,QString,QString)));

    // Restore application state from settings
    restoreState(settings->value("windowState").toByteArray());
    restoreGeometry(settings->value("windowGeometry").toByteArray());

    // Source editor related actions
    connect(central_window, SIGNAL(currentChanged(int)), this, SLOT(central_tab_changed(int)));

    foreach (QString file_name, settings->value("openSrcFiles").toStringList()) {
        if (file_name.isEmpty())
            continue;
        SrcEditor *editor = new SrcEditor();
        if (editor->loadFile(file_name)) {
            add_src_editor_to_tabs(editor);
        } else {
            delete(editor);
        }
    }

    QDir samples_dir(":/samples");
    for (QString fname: samples_dir.entryList(QDir::Files)) {
        TextSignalAction *textsigac = new TextSignalAction(fname, ":/samples/" + fname);
        ui->menuExamples->addAction(textsigac);
        connect(textsigac, SIGNAL( activated(QString)), this, SLOT(example_source(QString)));
    }

#ifdef __EMSCRIPTEN__
    ui->actionBuildExe->setEnabled(false);
#endif
}

MainWindow::~MainWindow() {
    if (corescene != nullptr)
        delete corescene;
    if (coreview != nullptr)
        delete coreview;
    delete central_window;
    delete ndialog;
    delete registers;
    delete program;
    delete memory;
    delete cache_program;
    delete cache_data;
    delete peripherals;
    delete terminal;
    delete lcd_display;
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

void MainWindow::show_hide_coreview(bool show) {
    coreview_shown = show;
    if (!show && (corescene == nullptr))
        return;
    if ((machine == nullptr) || !show) {
        delete corescene;
        corescene = nullptr;
        if (coreview != nullptr)
            coreview->setScene(corescene);
        return;
    }
    if (show && (corescene != nullptr))
        return;

    if (machine->config().pipelined()) {
        corescene = new CoreViewScenePipelined(machine);
    } else {
        corescene = new CoreViewSceneSimple(machine);
    }
    // Connect scene signals to actions
    connect(corescene, SIGNAL(request_registers()), this, SLOT(show_registers()));
    connect(corescene, SIGNAL(request_program_memory()), this, SLOT(show_program()));
    connect(corescene, SIGNAL(request_data_memory()), this, SLOT(show_memory()));
    connect(corescene, SIGNAL(request_jump_to_program_counter(std::uint32_t)), program, SIGNAL(jump_to_pc(std::uint32_t)));
    connect(corescene, SIGNAL(request_cache_program()), this, SLOT(show_cache_program()));
    connect(corescene, SIGNAL(request_cache_data()), this, SLOT(show_cache_data()));
    connect(corescene, SIGNAL(request_peripherals()), this, SLOT(show_peripherals()));
    connect(corescene, SIGNAL(request_terminal()), this, SLOT(show_terminal()));
    coreview->setScene(corescene);
}

void MainWindow::create_core(const machine::MachineConfig &config, bool load_executable,
                             bool keep_memory) {
    // Create machine
    machine::QtMipsMachine *new_machine = new machine::QtMipsMachine(&config, true, load_executable);

    if (keep_memory && (machine != nullptr)) {
        new_machine->memory_rw()->reset(*machine->memory());
    }

    // Remove old machine
    if (machine != nullptr)
        delete machine;
    machine = new_machine;

    // Create machine view
    if (corescene != nullptr)
        delete corescene;
    corescene = nullptr;
    show_hide_coreview(coreview_shown);

    set_speed(); // Update machine speed to current settings

    if (config.osemu_enable()) {
        osemu::OsSyscallExceptionHandler *osemu_handler =
                new osemu::OsSyscallExceptionHandler(config.osemu_known_syscall_stop(),
                                                     config.osemu_unknown_syscall_stop(),
                                                     config.osemu_fs_root());
        machine->register_exception_handler(machine::EXCAUSE_SYSCALL, osemu_handler);
        connect(osemu_handler, SIGNAL(char_written(int,uint)), terminal, SLOT(tx_byte(int,uint)));
        connect(osemu_handler, SIGNAL(rx_byte_pool(int,uint&,bool&)),
            terminal, SLOT(rx_byte_pool(int,uint&,bool&)));
        machine->set_step_over_exception(machine::EXCAUSE_SYSCALL, true);
        machine->set_stop_on_exception(machine::EXCAUSE_SYSCALL, false);
    } else {
        machine->set_step_over_exception(machine::EXCAUSE_SYSCALL, false);
        machine->set_stop_on_exception(machine::EXCAUSE_SYSCALL,
                                               config.osemu_exception_stop());
    }

    // Connect machine signals and slots
    connect(ui->actionRun, SIGNAL(triggered(bool)), machine, SLOT(play()));
    connect(ui->actionPause, SIGNAL(triggered(bool)), machine, SLOT(pause()));
    connect(ui->actionStep, SIGNAL(triggered(bool)), machine, SLOT(step()));
    connect(ui->actionRestart, SIGNAL(triggered(bool)), machine, SLOT(restart()));
    connect(machine, SIGNAL(status_change(machine::QtMipsMachine::Status)), this, SLOT(machine_status(machine::QtMipsMachine::Status)));
    connect(machine, SIGNAL(program_exit()), this, SLOT(machine_exit()));
    connect(machine, SIGNAL(program_trap(machine::QtMipsException&)), this, SLOT(machine_trap(machine::QtMipsException&)));
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
    lcd_display->setup(machine->peripheral_lcd_display());
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

void MainWindow::machine_reload(bool force_memory_reset, bool force_elf_load) {
    if (machine == nullptr)
        return new_machine();
    bool load_executable = force_elf_load || machine->executable_loaded();
    machine::MachineConfig cnf(&machine->config()); // We have to make local copy as create_core will delete current machine
    try {
        create_core(cnf, load_executable, !load_executable && !force_memory_reset);
    } catch (const machine::QtMipsExceptionInput &e) {
        QMessageBox msg(this);
        msg.setText(e.msg(false));
        msg.setIcon(QMessageBox::Critical);
        msg.setDetailedText(e.msg(true));
        msg.setWindowTitle("Error while initializing new machine");
        msg.exec();
    }
}

void MainWindow::print_action() {
#ifdef QTMIPS_WITH_PRINTING
    QPrinter printer(QPrinter::HighResolution);
    printer.setColorMode(QPrinter::Color);
    QPrintDialog print_dialog(&printer, this);
    if (print_dialog.exec() == QDialog::Accepted) {
        QRectF scene_rect = corescene->sceneRect();
        if (printer.outputFormat() == QPrinter::PdfFormat && (scene_rect.height() != 0)) {
            QPageLayout layout = printer.pageLayout();
            layout.setOrientation(QPageLayout::Portrait);
            QPageSize pagesize = layout.pageSize();
            QRectF paint_rect = layout.paintRect(QPageLayout::Point);
            QSize pointsize = pagesize.sizePoints();
            qreal ratio = scene_rect.width() / scene_rect.height();
            if (paint_rect.height() * ratio > paint_rect.width()) {
                pointsize.setHeight(pointsize.height() - paint_rect.height() + paint_rect.width() / ratio);
            } else {
                pointsize.setWidth(pointsize.width() - paint_rect.width() + paint_rect.height() * ratio);
            }
            pagesize = QPageSize(pointsize, "custom", QPageSize::ExactMatch);
            layout.setPageSize(pagesize, layout.margins());
            printer.setPageLayout(layout);
        }
        QPainter painter(&printer);
        QRectF target_rect = painter.viewport();
        corescene->render(&painter, target_rect, scene_rect, Qt::KeepAspectRatio);
    }
#endif // QTMIPS_WITH_PRINTING
}

#define SHOW_HANDLER(NAME, DEFAULT_AREA) \
    void MainWindow::show_##NAME() { \
        show_dockwidget(NAME, DEFAULT_AREA); \
    } \

SHOW_HANDLER(registers, Qt::TopDockWidgetArea)
SHOW_HANDLER(program, Qt::LeftDockWidgetArea)
SHOW_HANDLER(memory, Qt::RightDockWidgetArea)
SHOW_HANDLER(cache_program, Qt::RightDockWidgetArea)
SHOW_HANDLER(cache_data, Qt::RightDockWidgetArea)
SHOW_HANDLER(peripherals, Qt::RightDockWidgetArea)
SHOW_HANDLER(terminal, Qt::RightDockWidgetArea)
SHOW_HANDLER(lcd_display, Qt::RightDockWidgetArea)
SHOW_HANDLER(cop0dock, Qt::TopDockWidgetArea)
SHOW_HANDLER(messages, Qt::BottomDockWidgetArea)
#undef SHOW_HANDLER

void MainWindow::show_symbol_dialog(){
    if (machine == nullptr || machine->symbol_table() == nullptr)
        return;
    QStringList *symnames = machine->symbol_table()->names();
    GoToSymbolDialog *gotosyboldialog = new GoToSymbolDialog(this, *symnames);
    delete symnames;
    connect(gotosyboldialog, SIGNAL(program_focus_addr(std::uint32_t)),
            program, SIGNAL(focus_addr_with_save(std::uint32_t)));
    connect(gotosyboldialog, SIGNAL(program_focus_addr(std::uint32_t)),
            this, SLOT(show_program()));
    connect(gotosyboldialog, SIGNAL(memory_focus_addr(std::uint32_t)),
            memory, SIGNAL(focus_addr(std::uint32_t)));
    connect(gotosyboldialog, SIGNAL(memory_focus_addr(std::uint32_t)),
            this, SLOT(show_memory()));
    connect(gotosyboldialog, SIGNAL(obtain_value_for_name(std::uint32_t&,QString)),
            machine->symbol_table(), SLOT(name_to_value(std::uint32_t&,QString)));
    gotosyboldialog->setAttribute(Qt::WA_DeleteOnClose);
    gotosyboldialog->open();
}

void MainWindow::about_qtmips()
{
    AboutDialog *aboutdialog = new AboutDialog(this);
    aboutdialog->show();
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

void MainWindow::view_mnemonics_registers(bool enable) {
    machine::Instruction::set_symbolic_registers(enable);
    if (program == nullptr)
        return;
    program->request_update_all();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    settings->setValue("windowGeometry", saveGeometry());
    settings->setValue("windowState", saveState());
    settings->sync();

    QStringList list;
    if (modified_file_list(list, true) && !ignore_unsaved) {
        event->ignore();
        SaveChnagedDialog *dialog = new SaveChnagedDialog(list, this);
        int id = qMetaTypeId<QStringList>();
        if(!QMetaType::isRegistered(id)) {
                qRegisterMetaType<QStringList>();
        }
        connect(dialog, SIGNAL(user_decision(bool,QStringList)),
                this, SLOT(save_exit_or_ignore(bool,QStringList)),
                Qt::QueuedConnection);
        dialog->open();
    }
}

void MainWindow::save_exit_or_ignore(bool cancel, QStringList tosavelist) {
    bool save_unnamed = false;
    if (cancel)
        return;
    for (const auto &fname : tosavelist) {
        SrcEditor *editor = source_editor_for_file(fname, false);
        if (editor->saveAsRequired()) {
            save_unnamed = true;
        } else if (editor != nullptr) {
#ifndef __EMSCRIPTEN__
            editor->saveFile();
#else
            central_window->setCurrentWidget(editor);
            save_source();
            return;
#endif
        }
    }
    if (save_unnamed && (central_window != nullptr)) {
        for (int i = 0; i < central_window->count(); i++) {
            QWidget *w = central_window->widget(i);
            SrcEditor *editor = dynamic_cast<SrcEditor *>(w);
            if (editor == nullptr)
                continue;
            if (!editor->saveAsRequired())
                continue;
            central_window->setCurrentWidget(editor);
            save_source_as();
            return;
        }
    }
    ignore_unsaved = true;
    close();
}

void MainWindow::show_dockwidget(QDockWidget *dw, Qt::DockWidgetArea area) {
    if (dw == nullptr)
        return;
    if (dw->isHidden()) {
        dw->show();
        addDockWidget(area, dw);
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

void MainWindow::setCurrentSrcEditor(SrcEditor *srceditor) {
    current_srceditor = srceditor;
    if (srceditor == nullptr) {
        ui->actionSave->setEnabled(false);
        ui->actionSaveAs->setEnabled(false);
        ui->actionClose->setEnabled(false);
        ui->actionCompileSource->setEnabled(false);
    } else {
        ui->actionSave->setEnabled(true);
        ui->actionSaveAs->setEnabled(true);
        ui->actionClose->setEnabled(true);
        ui->actionCompileSource->setEnabled(true);
    }
}

void MainWindow::tab_widget_destroyed(QObject *obj) {
   if (obj == current_srceditor)
       setCurrentSrcEditor(nullptr);
}

void MainWindow::central_tab_changed(int index) {
    QWidget *widget = central_window->widget(index);
    SrcEditor *srceditor = dynamic_cast<SrcEditor *>(widget);
    if (srceditor != nullptr)
        setCurrentSrcEditor(srceditor);
}

void MainWindow::add_src_editor_to_tabs(SrcEditor *editor) {
    central_window->addTab(editor, editor->title());
    central_window->setCurrentWidget(editor);
    connect(editor, SIGNAL(destroyed(QObject*)), this, SLOT(tab_widget_destroyed(QObject*)));
}

void MainWindow::update_open_file_list() {
    QStringList open_src_files;
    if ((central_window == nullptr) || (settings == nullptr))
        return;
    for (int i = 0; i < central_window->count(); i++) {
        QWidget *w = central_window->widget(i);
        SrcEditor *editor = dynamic_cast<SrcEditor *>(w);
        if (editor == nullptr)
            continue;
        if (editor->filename() == "")
            continue;
        open_src_files.append(editor->filename());
    }
    settings->setValue("openSrcFiles", open_src_files);
}

bool MainWindow::modified_file_list(QStringList &list, bool report_unnamed) {
    bool ret = false;
    list.clear();
    QStringList open_src_files;
    if (central_window == nullptr)
        return false;
    for (int i = 0; i < central_window->count(); i++) {
        QWidget *w = central_window->widget(i);
        SrcEditor *editor = dynamic_cast<SrcEditor *>(w);
        if (editor == nullptr)
            continue;
        if ((editor->filename() == "") && !report_unnamed)
            continue;
        if (!editor->isModified())
            continue;
        ret = true;
        list.append(editor->filename());
    }
    return ret;
}

static int compare_filenames(const QString &filename1, const QString &filename2) {
    QFileInfo fi1(filename1);
    QFileInfo fi2(filename2);
    QString canon1 = fi1.canonicalFilePath();
    QString canon2 = fi2.canonicalFilePath();
    if (!canon1.isEmpty() && (canon1 == canon2))
        return 2;
    if (filename1 == filename2)
        return 1;
    return 0;
}

SrcEditor *MainWindow::source_editor_for_file(QString filename, bool open) {
    if (central_window == nullptr)
        return nullptr;
    int found_match = 0;
    SrcEditor *found_editor = nullptr;
    for (int i = 0; i < central_window->count(); i++) {
        QWidget *w = central_window->widget(i);
        SrcEditor *editor = dynamic_cast<SrcEditor *>(w);
        if (editor == nullptr)
            continue;
        int match = compare_filenames(filename, editor->filename());
        if ((match > found_match) ||
            ((editor == current_srceditor) && (match >= found_match))) {
            found_editor = editor;
            found_match = match;
        }
    }
    if (found_match > 0)
        return found_editor;

    if (!open)
        return nullptr;

    SrcEditor *editor = new SrcEditor();
    if (!editor->loadFile(filename)) {
        delete editor;
        return nullptr;
    }
    add_src_editor_to_tabs(editor);
    update_open_file_list();
    return editor;
}

void MainWindow::new_source() {
    SrcEditor *editor = new SrcEditor();
    add_src_editor_to_tabs(editor);
    update_open_file_list();
}

void MainWindow::open_source() {
#ifndef __EMSCRIPTEN__
    QString file_name = "";

    file_name = QFileDialog::getOpenFileName(this, tr("Open File"), "", "Source Files (*.asm *.S *.s *.c Makefile)");

    if (!file_name.isEmpty()) {
        SrcEditor *editor = source_editor_for_file(file_name, false);
        if (editor != nullptr) {
            if (central_window != nullptr)
                central_window->setCurrentWidget(editor);
            return;
        }
        editor = new SrcEditor();

        if (editor->loadFile(file_name)) {
            add_src_editor_to_tabs(editor);
        } else {
            QMessageBox::critical(this, "QtMips Error", tr("Cannot open file '%1' for reading.").arg(file_name));
            delete(editor);
        }
    }
#else
    QHtml5File::load("*", [&](const QByteArray &content, const QString &filename) {
            SrcEditor *editor = new SrcEditor();
            editor->loadByteArray(content, filename);
            add_src_editor_to_tabs(editor);
            });
#endif
    update_open_file_list();
}

void MainWindow::save_source_as() {
    if (current_srceditor == nullptr)
       return;
#ifndef __EMSCRIPTEN__
    QFileDialog fileDialog(this, tr("Save as..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setDefaultSuffix("s");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    const QString fn = fileDialog.selectedFiles().first();
    if (!current_srceditor->saveFile(fn)) {
        QMessageBox::critical(this, "QtMips Error", tr("Cannot save file '%1'.").arg(fn));
        return;
    }
    int idx = central_window->indexOf(current_srceditor);
    if (idx >= 0)
        central_window->setTabText(idx, current_srceditor->title());
    update_open_file_list();
#else
    QString filename = current_srceditor->filename();
    if (filename.isEmpty())
        filename = "unknown.s";
    QInputDialog *dialog = new QInputDialog(this);
    dialog->setWindowTitle("Select file name");
    dialog->setLabelText("File name:");
    dialog->setTextValue(filename);
    dialog->setMinimumSize(QSize(200, 100));
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, SIGNAL(textValueSelected(QString)),
            this, SLOT(src_editor_save_to(QString)), Qt::QueuedConnection);
    dialog->open();
#endif
}

void MainWindow::src_editor_save_to(QString filename) {
    if (filename.isEmpty() || (current_srceditor == nullptr))
        return;
    current_srceditor->setFileName(filename);
    if (!current_srceditor->filename().isEmpty())
            save_source();
    int idx = central_window->indexOf(current_srceditor);
    if (idx >= 0)
        central_window->setTabText(idx, current_srceditor->title());
    update_open_file_list();
}

void MainWindow::save_source() {
    if (current_srceditor == nullptr)
        return;
    if (current_srceditor->saveAsRequired())
        return save_source_as();
#ifndef __EMSCRIPTEN__
    if (!current_srceditor->saveFile()) {
        QMessageBox::critical(this, "QtMips Error", tr("Cannot save file '%1'.").arg(current_srceditor->filename()));
    }
#else
     QHtml5File::save(current_srceditor->document()->toPlainText().toUtf8(),
                      current_srceditor->filename());
     current_srceditor->setModified(false);
#endif
}

void MainWindow::close_source_check() {
    if (current_srceditor == nullptr)
        return;
    SrcEditor *editor = current_srceditor;
    if (!editor->isModified()) {
        close_source();
        return;
    }
    QMessageBox *msgbox = new QMessageBox(this);
    msgbox->setWindowTitle("Close unsaved source");
    msgbox->setText("Close unsaved source.");
    msgbox->setInformativeText("Do you want to save your changes?");
    msgbox->setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgbox->setDefaultButton(QMessageBox::Save);
    msgbox->setMinimumSize(QSize(200, 150));
    msgbox->setAttribute(Qt::WA_DeleteOnClose);
    connect(msgbox, SIGNAL(finished(int)),
            this, SLOT(close_source_decided(int)), Qt::QueuedConnection);
    msgbox->open();
}

void MainWindow::close_source_decided(int result) {
    if (current_srceditor == nullptr)
        return;
    SrcEditor *editor = current_srceditor;
    if (result == QMessageBox::Save) {
        if (editor->saveAsRequired()) {
            save_source_as();
            return;
        }
        save_source();
    } else if (result != QMessageBox::Discard) {
        return;
    }
    close_source();
}

void MainWindow::close_source() {
    if (current_srceditor == nullptr)
        return;
    SrcEditor *editor = current_srceditor;
    setCurrentSrcEditor(nullptr);
    int idx = central_window->indexOf(editor);
    if (idx >= 0)
        central_window->removeTab(idx);
    delete editor;
    update_open_file_list();
}

void MainWindow::example_source(QString source_file) {
    SrcEditor *editor = new SrcEditor();

    if (editor->loadFile(source_file)) {
        editor->setSaveAsRequired(true);
        add_src_editor_to_tabs(editor);
        update_open_file_list();
    } else {
        QMessageBox::critical(this, "QtMips Error", tr("Cannot open example file '%1' for reading.").arg(source_file));
        delete(editor);
    }
}

void MainWindow::message_selected(messagetype::Type type, QString file, int line,
                      int column, QString text, QString hint) {

    (void)type;
    (void)column;
    (void)text;
    (void)hint;

    if (file.isEmpty())
        return;
    SrcEditor *editor = source_editor_for_file(file, true);
    if (editor == nullptr)
        return;
    editor->setCursorToLine(line);
    editor->setFocus();
    if (central_window != nullptr)
        central_window->setCurrentWidget(editor);
}

bool SimpleAsmWithEditorCheck::process_file(QString filename, QString *error_ptr) {
    SrcEditor *editor = mainwindow->source_editor_for_file(filename, false);
    if (editor == nullptr)
        return Super::process_file(filename, error_ptr);
    QTextDocument *doc = editor->document();
    int ln = 1;
    for ( QTextBlock block = doc->begin(); block.isValid(); block = block.next(), ln++) {
        QString line = block.text();
        process_line(line, filename, ln);
    }
    return !error_occured;
}

bool SimpleAsmWithEditorCheck::process_pragma(QStringList &operands, QString filename,
                               int line_number, QString *error_ptr) {
    (void)error_ptr;
#if 0
    static const QMap<QString, QDockWidget *MainWindow::*> pragma_how_map = {
        {QString("registers"), static_cast<QDockWidget *MainWindow::*>(&MainWindow::registers)},
    };
#endif
    if ((operands.count() < 2) || QString::compare(operands.at(0), "qtmips", Qt::CaseInsensitive))
        return true;
    QString op = operands.at(1).toLower();
    if (op == "show") {
        if (operands.count() < 3)
            return true;
        QString show_method = "show_" + operands.at(2);
        QString show_method_sig = show_method + "()";
        if (mainwindow->metaObject()->indexOfMethod(show_method_sig.toLatin1().data()) == -1) {
            emit report_message(messagetype::MSG_WARNING, filename, line_number, 0,
                                "#pragma qtmisp show - unknown object " + operands.at(2), "");
            return true;
        }
        QMetaObject::invokeMethod(mainwindow, show_method.toLatin1().data());
        return true;
    }
    if (op == "tab") {
        if ((operands.count() < 3) || error_occured)
            return true;
        if (!QString::compare(operands.at(2), "core", Qt::CaseInsensitive) &&
                (mainwindow->central_window != nullptr) && (mainwindow->coreview != nullptr)) {
            mainwindow->central_window->setCurrentWidget(mainwindow->coreview);
        }
        return true;
    }
    if (op == "focus") {
        bool ok;
        if (operands.count() < 4)
            return true;
        fixmatheval::FmeExpression expression;
        fixmatheval::FmeValue value;
        QString error;
        ok = expression.parse(operands.at(3), error);
        if (!ok) {
            emit report_message(messagetype::MSG_WARNING, filename, line_number, 0, "epression parse error " + error, "");
            return true;
        }
        ok = expression.eval(value, symtab, error);
        if (!ok) {
            emit report_message(messagetype::MSG_WARNING, filename, line_number, 0, "epression evaluation error " + error, "");
            return true;
        }
        if (!QString::compare(operands.at(2), "memory", Qt::CaseInsensitive) && (mainwindow->memory != nullptr)) {
            QMetaObject::invokeMethod(mainwindow->memory, "focus_addr", Qt::AutoConnection, Q_ARG(std::uint32_t, value));
            return true;
        }
        if (!QString::compare(operands.at(2), "program", Qt::CaseInsensitive) && (mainwindow->program != nullptr)) {
            QMetaObject::invokeMethod(mainwindow->program, "focus_addr", Qt::AutoConnection, Q_ARG(std::uint32_t, value));
            return true;
        }
        emit report_message(messagetype::MSG_WARNING, filename, line_number, 0,
                            "unknown #pragma qtmisp focus unknown object " + operands.at(2), "");
        return true;
    }
    emit report_message(messagetype::MSG_WARNING, filename, line_number, 0,
                        "unknown #pragma qtmisp " + op, "");
    return true;
}

void MainWindow::compile_source() {
    bool error_occured = false;
    if (current_srceditor == nullptr)
        return;
    if (machine != nullptr) {
        if (machine->config().reset_at_compile())
            machine_reload(true);
    }
    if (machine == nullptr) {
        QMessageBox::critical(this, "QtMips Error", tr("No machine to store program."));
        return;
    }
    SymbolTableDb symtab(machine->symbol_table_rw(true));
    machine::MemoryAccess *mem = machine->physical_address_space_rw();
    if (mem == nullptr) {
        QMessageBox::critical(this, "QtMips Error", tr("No physical addresspace to store program."));
        return;
    }
    QString filename = current_srceditor->filename();

    machine->cache_sync();
    SrcEditor *editor = current_srceditor;
    QTextDocument *doc = editor->document();

    emit clear_messages();
    SimpleAsmWithEditorCheck sasm(this);

    connect(&sasm, SIGNAL(report_message(messagetype::Type,QString,int,int,QString,QString)),
            this, SIGNAL(report_message(messagetype::Type,QString,int,int,QString,QString)));

    sasm.setup(mem, &symtab, 0x80020000);

    int ln = 1;
    for ( QTextBlock block = doc->begin(); block.isValid(); block = block.next(), ln++) {
        QString line = block.text();
        if (!sasm.process_line(line, filename, ln))
            error_occured = true;
    }
    if (!sasm.finish())
        error_occured = true;

    if (error_occured)
        show_messages();
}

void MainWindow::build_execute() {
    QStringList list;
    if (modified_file_list(list)) {
        SaveChnagedDialog *dialog = new SaveChnagedDialog(list, this);
        connect(dialog, SIGNAL(user_decision(bool,QStringList&)),
                this, SLOT(build_execute_with_save(bool,QStringList&)));
        dialog->open();
    } else {
        build_execute_no_check();
    }
}

void MainWindow::build_execute_with_save(bool cancel, QStringList &tosavelist) {
    if (cancel)
        return;
    for (const auto &fname : tosavelist) {
        SrcEditor *editor = source_editor_for_file(fname, false);
        editor->saveFile();
    }
    build_execute_no_check();
}

void MainWindow::build_execute_no_check() {
    QString work_dir = "";
    ExtProcess *proc;
    ExtProcess *procptr = build_process;
    if (procptr != nullptr) {
        procptr->close();
        procptr->deleteLater();
    }

    emit clear_messages();
    show_messages();
    proc = new ExtProcess(this);
    build_process = procptr;
    connect(proc, SIGNAL(report_message(messagetype::Type,QString,int,int,QString,QString)),
            this, SIGNAL(report_message(messagetype::Type,QString,int,int,QString,QString)));
    connect(proc, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(build_execute_finished(int,QProcess::ExitStatus)));
    if (current_srceditor != nullptr) {
        if (!current_srceditor->filename().isEmpty()) {
            QFileInfo fi(current_srceditor->filename());
            work_dir = fi.dir().path();
        }
    }
    if (work_dir.isEmpty() && (machine != nullptr)) {
        if (!machine->config().elf().isEmpty()) {
            QFileInfo fi(machine->config().elf());
            work_dir = fi.dir().path();
        }
    }
    if (!work_dir.isEmpty())
        proc->setWorkingDirectory(work_dir);
    proc->start("make", QProcess::Unbuffered | QProcess::ReadOnly);
}

void MainWindow::build_execute_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if ((exitStatus != QProcess::NormalExit) || (exitCode != 0))
        return;

    if (machine != nullptr) {
        if (machine->config().reset_at_compile())
            machine_reload(true, true);
    }
}
