#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "ui_MainWindow.h"
#include "newdialog.h"
#include "coreview.h"
#include "cachecontent.h"
#include "cachestatistics.h"
#include "registersdock.h"
#include "programdock.h"
#include "memorydock.h"

#include "qtmipsmachine.h"
#include "machineconfig.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void start();
    void create_core(machine::MachineConfig *config);

    bool configured();

public slots:
    // Actions signals
    void new_machine();
    void show_registers();
    void show_program();
    void show_memory();
    void show_cache_content();
    void show_cache_statictics();
    // Actions - execution speed
    void set_speed();
    // Machine signals
    void machine_status(enum machine::QtMipsMachine::Status st);
    void machine_exit();
    void machine_trap(machine::QtMipsException &e);

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    NewDialog *ndialog;

    CoreView *coreview;
    CoreViewScene *corescene;

    RegistersDock *registers;
    ProgramDock *program;
    MemoryDock *memory;
    // TODO implement cahce docks
    CacheContentDock *cache_content;
    CacheStatisticsDock *cache_statictics;

    QActionGroup *speed_group;

    QSettings  *settings;

    machine::QtMipsMachine *machine; // Current simulated machine

    void show_dockwidget(QDockWidget *w);
};

#endif // MAINWINDOW_H
