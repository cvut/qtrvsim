#ifndef MESSAGESDOCK_H
#define MESSAGESDOCK_H

#include "messagesmodel.h"

#include <QComboBox>
#include <QDockWidget>
#include <QLabel>
#include <QSettings>

class MessagesDock : public QDockWidget {
    Q_OBJECT

    using Super = QDockWidget;

public:
    MessagesDock(QWidget *parent, QSettings *settings);

public slots:
    void insert_line(
        messagetype::Type type,
        QString file,
        int line,
        int column,
        QString text,
        QString hint);
    void clear_messages();

signals:
    void report_message(
        messagetype::Type type,
        QString file,
        int line,
        int column,
        QString text,
        QString hint);
    void pass_clear_messages();
    void message_selected(
        messagetype::Type type,
        QString file,
        int line,
        int column,
        QString text,
        QString hint);

private:
    QSettings *settings;
};

#endif // MESSAGESDOCK_H
