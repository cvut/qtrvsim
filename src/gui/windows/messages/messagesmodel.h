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
    explicit MessagesModel(QObject *parent);
    ~MessagesModel() override;
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role)
        const override;
    [[nodiscard]] QVariant
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
