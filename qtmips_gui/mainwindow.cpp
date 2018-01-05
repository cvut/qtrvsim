#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    machine = nullptr;
    corescene = nullptr;
    settings = new QSettings("CTU", "QtMips");

    ui = new Ui::MainWindow();
    ui->setupUi(this);
    setWindowTitle("QtMips");

    // Prepare empty core view
    coreview  = new CoreView(this);
    this->setCentralWidget(coreview);
    // Create/prepare other widgets
    ndialog = new NewDialog(this, settings);
    registers = new RegistersDock(this);
    registers->hide();
    program = new ProgramDock(this);
    program->hide();
    cache_content = new CacheContentDock(this);
    cache_content->hide();
    cache_statictics = new CacheStatisticsDock(this);
    cache_statictics->hide();

    // Execution speed actions
    speed_group = new QActionGroup(this);
    speed_group->addAction(ui->ips1);
    speed_group->addAction(ui->ips5);
    speed_group->addAction(ui->ips10);
    speed_group->addAction(ui->ipsUnlimited);
    ui->ips1->setChecked(true);

    // Connect signals from menu
    connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(new_machine()));
    connect(ui->actionRegisters, SIGNAL(triggered(bool)), this, SLOT(show_registers()));
    connect(ui->actionProgram_memory, SIGNAL(triggered(bool)), this, SLOT(show_program()));
    connect(ui->actionCache, SIGNAL(triggered(bool)), this, SLOT(show_cache_content()));
    connect(ui->actionCache_statistics, SIGNAL(triggered(bool)), this, SLOT(show_cache_statictics()));
    connect(ui->ips1, SIGNAL(toggled(bool)), this, SLOT(set_speed()));
    connect(ui->ips5, SIGNAL(toggled(bool)), this, SLOT(set_speed()));
    connect(ui->ips10, SIGNAL(toggled(bool)), this, SLOT(set_speed()));
    connect(ui->ipsUnlimited, SIGNAL(toggled(bool)), this, SLOT(set_speed()));

    // Restore application state from settings
    restoreState(settings->value("windowState").toByteArray());
    restoreGeometry(settings->value("windowGeometry").toByteArray());
}

MainWindow::~MainWindow() {
    settings->sync();
    delete settings;
    if (corescene != nullptr)
        delete corescene;
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
    // Remove old machine
    if (machine != nullptr)
        delete machine;
    // Create machine
    machine = new machine::QtMipsMachine(config);
    // Create machine view
    corescene = new CoreViewScene(coreview, machine);

    set_speed(); // Update machine speed to current settings

    // Connect machine signals and slots
    connect(ui->actionRun, SIGNAL(triggered(bool)), machine, SLOT(play()));
    connect(ui->actionPause, SIGNAL(triggered(bool)), machine, SLOT(pause()));
    connect(ui->actionStep, SIGNAL(triggered(bool)), machine, SLOT(step()));
    connect(ui->actionRestart, SIGNAL(triggered(bool)), machine, SLOT(restart()));
    connect(machine, SIGNAL(status_change(machine::QtMipsMachine::Status)), this, SLOT(machine_status(machine::QtMipsMachine::Status)));
    connect(machine, SIGNAL(program_exit()), this, SLOT(machine_exit()));
    connect(machine, SIGNAL(program_trap(machine::QtMipsException&)), this, SLOT(machine_trap(machine::QtMipsException&)));

    // Setup docks
    registers->setup(machine);
    program->setup(machine);
    // Set status to ready
    machine_status(machine::QtMipsMachine::ST_READY);
}

bool MainWindow::configured() {
    return (machine != nullptr);
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

void MainWindow::show_program() {
    show_dockwidget(program);
}

void MainWindow::set_speed() {
    if (machine == nullptr)
        return; // just ignore

    if (ui->ips1->isChecked())
        machine->set_speed(1000);
    else if (ui->ips5->isChecked())
        machine->set_speed(500);
    else if (ui->ips10->isChecked())
        machine->set_speed(100);
    else
        machine->set_speed(0);
}

void MainWindow::closeEvent(QCloseEvent *event) {
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
