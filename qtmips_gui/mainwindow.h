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
#include <QTabWidget>
#include "ui_MainWindow.h"
#include "newdialog.h"
#include "coreview.h"
#include "registersdock.h"
#include "programdock.h"
#include "memorydock.h"
#include "cachedock.h"
#include "peripheralsdock.h"
#include "terminaldock.h"
#include "lcddisplaydock.h"
#include "cop0dock.h"
#include "messagesdock.h"

#include "qtmipsmachine.h"
#include "machineconfig.h"
#include "srceditor.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void start();
    void create_core(const machine::MachineConfig &config, bool load_executable = true);

    bool configured();

signals:
    void report_message(messagetype::Type type, QString file, int line,
                                   int column, QString text, QString hint);
    void clear_messages();

public slots:
    // Actions signals
    void new_machine();
    void machine_reload();
    void print_action();
    void new_source();
    void open_source();
    void save_source();
    void save_source_as();
    void close_source();
    void compile_source();
    void show_registers();
    void show_program();
    void show_memory();
    void show_cache_data();
    void show_cache_program();
    void show_peripherals();
    void show_terminal();
    void show_lcd_display();
    void show_cop0dock();
    void show_hide_coreview(bool show);
    void show_symbol_dialog();
    void show_messages();
    // Actions - help menu
    void about_qtmips();
    void about_qt();
    // Actions - execution speed
    void set_speed();
    // Machine signals
    void machine_status(enum machine::QtMipsMachine::Status st);
    void machine_exit();
    void machine_trap(machine::QtMipsException &e);
    void central_tab_changed(int index);
    void tab_widget_destroyed(QObject *obj);
    void view_mnemonics_registers(bool enable);
    void message_selected(messagetype::Type type, QString file, int line, int column, QString text, QString hint);

protected:
    void closeEvent(QCloseEvent *event);
    void setCurrentSrcEditor(SrcEditor  *srceditor);

protected slots:
    void src_editor_save_to(QString filename);

private:
    Ui::MainWindow *ui;

    NewDialog *ndialog;
    QTabWidget *central_window;

    GraphicsView *coreview;
    CoreViewScene *corescene;

    RegistersDock *registers;
    ProgramDock *program;
    MemoryDock *memory;
    CacheDock *cache_program, *cache_data;
    PeripheralsDock *peripherals;
    TerminalDock *terminal;
    LcdDisplayDock *lcd_display;
    Cop0Dock *cop0dock;
    MessagesDock *messages;
    bool coreview_shown;
    SrcEditor  *current_srceditor;

    QActionGroup *speed_group;

    QSettings  *settings;

    machine::QtMipsMachine *machine; // Current simulated machine

    void show_dockwidget(QDockWidget *w, Qt::DockWidgetArea area = Qt::RightDockWidgetArea);
    void add_src_editor_to_tabs(SrcEditor *editor);
    void update_open_file_list();
    SrcEditor *source_editor_for_file(QString filename, bool open);
};

#endif // MAINWINDOW_H
