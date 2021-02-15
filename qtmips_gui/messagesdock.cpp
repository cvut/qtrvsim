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

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTableView>
#include <QComboBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QSettings>

#include "messagesdock.h"
#include "messagesmodel.h"
#include "messagesview.h"
#include "messagetype.h"
#include "hexlineedit.h"



MessagesDock::MessagesDock(QWidget *parent, QSettings *settings) : Super(parent) {
    setObjectName("Messages");
    setWindowTitle("Messages");

    this->settings = settings;

    QWidget *content = new QWidget();

    QListView *messages_content = new MessagesView(0, settings);
    MessagesModel *messages_model = new MessagesModel(this);
    messages_content->setModel(messages_model);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(messages_content);

    content->setLayout(layout);

    setWidget(content);

    connect(this, &MessagesDock::report_message, messages_model,
            &MessagesModel::insert_line);
    connect(this, &MessagesDock::pass_clear_messages, messages_model,
            &MessagesModel::clear_messages);
    connect(messages_content, &QAbstractItemView::activated, messages_model,
            &MessagesModel::activated);
    connect(messages_model, &MessagesModel::message_selected, this,
            &MessagesDock::message_selected);
}

void MessagesDock::insert_line(messagetype::Type type, QString file, int line,
                               int column, QString text, QString hint) {
    report_message(type, file, line, column, text, hint);
}

void MessagesDock::clear_messages() {
    emit pass_clear_messages();
}
