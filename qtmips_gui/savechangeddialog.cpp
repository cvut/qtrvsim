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

#include "savechangeddialog.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QStandardItem>
#include <QListView>

SaveChnagedDialog::SaveChnagedDialog(QStringList &changedlist, QWidget *parent) :
    QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_ShowModal);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setWindowTitle(tr("Save next modified files?"));

    model = new QStandardItemModel(this);
    bool unknown_inserted = false;

    for ( const auto& fname : changedlist) {
        int row = model->rowCount();
        QStandardItem* item = new QStandardItem();
        item->setData(fname, Qt::UserRole);
        if (!fname.isEmpty()) {
            item->setText(fname);
        } else {
            if (!unknown_inserted) {
                item->setText("Unknown");
                unknown_inserted = true;
            }
        }
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(Qt::Checked);
        model->setItem(row, 0, item);
    }

    QVBoxLayout *all = new QVBoxLayout(this);

    QListView *listview = new QListView(this);
    listview->setModel(model);
    listview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    all->addWidget(listview);

    QWidget *hbBtn = new QWidget();
    QHBoxLayout *hlBtn = new QHBoxLayout(hbBtn);

    QPushButton *cancelButton = new QPushButton(tr("&Cancel"), parent);
    QPushButton *ignoreButton = new QPushButton(tr("&Ignore"), parent);
    QPushButton *saveButton = new QPushButton(tr("&Save"), parent);
    saveButton->setFocus();
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel_clicked()));
    connect(ignoreButton, SIGNAL(clicked()), this, SLOT(ignore_clicked()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(save_clicked()));
    hlBtn->addWidget(cancelButton);
    hlBtn->addStretch();
    hlBtn->addWidget(ignoreButton);
    hlBtn->addStretch();
    hlBtn->addWidget(saveButton);

    all->addWidget(hbBtn);

    setMinimumSize(400, 300);
}

SaveChnagedDialog::~SaveChnagedDialog()
{
}

void SaveChnagedDialog::cancel_clicked() {
    QStringList list;
    emit user_decision(true, list);
    close();
}

void SaveChnagedDialog::ignore_clicked() {
    QStringList list;
    emit user_decision(false, list);
    close();
}

void SaveChnagedDialog::save_clicked() {
    QStringList list;
    for(int r = 0; r < model->rowCount(); ++r) {
        if (model->item(r)->checkState() == Qt::Checked)
            list.append(model->item(r)->data(Qt::UserRole).toString());
    }
    emit user_decision(false, list);
    close();
}

