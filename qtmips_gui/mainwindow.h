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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "ui_MainWindow.h"
#include "newdialog.h"
#include "coreview.h"
#include "registersdock.h"
#include "programdock.h"
#include "memorydock.h"
#include "cachedock.h"
#include "peripheralsdock.h"
#include "terminaldock.h"
#include "cop0dock.h"

#include "qtmipsmachine.h"
#include "machineconfig.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void start();
    void create_core(const machine::MachineConfig &config);

    bool configured();

public slots:
    // Actions signals
    void new_machine();
    void machine_reload();
    void print_action();
    void show_registers();
    void show_program();
    void show_memory();
    void show_cache_data();
    void show_cache_program();
    void show_peripherals();
    void show_terminal();
    void show_cop0dock();
    void show_symbol_dialog();
    // Actions - help menu
    void about_qtmips();
    void about_qt();
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

    GraphicsView *coreview;
    CoreViewScene *corescene;

    RegistersDock *registers;
    ProgramDock *program;
    MemoryDock *memory;
    CacheDock *cache_program, *cache_data;
    PeripheralsDock *peripherals;
    TerminalDock *terminal;
    Cop0Dock *cop0dock;


    QActionGroup *speed_group;

    QSettings  *settings;

    machine::QtMipsMachine *machine; // Current simulated machine

    void show_dockwidget(QDockWidget *w);
};

#endif // MAINWINDOW_H
