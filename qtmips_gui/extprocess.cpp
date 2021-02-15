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

#include <QFileInfo>
#include <QDir>
#include "extprocess.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <typeinfo>

using namespace std;

ExtProcess::ExtProcess(QObject *parent) : Super(parent) {
  setProcessChannelMode(QProcess::MergedChannels);
  connect(this, &QProcess::readyReadStandardOutput, this,
          &ExtProcess::process_output);
  connect(this, QOverload<int, ExitStatus>::of(&QProcess::finished), this,
          &ExtProcess::report_finished);
  connect(this, &QProcess::started, this, &ExtProcess::report_started);
}

void ExtProcess::report_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    if ((exitStatus != QProcess::NormalExit) || (exitCode != 0))
        report_message(messagetype::MSG_FINISH, "", 0, 0, program() +
                       ": failed - exit code " + QString::number(exitCode), "");
    else
        report_message(messagetype::MSG_FINISH, "", 0, 0, program() +
                       ": finished", "");
    deleteLater();
}

void ExtProcess::report_started() {
    report_message(messagetype::MSG_START, "", 0, 0, program() + ": started", "");
}

void ExtProcess::process_output()
{
    QString file = "";
    int ln = 0;
    int col = 0;
    messagetype::Type type = messagetype::MSG_INFO;
    if (!canReadLine())
        return;
    QString line = QString::fromLocal8Bit(readLine());
    while (line.count() > 0) {
        if (line.at(line.count() - 1) != '\n' &&
            line.at(line.count() - 1) != '\r')
            break;
        line.truncate(line.count() - 1);
    }

    int pos = line.indexOf(':');
    if (pos >= 0) {
        QFileInfo fi(QDir(workingDirectory()), line.mid(0, pos));
        line = line.mid(pos + 1);
        file = fi.absoluteFilePath();
    }

    for(pos = 0; line.count() > pos && line.at(pos).isDigit(); pos++);
    if ((pos < line.count()) && (line.at(pos) == ':')) {
        ln = line.mid(0, pos).toInt();
        line = line.mid(pos + 1);
    }

    for(pos = 0; line.count() > pos && line.at(pos).isDigit(); pos++);
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
