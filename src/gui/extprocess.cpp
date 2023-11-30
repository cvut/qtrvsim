#include "extprocess.h"

#include <QDir>
#include <QFileInfo>

using namespace std;

ExtProcess::ExtProcess(QObject *parent) : Super(parent) {
    setProcessChannelMode(QProcess::MergedChannels);
    connect(
        this, &QProcess::readyReadStandardOutput, this,
        &ExtProcess::process_output);
    connect(
        this, QOverload<int, ExitStatus>::of(&QProcess::finished), this,
        &ExtProcess::report_finished);
    connect(this, &QProcess::started, this, &ExtProcess::report_started);
}

void ExtProcess::report_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if ((exitStatus != QProcess::NormalExit) || (exitCode != 0)) {
        report_message(
            messagetype::MSG_FINISH, "", 0, 0,
            program() + ": failed - exit code " + QString::number(exitCode),
            "");
    } else {
        report_message(
            messagetype::MSG_FINISH, "", 0, 0, program() + ": finished", "");
    }
    deleteLater();
}

void ExtProcess::report_started() {
    report_message(
        messagetype::MSG_START, "", 0, 0, program() + ": started", "");
}

void ExtProcess::process_output() {
    QString file = "";
    int ln = 0;
    int col = 0;
    messagetype::Type type = messagetype::MSG_INFO;

    while (canReadLine()) {
        QString line = QString::fromLocal8Bit(readLine());
        while (line.count() > 0) {
            if (line.at(line.count() - 1) != '\n'
                && line.at(line.count() - 1) != '\r') {
                break;
            }
            line.truncate(line.count() - 1);
        }

        int pos = line.indexOf(':');
        if (pos >= 0) {
            QFileInfo fi(QDir(workingDirectory()), line.mid(0, pos));
            line = line.mid(pos + 1);
            file = fi.absoluteFilePath();
        }

        for (pos = 0; line.count() > pos && line.at(pos).isDigit(); pos++) {}

        if ((pos < line.count()) && (line.at(pos) == ':')) {
            ln = line.mid(0, pos).toInt();
            line = line.mid(pos + 1);
        }

        for (pos = 0; line.count() > pos && line.at(pos).isDigit(); pos++) {}

        if ((pos < line.count()) && (line.at(pos) == ':')) {
            col = line.mid(0, pos).toInt();
            line = line.mid(pos + 1);
        }

        if (line.startsWith(' ')) {
            line = line.mid(1);
        }
        if (line.startsWith('\t')) {
            line = line.mid(1);
        }
        if (line.startsWith("error:", Qt::CaseInsensitive)) {
            type = messagetype::MSG_ERROR;
        } else if (line.startsWith("warning:", Qt::CaseInsensitive)) {
            type = messagetype::MSG_WARNING;
        }

        report_message(type, file, ln, col, line, "");
    }
}
