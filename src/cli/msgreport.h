#ifndef MSGREPORT_H
#define MSGREPORT_H

#include "assembler/messagetype.h"

#include <QCoreApplication>
#include <QObject>
#include <QString>
#include <QVector>

class MsgReport : public QObject {
    Q_OBJECT

    using Super = QObject;

public:
    explicit MsgReport(QCoreApplication *app);

public slots:
    static void report_message(
        messagetype::Type type,
        const QString &file,
        int line,
        int column,
        const QString &text,
        const QString &hint);
};

#endif // MSGREPORT_H
