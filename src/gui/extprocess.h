#ifndef MSGREPORT_H
#define MSGREPORT_H

#include "assembler/messagetype.h"

#include <QProcess>
#include <QString>

class ExtProcess : public QProcess {
    Q_OBJECT

    using Super = QProcess;

public:
    explicit ExtProcess(QObject *parent = nullptr);

signals:
    void report_message(
        messagetype::Type type,
        QString file,
        int line,
        int column,
        QString text,
        QString hint);

protected slots:
    void process_output();
    void report_started();
    void report_finished(int exitCode, QProcess::ExitStatus exitStatus);

protected:
    QByteArray m_buffer;
};

#endif // MSGREPORT_H
