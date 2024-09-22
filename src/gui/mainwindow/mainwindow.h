#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef WITH_PRINTING
    #include <QPrintDialog>
    #include <QPrinter>
#endif
#include "assembler/simpleasm.h"
#include "dialogs/new/newdialog.h"
#include "extprocess.h"
#include "machine/machine.h"
#include "machine/machineconfig.h"
#include "scene.h"
#include "ui_MainWindow.h"
#include "widgets/hidingtabwidget.h"
#include "windows/cache/cachedock.h"
#include "windows/csr/csrdock.h"
#include "windows/editor/editordock.h"
#include "windows/editor/editortab.h"
#include "windows/editor/srceditor.h"
#include "windows/lcd/lcddisplaydock.h"
#include "windows/memory/memorydock.h"
#include "windows/messages/messagesdock.h"
#include "windows/peripherals/peripheralsdock.h"
#include "windows/predictor/predictor_bht_dock.h"
#include "windows/predictor/predictor_btb_dock.h"
#include "windows/predictor/predictor_info_dock.h"
#include "windows/program/programdock.h"
#include "windows/registers/registersdock.h"
#include "windows/terminal/terminaldock.h"

#include <QMainWindow>
#include <QPointer>
#include <QSettings>
#include <QTabWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT

    friend class SimpleAsmWithEditorCheck;

public:
    explicit MainWindow(OWNED QSettings *settings, QWidget *parent = nullptr);
    ~MainWindow() override;

    void start();
    void create_core(
        const machine::MachineConfig &config,
        bool load_executable = true,
        bool keep_memory = false);

    bool configured();

signals:
    void report_message(
        messagetype::Type type,
        QString file,
        int line,
        int column,
        QString text,
        QString hint);
    void clear_messages();

public slots:
    // Actions signals
    void new_machine();
    void machine_reload(bool force_memory_reset = false, bool force_elf_load = false);
    void print_action();
    void close_source_by_name(QString &filename, bool ask = false);
    void example_source(const QString &source_file);
    void compile_source();
    void build_execute();
    void build_execute_no_check();
    void build_execute_with_save(bool cancel, QStringList tosavelist);
    void reset_state_registers();
    void reset_state_program();
    void reset_state_memory();
    void reset_state_cache_program();
    void reset_state_cache_data();
    void reset_state_cache_level2();
    void reset_state_peripherals();
    void reset_state_terminal();
    void reset_state_lcd_display();
    void reset_state_csrdock();
    void reset_state_messages();
    void show_registers();
    void show_program();
    void show_memory();
    void show_cache_data();
    void show_cache_program();
    void show_cache_level2();
    void show_peripherals();
    void show_terminal();
    void show_lcd_display();
    void show_csrdock();
    void show_hide_coreview(bool show);
    void show_messages();
    void reset_windows();
    void show_symbol_dialog();
    // Branch predictor
    void show_bp_btb();
    void show_bp_bht();
    void show_bp_info();
    void reset_state_bp_btb();
    void reset_state_bp_bht();
    void reset_state_bp_info();
    // Actions - help
    void about_program();
    void about_qt();
    // Actions - execution speed
    void set_speed();
    // Machine signals
    void machine_status(enum machine::Machine::Status st);
    void machine_exit();
    void machine_trap(machine::SimulatorException &e);
    void view_mnemonics_registers(bool enable);
    void message_selected(
        messagetype::Type type,
        const QString &file,
        int line,
        int column,
        const QString &text,
        const QString &hint);

protected:
    void closeEvent(QCloseEvent *cancel) override;

private:
    Box<Ui::MainWindow> ui {};

    Box<NewDialog> ndialog {};
    Box<HidingTabWidget> central_widget_tabs {};
    Box<EditorDock> editor_tabs {};

    Box<GraphicsView> coreview {};
    Box<CoreViewScene> corescene;

    Box<RegistersDock> registers {};
    Box<ProgramDock> program {};
    Box<MemoryDock> memory {};
    Box<CacheDock> cache_program {}, cache_data {}, cache_level2 {};

    // Branch predictor
    Box<DockPredictorBTB> bp_btb {};
    Box<DockPredictorBHT> bp_bht {};
    Box<DockPredictorInfo> bp_info {};

    Box<PeripheralsDock> peripherals {};
    Box<TerminalDock> terminal {};
    Box<LcdDisplayDock> lcd_display {};
    CsrDock *csrdock {};
    MessagesDock *messages {};
    bool coreview_shown = true;

    QActionGroup *speed_group {};

    QSharedPointer<QSettings> settings;

    Box<machine::Machine> machine; // Current simulated machine

    void show_dockwidget(
        QDockWidget *w,
        Qt::DockWidgetArea area = Qt::RightDockWidgetArea,
        bool defaultVisible = false,
        bool resetState = false);
    QPointer<ExtProcess> build_process;
    bool ignore_unsaved = false;

#ifdef WITH_PRINTING
    QPrinter printer { QPrinter::HighResolution };
    QPrintDialog print_dialog { &printer, this };
#endif
};

class SimpleAsmWithEditorCheck : public SimpleAsm {
    Q_OBJECT
    using Super = SimpleAsm;

public:
    explicit SimpleAsmWithEditorCheck(MainWindow *a_mainwindow, QObject *parent = nullptr)
        : Super(parent)
        , mainwindow(a_mainwindow) {}
    bool process_file(const QString &filename, QString *error_ptr = nullptr) override;

protected:
    bool process_pragma(
        QStringList &operands,
        const QString &filename = "",
        int line_number = 0,
        QString *error_ptr = nullptr) override;

private:
    MainWindow *mainwindow;
};

#endif // MAINWINDOW_H
