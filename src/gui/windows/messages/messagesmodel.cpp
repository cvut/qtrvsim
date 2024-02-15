#include "messagesmodel.h"

#include <QBrush>
#include <utility>

class MessagesEntry {
public:
    inline MessagesEntry(
        messagetype::Type type,
        QString file,
        int line,
        int column,
        QString text,
        QString hint) {
        this->type = type;
        this->file = std::move(file);
        this->line = line;
        this->column = column;
        this->text = std::move(text);
        this->hint = std::move(hint);
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

MessagesModel::~MessagesModel() {
    clear_messages();
}

int MessagesModel::rowCount(const QModelIndex & /*parent*/) const {
    return messages.count();
}

int MessagesModel::columnCount(const QModelIndex & /*parent*/) const {
    return 1;
}

QVariant MessagesModel::headerData(
    int section,
    Qt::Orientation orientation,
    int role) const {
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            switch (section) {
            case 0: return tr("Type");
            case 1: return tr("Source");
            case 2: return tr("Line");
            case 3: return tr("Column");
            case 4: return tr("Text");
            default: return tr("");
            }
        }
    }
    return Super::headerData(section, orientation, role);
}

QVariant MessagesModel::data(const QModelIndex &index, int role) const {
    if (index.row() >= rowCount()) {
        return {};
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        MessagesEntry *ent = messages.at(index.row());
        QString ret = "";
        if (!ent->file.isEmpty()) {
            ret += ent->file + ":";
        }
        if (ent->line) {
            ret += QString::number(ent->line) + ":";
        }
        if (ent->column) {
            ret += QString::number(ent->column) + ":";
        }
        ret += ent->text;
        return ret;
    }
    if (role == Qt::BackgroundRole) {
        MessagesEntry *ent = messages.at(index.row());
        switch (ent->type) {
        case messagetype::MSG_ERROR: return QBrush(QColor(255, 230, 230));
        case messagetype::MSG_WARNING: return QBrush(QColor(255, 255, 220));
        default: return {};
        }
    }
    return {};
}

void MessagesModel::insert_line(
    messagetype::Type type,
    const QString &file,
    int line,
    int column,
    const QString &text,
    const QString &hint) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    messages.append(new MessagesEntry(type, file, line, column, text, hint));
    endInsertRows();
}

void MessagesModel::clear_messages() {
    auto row_count = rowCount();
    if (row_count == 0) return;
    beginRemoveRows(QModelIndex(), 0, row_count - 1);
    while (!messages.isEmpty()) {
        delete messages.takeFirst();
    }
    endRemoveRows();
}

void MessagesModel::activated(QModelIndex index) {
    if (index.row() >= rowCount()) {
        return;
    }

    MessagesEntry *ent = messages.at(index.row());
    emit message_selected(
        ent->type, ent->file, ent->line, ent->column, ent->text, ent->hint);
}
