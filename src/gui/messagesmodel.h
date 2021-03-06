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

#ifndef MESSAGESMODEL_H
#define MESSAGESMODEL_H

#include "assembler/messagetype.h"

#include <QAbstractListModel>
#include <QFont>
#include <QVector>

class MessagesEntry;

class MessagesModel : public QAbstractListModel {
    Q_OBJECT

    using Super = QAbstractListModel;

public:
    MessagesModel(QObject *parent);
    ~MessagesModel() override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role)
        const override;
    QVariant
    data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public slots:
    void insert_line(
        messagetype::Type type,
        const QString &file,
        int line,
        int column,
        const QString &text,
        const QString &hint);
    void clear_messages();
    void activated(QModelIndex index);

signals:
    void message_selected(
        messagetype::Type type,
        QString file,
        int line,
        int column,
        QString text,
        QString hint);

private:
    QVector<MessagesEntry *> messages;
};

#endif // MESSAGESMODEL_H
