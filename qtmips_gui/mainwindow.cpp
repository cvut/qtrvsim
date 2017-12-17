#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    settings = new QSettings("CTU", "QtMips");
    coreview  = nullptr;

    ui = new Ui::MainWindow();
    ui->setupUi(this);
    setWindowTitle("QtMips");

    // Create/prepare other widgets
    ndialog = new NewDialog(this, settings);
    cache_content = new CacheContentDock(this);
    cache_content->hide();
    cache_statictics = new CacheStatisticsDock(this);
    cache_statictics->hide();
    registers = new RegistersDock(this);
    registers->hide();

    // Connect signals from menu
    connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(new_machine()));
    connect(ui->actionCache, SIGNAL(triggered(bool)), this, SLOT(show_cache_content()));
    connect(ui->actionCache_statistics, SIGNAL(triggered(bool)), this, SLOT(show_cache_statictics()));
    connect(ui->actionRegisters, SIGNAL(triggered(bool)), this, SLOT(show_registers()));

    // Restore application state from settings
    restoreState(settings->value("windowState").toByteArray());
    restoreGeometry(settings->value("windowGeometry").toByteArray());
}

MainWindow::~MainWindow() {
    settings->sync();
    delete settings;
    if (coreview != nullptr)
        delete coreview;
    delete ndialog;
    delete cache_content;
    delete cache_statictics;
    delete registers;
    delete ui;
    if (machine != nullptr)
        delete machine;
}

void MainWindow::start() {
    this->show();
    ndialog->show();
}

void MainWindow::create_core(machine::MachineConfig *config) {
    // Create machine
    machine = new machine::QtMipsMachine(config);
    // Create machine view
    coreview = new CoreView(this, machine);
    this->setCentralWidget(coreview);

    machine->set_speed(1000); // Set default speed to 1 sec

    // Connect machine signals
    connect(ui->actionRun, SIGNAL(triggered(bool)), machine, SLOT(play()));
    connect(ui->actionPause, SIGNAL(triggered(bool)), machine, SLOT(pause()));
    connect(ui->actionStep, SIGNAL(triggered(bool)), machine, SLOT(step()));
    connect(ui->actionRestart, SIGNAL(triggered(bool)), machine, SLOT(restart()));

    // Setup docks
    registers->setup(machine);
}

void MainWindow::new_machine() {
    ndialog->show();
}

void MainWindow::show_cache_content() {
    show_dockwidget(cache_content);
}

void MainWindow::show_cache_statictics() {
    show_dockwidget(cache_statictics);
}

void MainWindow::show_registers() {
    show_dockwidget(registers);
}

bool MainWindow::configured() {
    return (machine != nullptr);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    settings->setValue("windowGeometry", saveGeometry());
    settings->setValue("windowState", saveState());
    settings->sync();
    QMainWindow::closeEvent(event);
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
