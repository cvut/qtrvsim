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

#include <QFile>
#include <QFileInfo>
#include <QTextDocumentWriter>
#include <QTextCursor>
#include <QTextBlock>

#include "srceditor.h"

void SrcEditor::setup_common() {
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    setFont(font);
    tname = "Unknown";
}

SrcEditor::SrcEditor(QWidget *parent) : Super(parent) {
    setup_common();
}

SrcEditor::SrcEditor(const QString &text, QWidget *parent) : Super(text, parent) {
    setup_common();
}

SrcEditor::~SrcEditor() {

}

QString SrcEditor::filename() {
    return fname;
}

QString SrcEditor::title() {
    return tname;
}

bool SrcEditor::loadFile(QString filename) {
    QFile file(filename);
    QFileInfo fi(filename);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        setPlainText(file.readAll());
        fname = filename;
        tname = fi.fileName();
        return true;
    } else {
        return false;
    }
}

bool SrcEditor::loadByteArray(const QByteArray &content, QString filename) {
    setPlainText(QString::fromUtf8(content.data(), content.size()));
    if (!filename.isEmpty()) {
        QFileInfo fi(filename);
        fname = filename;
        tname = fi.fileName();
    }
    return true;
}

bool SrcEditor::saveFile(QString filename) {
    if (filename.isEmpty())
        filename = this->filename();
    if (filename.isEmpty())
        return false;
    QFileInfo fi(filename);
    QTextDocumentWriter writer(filename);
    writer.setFormat("plaintext");
    bool success = writer.write(document());
    if (success) {
        fname = filename;
        tname = fi.fileName();
    }
    return success;
}

void SrcEditor::setCursorToLine(int ln) {
    QTextCursor cursor(document()->findBlockByLineNumber(ln-1));
    setTextCursor(cursor);
}
