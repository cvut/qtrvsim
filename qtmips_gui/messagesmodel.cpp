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

#include <QBrush>

#include "messagesmodel.h"

class MessagesEntry {
public:
    inline MessagesEntry(messagetype::Type type, QString file, int line,
                  int column, QString text, QString hint) {

        this->type = type;
        this->file = file;
        this->line = line;
        this->column = column;
        this->text = text;
        this->hint = hint;
    }
    messagetype::Type type;
    QString file;
    int line;
    int column;
    QString text;
    QString hint;
};


MessagesModel::MessagesModel(QObject *parent) : Super(parent) {

}

int MessagesModel::rowCount(const QModelIndex & /*parent*/) const {
   return messages.count();
}

int MessagesModel::columnCount(const QModelIndex & /*parent*/) const {
    return 1;
}

QVariant MessagesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal) {
        if(role == Qt::DisplayRole) {
            switch (section) {
            case 0:
                return tr("Type");
            case 1:
                return tr("Source");
            case 2:
                return tr("Line");
            case 3:
                return tr("Column");
            case 4:
                return tr("Text");
            default:
                return tr("");
            }
        }
    }
    return Super::headerData(section, orientation, role);
}

QVariant MessagesModel::data(const QModelIndex &index, int role) const {
    if (index.row() >= rowCount())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        MessagesEntry *ent = messages.at(index.row());
        QString ret = "";
        if (!ent->file.isEmpty())
            ret += ent->file + ":";
        if (ent->line)
            ret += QString::number(ent->line) + ":";
        if (ent->column)
            ret += QString::number(ent->column) + ":";
        ret += ent->text;
        return ret;
    }
    if (role == Qt::BackgroundRole) {
        MessagesEntry *ent = messages.at(index.row());
        switch (ent->type) {
        case messagetype::MSG_ERROR:
            return QBrush(QColor(255, 230, 230));
        case messagetype::MSG_WARNING:
            return QBrush(QColor(255, 255, 220));
        default:
            return QVariant();
        }
    }
    return QVariant();
}

void MessagesModel::insert_line(messagetype::Type type, QString file, int line,
                               int column, QString text, QString hint) {

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    messages.append(new MessagesEntry(type, file, line, column, text, hint));
    endInsertRows();
}

void MessagesModel::clear_messages() {
    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    while (!messages.isEmpty()) {
        delete messages.takeFirst();
    }
    endRemoveRows();
}

void MessagesModel::activated(QModelIndex index) {
    if (index.row() >= rowCount())
        return;

    MessagesEntry *ent = messages.at(index.row());
    emit message_selected(ent->type, ent->file, ent->line, ent->column, ent->text, ent->hint);
}
