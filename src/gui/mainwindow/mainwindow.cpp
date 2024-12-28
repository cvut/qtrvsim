#include "windows/editor/editordock.h"
#include "windows/editor/editortab.h"

#include <QProcessEnvironment>
#include <QtWidgets>
#include <qactiongroup.h>
#include <qwidget.h>

#include "assembler/fixmatheval.h"
#include "assembler/simpleasm.h"
#include "dialogs/about/aboutdialog.h"
#include "dialogs/gotosymbol/gotosymboldialog.h"
#include "dialogs/savechanged/savechangeddialog.h"
#include "extprocess.h"
#include "helper/async_modal.h"
#include "mainwindow.h"
#include "os_emulation/ossyscall.h"
#include "textsignalaction.h"
#include "common/logging.h"

#include <QFileInfo>
#include <QMessageBox>
#include <QMetaObject>
#include <QTextDocument>

LOG_CATEGORY("gui.mainwindow");

#ifdef __EMSCRIPTEN__
    #include "qhtml5file.h"

    #include <QFileInfo>

constexpr bool WEB_ASSEMBLY = true;
#else
constexpr bool WEB_ASSEMBLY = false;
#endif

MainWindow::MainWindow(QSettings *settings, QWidget *parent)
    : QMainWindow(parent)
    , settings(settings) {
    machine.reset();
    corescene.reset();

    ui.reset(new Ui::MainWindow());
    ui->setupUi(this);
    setWindowTitle(APP_NAME);
    setDockNestingEnabled(true);

    // Setup central widget

    central_widget_tabs.reset(new HidingTabWidget(this));
    central_widget_tabs->setTabBarAutoHide(true);
    this->setCentralWidget(central_widget_tabs.data());

    coreview.reset(new GraphicsView(this));
    coreview->setWindowTitle("&Core");
    central_widget_tabs->addTab(coreview.data(), coreview->windowTitle());

    // Setup editor

    editor_tabs.reset(new EditorDock(this->settings, central_widget_tabs.data()));
    editor_tabs->setTabBarAutoHide(true);
    editor_tabs->setWindowTitle("&Editor");
    ui->actionBuildExe->setEnabled(false);
    connect(ui->actionNew, &QAction::triggered, editor_tabs.data(), &EditorDock::create_empty_tab);
    connect(ui->actionOpen, &QAction::triggered, editor_tabs.data(), &EditorDock::open_file_dialog);
    connect(ui->actionSave, &QAction::triggered, editor_tabs.data(), &EditorDock::save_current_tab);
    connect(
        ui->actionSaveAs, &QAction::triggered, editor_tabs.data(),
        &EditorDock::save_current_tab_as);
    connect(
        ui->actionClose, &QAction::triggered, editor_tabs.data(), &EditorDock::close_current_tab);
    connect(
        editor_tabs.data(), &EditorDock::requestAddRemoveTab, central_widget_tabs.data(),
        &HidingTabWidget::addRemoveTabRequested);

    connect(
        editor_tabs.data(), &EditorDock::editor_available_changed, this, [this](bool available) {
            ui->actionSave->setEnabled(available);
            ui->actionSaveAs->setEnabled(available);
            ui->actionClose->setEnabled(available);
            ui->actionCompileSource->setEnabled(available);
        });
    if constexpr (!WEB_ASSEMBLY) {
        // Only enable build action if we know there to look for the Makefile.
        connect(editor_tabs.data(), &EditorDock::currentChanged, this, [this](int index) {
            bool has_elf_file = machine != nullptr && !machine->config().elf().isEmpty();
            bool current_tab_is_file
                = (index >= 0) && !editor_tabs->get_tab(index)->get_editor()->filename().isEmpty();
            ui->actionBuildExe->setEnabled(has_elf_file || current_tab_is_file);
        });
    }
    connect(
        ui->actionEditorShowLineNumbers, &QAction::triggered, editor_tabs.data(),
        &EditorDock::set_show_line_numbers);

    bool line_numbers_visible = settings->value("EditorShowLineNumbers", true).toBool();
    editor_tabs->set_show_line_numbers(line_numbers_visible);
    ui->actionEditorShowLineNumbers->setChecked(line_numbers_visible);

    // Create/prepare other widgets

    ndialog.reset(new NewDialog(this, settings));
    registers.reset(new RegistersDock(this, machine::Xlen::_32));
    registers->hide();
    program.reset(new ProgramDock(this, settings));
    addDockWidget(Qt::LeftDockWidgetArea, program.data());
    program->show();
    memory.reset(new MemoryDock(this, settings));
    memory->hide();
    cache_program.reset(new CacheDock(this, "Program"));
    cache_program->hide();
    cache_data.reset(new CacheDock(this, "Data"));
    cache_data->hide();
    cache_level2.reset(new CacheDock(this, "L2"));
    cache_level2->hide();
    bp_btb.reset(new DockPredictorBTB(this));
    bp_btb->hide();
    bp_bht.reset(new DockPredictorBHT(this));
    bp_bht->hide();
    bp_info.reset(new DockPredictorInfo(this));
    bp_info->hide();
    peripherals.reset(new PeripheralsDock(this, settings));
    peripherals->hide();
    terminal.reset(new TerminalDock(this, settings));
    terminal->hide();
    lcd_display.reset(new LcdDisplayDock(this, settings));
    lcd_display->hide();
    csrdock = new CsrDock(this);
    csrdock->hide();
    messages = new MessagesDock(this, settings);
    messages->hide();

    // Execution speed actions
    speed_group = new QActionGroup(this);
    speed_group->addAction(ui->ips1);
    speed_group->addAction(ui->ips2);
    speed_group->addAction(ui->ips5);
    speed_group->addAction(ui->ips10);
    speed_group->addAction(ui->ips25);
    speed_group->addAction(ui->ipsUnlimited);
    speed_group->addAction(ui->ipsMax);
    ui->ips1->setChecked(true);

    // Connect signals from menu
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionNewMachine, &QAction::triggered, this, &MainWindow::new_machine);
    connect(ui->actionReload, &QAction::triggered, this, [this] { machine_reload(false, false); });
    connect(ui->actionPrint, &QAction::triggered, this, &MainWindow::print_action);

    connect(
        ui->actionMnemonicRegisters, &QAction::triggered, this,
        &MainWindow::view_mnemonics_registers);
    connect(ui->actionCompileSource, &QAction::triggered, this, &MainWindow::compile_source);
    connect(ui->actionBuildExe, &QAction::triggered, this, &MainWindow::build_execute);
    connect(ui->actionShow_Symbol, &QAction::triggered, this, &MainWindow::show_symbol_dialog);
    connect(ui->actionRegisters, &QAction::triggered, this, &MainWindow::show_registers);
    connect(ui->actionProgram_memory, &QAction::triggered, this, &MainWindow::show_program);
    connect(ui->actionMemory, &QAction::triggered, this, &MainWindow::show_memory);
    connect(ui->actionProgram_Cache, &QAction::triggered, this, &MainWindow::show_cache_program);
    connect(ui->actionData_Cache, &QAction::triggered, this, &MainWindow::show_cache_data);
    connect(ui->actionL2_Cache, &QAction::triggered, this, &MainWindow::show_cache_level2);

    // Branch predictor
    connect(
        ui->actionBranch_Predictor_History_table, &QAction::triggered, this,
        &MainWindow::show_bp_bht);
    connect(
        ui->actionBranch_Predictor_Target_table, &QAction::triggered, this,
        &MainWindow::show_bp_btb);
    connect(
        ui->actionBranch_Predictor_Info, &QAction::triggered, this,
        &MainWindow::show_bp_info);

    connect(ui->actionPeripherals, &QAction::triggered, this, &MainWindow::show_peripherals);
    connect(ui->actionTerminal, &QAction::triggered, this, &MainWindow::show_terminal);
    connect(ui->actionLcdDisplay, &QAction::triggered, this, &MainWindow::show_lcd_display);
    connect(ui->actionCsrShow, &QAction::triggered, this, &MainWindow::show_csrdock);
    connect(ui->actionCore_View_show, &QAction::triggered, this, &MainWindow::show_hide_coreview);
    connect(ui->actionMessages, &QAction::triggered, this, &MainWindow::show_messages);
    connect(ui->actionResetWindows, &QAction::triggered, this, &MainWindow::reset_windows);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about_program);
    connect(ui->actionAboutQt, &QAction::triggered, this, &MainWindow::about_qt);
    connect(ui->ips1, &QAction::toggled, this, &MainWindow::set_speed);
    connect(ui->ips2, &QAction::toggled, this, &MainWindow::set_speed);
    connect(ui->ips5, &QAction::toggled, this, &MainWindow::set_speed);
    connect(ui->ips10, &QAction::toggled, this, &MainWindow::set_speed);
    connect(ui->ips25, &QAction::toggled, this, &MainWindow::set_speed);
    connect(ui->ipsUnlimited, &QAction::toggled, this, &MainWindow::set_speed);
    connect(ui->ipsMax, &QAction::toggled, this, &MainWindow::set_speed);

    connect(this, &MainWindow::report_message, messages, &MessagesDock::insert_line);
    connect(this, &MainWindow::clear_messages, messages, &MessagesDock::clear_messages);
    connect(messages, &MessagesDock::message_selected, this, &MainWindow::message_selected);

    // Restore application state from settings
    restoreState(settings->value("windowState").toByteArray());
    restoreGeometry(settings->value("windowGeometry").toByteArray());
    if (settings->value("viewMnemonicRegisters").toBool()) {
        ui->actionMnemonicRegisters->trigger();
    }

    for (const QString &file_name : settings->value("openSrcFiles").toStringList()) {
        editor_tabs->open_file(file_name);
    }

    QDir samples_dir(":/samples");
    for (const QString &fname : samples_dir.entryList(QDir::Files)) {
        auto *textsigac = new TextSignalAction(fname, ":/samples/" + fname, ui->menuExamples);
        ui->menuExamples->addAction(textsigac);
        connect(textsigac, &TextSignalAction::activated, this, &MainWindow::example_source);
    }
}

MainWindow::~MainWindow() {
    settings->sync();
}

void MainWindow::start() {
    this->show();
    ndialog->show();
}

void MainWindow::show_hide_coreview(bool show) {
    coreview_shown = show;
    if (!show) {
        if (corescene == nullptr) {
        } else {
            central_widget_tabs->removeTab(central_widget_tabs->indexOf(coreview.data()));
            corescene.reset();
            if (coreview != nullptr) { coreview->setScene(corescene.data()); }
        }
        return;
    }
    if (machine == nullptr) { return; }
    if (corescene != nullptr) { return; }

    if (machine->config().pipelined()) {
        corescene.reset(new CoreViewScenePipelined(machine.data()));
    } else {
        corescene.reset(new CoreViewSceneSimple(machine.data()));
    }
    central_widget_tabs->insertTab(0, coreview.data(), coreview->windowTitle());
    // Ensures correct zoom.
    coreview->setScene(corescene.data());
    this->setCentralWidget(central_widget_tabs.data());

    // Connect scene signals to actions
    connect(corescene.data(), &CoreViewScene::request_registers, this, &MainWindow::show_registers);
    connect(
        corescene.data(), &CoreViewScene::request_program_memory, this, &MainWindow::show_program);
    connect(corescene.data(), &CoreViewScene::request_data_memory, this, &MainWindow::show_memory);
    connect(
        corescene.data(), &CoreViewScene::request_jump_to_program_counter, program.data(),
        &ProgramDock::jump_to_pc);
    connect(
        corescene.data(), &CoreViewScene::request_cache_program, this,
        &MainWindow::show_cache_program);
    connect(
        corescene.data(), &CoreViewScene::request_cache_data, this, &MainWindow::show_cache_data);
    connect(
        corescene.data(), &CoreViewScene::request_peripherals, this, &MainWindow::show_peripherals);
    connect(corescene.data(), &CoreViewScene::request_terminal, this, &MainWindow::show_terminal);
    coreview->setScene(corescene.data());
}

void MainWindow::create_core(
    const machine::MachineConfig &config,
    bool load_executable,
    bool keep_memory) {
    // Create machine
    auto *new_machine = new machine::Machine(config, true, load_executable);

    if (keep_memory && (machine != nullptr)) {
        new_machine->memory_rw()->reset(*machine->memory());
    }

    // Remove old machine
    machine.reset(new_machine);

    // Create machine view
    auto focused_index = central_widget_tabs->currentIndex();
    corescene.reset();
    show_hide_coreview(coreview_shown);
    central_widget_tabs->setCurrentIndex(focused_index);

    set_speed(); // Update machine speed to current settings

    const static machine::ExceptionCause ecall_variats[] = {machine::EXCAUSE_ECALL_ANY,
        machine::EXCAUSE_ECALL_M, machine::EXCAUSE_ECALL_S, machine::EXCAUSE_ECALL_U};

    if (config.osemu_enable()) {
        auto *osemu_handler = new osemu::OsSyscallExceptionHandler(
            config.osemu_known_syscall_stop(), config.osemu_unknown_syscall_stop(),
            config.osemu_fs_root());
        osemu_handler->setParent(new_machine);
        connect(
            osemu_handler, &osemu::OsSyscallExceptionHandler::char_written, terminal.data(),
            QOverload<int, unsigned int>::of(&TerminalDock::tx_byte));
        connect(
            osemu_handler, &osemu::OsSyscallExceptionHandler::rx_byte_pool, terminal.data(),
            &TerminalDock::rx_byte_pool);
        for (auto ecall_variat : ecall_variats) {
            machine->register_exception_handler(ecall_variat, osemu_handler);
            machine->set_step_over_exception(ecall_variat, true);
            machine->set_stop_on_exception(ecall_variat, false);
        }
    } else {
        for (auto ecall_variat : ecall_variats) {
            machine->set_step_over_exception(ecall_variat, false);
            machine->set_stop_on_exception(ecall_variat, config.osemu_exception_stop());
        }
    }

    // Connect machine signals and slots
    connect(ui->actionRun, &QAction::triggered, machine.data(), &machine::Machine::play);
    connect(ui->actionPause, &QAction::triggered, machine.data(), &machine::Machine::pause);
    connect(ui->actionStep, &QAction::triggered, machine.data(), &machine::Machine::step);
    connect(ui->actionRestart, &QAction::triggered, machine.data(), &machine::Machine::restart);
    connect(machine.data(), &machine::Machine::status_change, this, &MainWindow::machine_status);
    connect(machine.data(), &machine::Machine::program_exit, this, &MainWindow::machine_exit);
    connect(machine.data(), &machine::Machine::program_trap, this, &MainWindow::machine_trap);
    // Connect signal from break to machine pause
    connect(
        machine->core(), &machine::Core::stop_on_exception_reached, machine.data(),
        &machine::Machine::pause);

    // Setup docks
    registers->connectToMachine(machine.data());
    program->setup(machine.data());
    memory->setup(machine.data());
    cache_program->setup(machine->cache_program());
    cache_data->setup(machine->cache_data());
    bool cache_after_cache = config.cache_data().enabled() || config.cache_program().enabled();
    cache_level2->setup(machine->cache_level2(), cache_after_cache);

    // Branch predictor
    bp_btb->setup(machine->core()->get_predictor(), machine->core());
    bp_bht->setup(machine->core()->get_predictor(), machine->core());
    bp_info->setup(machine->core()->get_predictor(), machine->core());

    terminal->setup(machine->serial_port());
    peripherals->setup(machine->peripheral_spi_led());
    lcd_display->setup(machine->peripheral_lcd_display());
    csrdock->setup(machine.data());

    connect(
        machine->core(), &machine::Core::step_done, program.data(),
        &ProgramDock::update_pipeline_addrs);

    // Set status to ready
    machine_status(machine::Machine::ST_READY);
}

bool MainWindow::configured() {
    return (machine != nullptr);
}

void MainWindow::new_machine() {
    ndialog->show();
}

void MainWindow::machine_reload(bool force_memory_reset, bool force_elf_load) {
    if (machine == nullptr) { return new_machine(); }
    bool load_executable = force_elf_load || machine->executable_loaded();
    machine::MachineConfig cnf(&machine->config()); // We have to make local copy as create_core
                                                    // will delete the current machine
    try {
        create_core(cnf, load_executable, !load_executable && !force_memory_reset);
    } catch (const machine::SimulatorExceptionInput &e) {
        showAsyncCriticalBox(
            this, "Error while initializing new machine", e.msg(false), e.msg(true));
    }
}

void MainWindow::print_action() {
#ifdef WITH_PRINTING
    printer.setColorMode(QPrinter::Color);
    if (print_dialog.exec() == QDialog::Accepted) {
        // This vector pre-drawing step is required because Qt fallbacks to
        // bitmap and produces extremely large and slow to render files.
        // (https://forum.qt.io/topic/21330/printing-widgets-not-as-bitmap-but-in-a-vector-based-format/3)
        QPicture scene_as_vector;
        {
            QPainter painter(&scene_as_vector);
            corescene->render(&painter);
            painter.end();
        }

        // Prepare printer for PDF printing with appropriate resize.
        QRectF scene_rect = corescene->sceneRect();
        if (printer.outputFormat() == QPrinter::PdfFormat && (scene_rect.height() != 0)) {
            QPageLayout layout = printer.pageLayout();
            layout.setOrientation(QPageLayout::Portrait);
            QPageSize pagesize = layout.pageSize();
            QRectF paint_rect = layout.paintRect(QPageLayout::Point);
            QSize pointsize = pagesize.sizePoints();
            qreal ratio = scene_rect.width() / scene_rect.height();
            // Cut off the excess
            if (paint_rect.height() * ratio > paint_rect.width()) {
                pointsize.setHeight(
                    pointsize.height() - paint_rect.height() + paint_rect.width() / ratio);
            } else {
                pointsize.setWidth(
                    pointsize.width() - paint_rect.width() + paint_rect.height() * ratio);
            }
            pagesize = QPageSize(pointsize, "custom", QPageSize::ExactMatch);
            layout.setPageSize(pagesize, layout.margins());
            printer.setPageLayout(layout);
        }

        QPainter painter(&printer);
        // scale to fit paint size
        QRectF paint_rect = printer.pageLayout().paintRect(QPageLayout::Millimeter);
        double xscale = paint_rect.width() / scene_as_vector.widthMM();
        double yscale = paint_rect.height() / scene_as_vector.heightMM();
        double scale = qMin(xscale, yscale);
        painter.scale(scale, scale);
        painter.drawPicture(0, 0, scene_as_vector);
        painter.end();
    }
#else
    showAsyncMessageBox(
        this, QMessageBox::Information, "Printing is not supported.",
        "The simulator was compiled without printing support.\n"
        "If you compiled the simulator yourself, make sure that the Qt "
        "print support library is present.\n"
        "Otherwise report this to the executable provider (probably your "
        "teacher).");
#endif // WITH_PRINTING
}

#define SHOW_HANDLER(NAME, DEFAULT_AREA, DEFAULT_VISIBLE)                                          \
    void MainWindow::show_##NAME() {                                                               \
        show_dockwidget(&*NAME, DEFAULT_AREA, true, false);                                        \
    }                                                                                              \
void MainWindow::reset_state_##NAME() {                                                            \
    show_dockwidget(&*NAME, DEFAULT_AREA, DEFAULT_VISIBLE, true);                                  \
}

SHOW_HANDLER(registers, Qt::TopDockWidgetArea, true)
SHOW_HANDLER(program, Qt::LeftDockWidgetArea, true)
SHOW_HANDLER(memory, Qt::RightDockWidgetArea, true )
SHOW_HANDLER(cache_program, Qt::RightDockWidgetArea, false)
SHOW_HANDLER(cache_data, Qt::RightDockWidgetArea, false)
SHOW_HANDLER(cache_level2, Qt::RightDockWidgetArea, false)
SHOW_HANDLER(bp_btb, Qt::RightDockWidgetArea, false)
SHOW_HANDLER(bp_bht, Qt::RightDockWidgetArea, false)
SHOW_HANDLER(bp_info, Qt::RightDockWidgetArea, false)
SHOW_HANDLER(peripherals, Qt::RightDockWidgetArea, false)
SHOW_HANDLER(terminal, Qt::RightDockWidgetArea, false)
SHOW_HANDLER(lcd_display, Qt::RightDockWidgetArea, false)
SHOW_HANDLER(csrdock, Qt::TopDockWidgetArea, false)
SHOW_HANDLER(messages, Qt::BottomDockWidgetArea, false)
#undef SHOW_HANDLER

void MainWindow::reset_windows() {
    reset_state_registers();
    reset_state_program();
    reset_state_memory();
    reset_state_cache_program();
    reset_state_cache_data();
    reset_state_cache_level2();
    reset_state_bp_btb();
    reset_state_bp_bht();
    reset_state_bp_info();
    reset_state_peripherals();
    reset_state_terminal();
    reset_state_lcd_display();
    reset_state_csrdock();
    reset_state_messages();
}

void MainWindow::show_symbol_dialog() {
    if (machine == nullptr || machine->symbol_table() == nullptr) { return; }
    QStringList symbol_names = machine->symbol_table()->names();
    auto *gotosyboldialog = new GoToSymbolDialog(this, symbol_names);
    connect(
        gotosyboldialog, &GoToSymbolDialog::program_focus_addr, program.data(),
        &ProgramDock::focus_addr_with_save);
    connect(
        gotosyboldialog, &GoToSymbolDialog::program_focus_addr, this, &MainWindow::show_program);
    connect(
        gotosyboldialog, &GoToSymbolDialog::memory_focus_addr, memory.data(),
        &MemoryDock::focus_addr);
    connect(gotosyboldialog, &GoToSymbolDialog::memory_focus_addr, this, &MainWindow::show_memory);
    connect(
        gotosyboldialog, &GoToSymbolDialog::obtain_value_for_name, machine->symbol_table(),
        &machine::SymbolTable::name_to_value);
    gotosyboldialog->setAttribute(Qt::WA_DeleteOnClose);
    gotosyboldialog->open();
}

void MainWindow::about_program() {
    auto *aboutdialog = new AboutDialog(this);
    aboutdialog->show();
}

void MainWindow::about_qt() {
    QMessageBox::aboutQt(this);
}

void MainWindow::set_speed() {
    if (machine == nullptr) {
        return; // just ignore
    }

    if (ui->ips1->isChecked()) {
        machine->set_speed(1000);
    } else if (ui->ips2->isChecked()) {
        machine->set_speed(500);
    } else if (ui->ips5->isChecked()) {
        machine->set_speed(200);
    } else if (ui->ips10->isChecked()) {
        machine->set_speed(100);
    } else if (ui->ips25->isChecked()) {
        machine->set_speed(40);
    } else if (ui->ipsMax->isChecked()) {
        machine->set_speed(0, 100);
    } else {
        machine->set_speed(0);
    }
}

void MainWindow::view_mnemonics_registers(bool enable) {
    machine::Instruction::set_symbolic_registers(enable);
    settings->setValue("viewMnemonicRegisters", enable);
    if (program == nullptr) { return; }
    program->request_update_all();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    settings->setValue("windowGeometry", saveGeometry());
    settings->setValue("windowState", saveState());
    settings->setValue("openSrcFiles", editor_tabs->get_open_file_list());
    settings->sync();

    QStringList list;
    if (!ignore_unsaved && editor_tabs->get_modified_tab_filenames(list, true)) {
        event->ignore();
        auto *dialog = new SaveChangedDialog(list, this);
        if (!QMetaType::isRegistered(qMetaTypeId<QStringList>())) {
            qRegisterMetaType<QStringList>();
        }
        connect(
            dialog, &SaveChangedDialog::user_decision, this,
            [this](bool cancel, const QStringList &tabs_to_save) {
                if (cancel) return;
                for (const auto &name : tabs_to_save) {
                    auto tab_id = editor_tabs->find_tab_id_by_filename(name);
                    if (tab_id.has_value()) editor_tabs->save_tab(tab_id.value());
                }
                ignore_unsaved = true;
                close();
            },
            Qt::QueuedConnection);
        dialog->open();
    }
}

void MainWindow::show_dockwidget(QDockWidget *dw, Qt::DockWidgetArea area,
                                 bool defaultVisible, bool resetState) {
    if (dw == nullptr) { return; }
    if (resetState) {
        if (dw->isFloating()) {
            dw->hide();
            dw->setFloating(false);
        }
        addDockWidget(area, dw);
        if (defaultVisible) {
            dw->show();
        } else {
            dw->hide();
        }
    } else if (dw->isHidden()) {
        dw->show();
        addDockWidget(area, dw);
    } else {
        dw->raise();
        dw->setFocus();
    }
}

void MainWindow::machine_status(enum machine::Machine::Status st) {
    QString status;
    switch (st) {
    case machine::Machine::ST_READY:
        ui->actionPause->setEnabled(false);
        ui->actionRun->setEnabled(true);
        ui->actionStep->setEnabled(true);
        status = "Ready";
        break;
    case machine::Machine::ST_RUNNING:
        ui->actionPause->setEnabled(true);
        ui->actionRun->setEnabled(false);
        ui->actionStep->setEnabled(false);
        status = "Running";
        break;
    case machine::Machine::ST_BUSY:
        // Busy is not interesting (in such case we should just be running
        return;
    case machine::Machine::ST_EXIT:
        // machine_exit is called, so we disable controls in that
        status = "Exited";
        break;
    case machine::Machine::ST_TRAPPED:
        // machine_trap is called, so we disable controls in that
        status = "Trapped";
        break;
    default: status = "Unknown"; break;
    }
    ui->statusBar->showMessage(status);
}

void MainWindow::machine_exit() {
    ui->actionPause->setEnabled(false);
    ui->actionRun->setEnabled(false);
    ui->actionStep->setEnabled(false);
}

void MainWindow::machine_trap(machine::SimulatorException &e) {
    machine_exit();
    showAsyncCriticalBox(this, "Machine trapped", e.msg(false), e.msg(true));
}

void MainWindow::close_source_by_name(QString &filename, bool ask) {
    editor_tabs->close_tab_by_name(filename, ask);
}

void MainWindow::example_source(const QString &source_file) {
    if (!editor_tabs->open_file(source_file, true)) {
        showAsyncCriticalBox(
            this, "Simulator Error",
            tr("Cannot open example file '%1' for reading.").arg(source_file));
    }
}

void MainWindow::message_selected(
    messagetype::Type type,
    const QString &file,
    int line,
    int column,
    const QString &text,
    const QString &hint) {
    (void)type;
    (void)column;
    (void)text;
    (void)hint;

    if (file.isEmpty()) { return; }
    central_widget_tabs->setCurrentWidget(editor_tabs.data());
    if (!editor_tabs->set_cursor_to(file, line, column)) {
        editor_tabs->open_file_if_not_open(file, false);
        if (!editor_tabs->set_cursor_to(file, line, column))
            return;
    }

    // Highlight the line
    auto editor = editor_tabs->get_current_editor();
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(Qt::red).lighter(160));
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = editor->textCursor();
    selection.cursor.clearSelection();
    editor->setExtraSelections({ selection });
}

bool SimpleAsmWithEditorCheck::process_file(const QString &filename, QString *error_ptr) {
    EditorTab* tab = mainwindow->editor_tabs->find_tab_by_filename(filename);
    if (tab == nullptr) { return Super::process_file(filename, error_ptr); }
    SrcEditor *editor = tab->get_editor();
    QTextDocument *doc = editor->document();
    int ln = 1;
    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next(), ln++) {
        QString line = block.text();
        process_line(line, filename, ln);
    }
    return !error_occured;
}

bool SimpleAsmWithEditorCheck::process_pragma(
    QStringList &operands,
    const QString &filename,
    int line_number,
    QString *error_ptr) {
    (void)error_ptr;
#if 0
    static const QMap<QString, QDockWidget *MainWindow::*> pragma_how_map = {
        {QString("registers"), static_cast<QDockWidget *MainWindow::*>(&MainWindow::registers)},
    };
#endif
    if (operands.count() < 2
        || (QString::compare(operands.at(0), "qtrvsim", Qt::CaseInsensitive)
            && QString::compare(operands.at(0), "qtmips", Qt::CaseInsensitive))) {
        return true;
    }
    QString op = operands.at(1).toLower();
    if (op == "show") {
        if (operands.count() < 3) { return true; }
        QString show_method = "show_" + operands.at(2);
        QString show_method_sig = show_method + "()";
        if (mainwindow->metaObject()->indexOfMethod(show_method_sig.toLatin1().data()) == -1) {
            emit report_message(
                messagetype::MSG_WARNING, filename, line_number, 0,
                "#pragma qtrvsim show - unknown object " + operands.at(2), "");
            return true;
        }
        QMetaObject::invokeMethod(mainwindow, show_method.toLatin1().data());
        return true;
    }
    if (op == "tab") {
        if ((operands.count() < 3) || error_occured) { return true; }
        if (!QString::compare(operands.at(2), "core", Qt::CaseInsensitive)
            && (mainwindow->editor_tabs != nullptr) && (mainwindow->coreview != nullptr)) {
            mainwindow->editor_tabs->setCurrentWidget(mainwindow->coreview.data());
        }
        return true;
    }
    if (op == "focus") {
        bool ok;
        if (operands.count() < 4) { return true; }
        fixmatheval::FmeExpression expression;
        fixmatheval::FmeValue value;
        QString error;
        ok = expression.parse(operands.at(3), error);
        if (!ok) {
            emit report_message(
                messagetype::MSG_WARNING, filename, line_number, 0,
                "expression parse error " + error, "");
            return true;
        }
        ok = expression.eval(value, symtab, error, address);
        if (!ok) {
            emit report_message(
                messagetype::MSG_WARNING, filename, line_number, 0,
                "expression evaluation error " + error, "");
            return true;
        }
        if (!QString::compare(operands.at(2), "memory", Qt::CaseInsensitive)
            && (mainwindow->memory != nullptr)) {
            mainwindow->memory->focus_addr(machine::Address(value));
            return true;
        }
        if (!QString::compare(operands.at(2), "program", Qt::CaseInsensitive)
            && (mainwindow->program != nullptr)) {
            mainwindow->program->focus_addr(machine::Address(value));
            return true;
        }
        emit report_message(
            messagetype::MSG_WARNING, filename, line_number, 0,
            "unknown #pragma qtrvsim focus unknown object " + operands.at(2), "");
        return true;
    }
    emit report_message(
        messagetype::MSG_WARNING, filename, line_number, 0, "unknown #pragma qtrvsim " + op, "");
    return true;
}

void MainWindow::compile_source() {
    bool error_occured = false;
    if (machine != nullptr) {
        if (machine->config().reset_at_compile()) { machine_reload(true); }
    }
    if (machine == nullptr) {
        showAsyncCriticalBox(this, "Simulator Error", tr("No machine to store program."));
        return;
    }
    SymbolTableDb symtab(machine->symbol_table_rw(true));
    machine::FrontendMemory *mem = machine->memory_data_bus_rw();
    if (mem == nullptr) {
        showAsyncCriticalBox(
            this, "Simulator Error", tr("No physical addresspace to store program."));
        return;
    }

    machine->cache_sync();

    auto editor = editor_tabs->get_current_editor();
    auto filename = editor->filename().isEmpty() ? "Unknown" : editor->filename();
    auto content = editor->document();

    emit clear_messages();
    SimpleAsmWithEditorCheck sasm(this);

    connect(&sasm, &SimpleAsm::report_message, this, &MainWindow::report_message);

    sasm.setup(mem, &symtab, machine::Address(0x00000200), machine->core()->get_xlen());

    int ln = 1;
    for (QTextBlock block = content->begin(); block.isValid(); block = block.next(), ln++) {
        QString line = block.text();
        if (!sasm.process_line(line, filename, ln)) { error_occured = true; }
    }
    if (!sasm.finish()) { error_occured = true; }

    if (error_occured) { show_messages(); }
}

void MainWindow::build_execute() {
    QStringList list;
    if (editor_tabs->get_modified_tab_filenames(list, false)) {
        auto *dialog = new SaveChangedDialog(list, this);
        connect(
            dialog, &SaveChangedDialog::user_decision, this, &MainWindow::build_execute_with_save);
        dialog->open();
    } else {
        build_execute_no_check();
    }
}

void MainWindow::build_execute_with_save(
    bool cancel,
    QStringList tosavelist) { // NOLINT(performance-unnecessary-value-param)
    if (cancel) { return; }
    for (const auto &filename : tosavelist) {
        editor_tabs->find_tab_by_filename(filename)->get_editor()->saveFile();
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
    connect(proc, &ExtProcess::report_message, this, &MainWindow::report_message);
    connect(
        proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
        [this](int exitCode, QProcess::ExitStatus exitStatus) {
            if ((exitStatus != QProcess::NormalExit) || (exitCode != 0)) { return; }

            if (machine != nullptr) {
                if (machine->config().reset_at_compile()) { machine_reload(true, true); }
            }
        });
    auto current_srceditor = editor_tabs->get_current_editor();
    if (current_srceditor != nullptr && !current_srceditor->filename().isEmpty()) {
        QFileInfo fi(current_srceditor->filename());
        work_dir = fi.dir().path();
    }
    if (work_dir.isEmpty() && (machine != nullptr)) {
        if (!machine->config().elf().isEmpty()) {
            QFileInfo fi(machine->config().elf());
            work_dir = fi.dir().path();
        }
    }
    if (!work_dir.isEmpty()) {
        proc->setWorkingDirectory(work_dir);
        proc->start("make", {}, QProcess::Unbuffered | QProcess::ReadOnly);
    }
}
