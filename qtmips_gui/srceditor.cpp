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

#include "srceditor.h"

#include "highlighterasm.h"
#include "highlighterc.h"

#include <QFile>
#include <QFileInfo>
#include <QPalette>
#include <QTextCursor>
#include <QTextDocumentWriter>

void SrcEditor::setup_common() {
    QFont font;
    saveAsRequiredFl = true;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    setFont(font);
    tname = "Unknown";
    highlighter = new HighlighterAsm(document());

    QPalette p = palette();
    p.setColor(QPalette::Active, QPalette::Base, Qt::white);
    p.setColor(QPalette::Inactive, QPalette::Base, Qt::white);
    p.setColor(QPalette::Disabled, QPalette::Base, Qt::white);
    setPalette(p);

    setTextColor(Qt::black);
}

SrcEditor::SrcEditor(QWidget *parent) : Super(parent) {
    setup_common();
}

SrcEditor::SrcEditor(const QString &text, QWidget *parent)
    : Super(text, parent) {
    setup_common();
}

SrcEditor::~SrcEditor() {
    delete highlighter;
}

QString SrcEditor::filename() {
    return fname;
}

QString SrcEditor::title() {
    return tname;
}

void SrcEditor::setFileName(const QString &filename) {
    QFileInfo fi(filename);
    saveAsRequiredFl = filename.isEmpty() || filename.startsWith(":/");

    fname = filename;
    tname = fi.fileName();
    delete highlighter;
    highlighter = nullptr;
    if ((fi.suffix() == "c") || (fi.suffix() == "C") || (fi.suffix() == "cpp")
        || ((fi.suffix() == "c++"))) {
        highlighter = new HighlighterC(document());
    } else {
        highlighter = new HighlighterAsm(document());
    }
}

bool SrcEditor::loadFile(const QString &filename) {
    QFile file(filename);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        setPlainText(file.readAll());
        setFileName(filename);
        return true;
    } else {
        return false;
    }
}

bool SrcEditor::loadByteArray(
    const QByteArray &content,
    const QString &filename) {
    setPlainText(QString::fromUtf8(content.data(), content.size()));
    if (!filename.isEmpty()) {
        setFileName(filename);
    }
    return true;
}

bool SrcEditor::saveFile(QString filename) {
    if (filename.isEmpty()) {
        filename = this->filename();
    }
    if (filename.isEmpty()) {
        return false;
    }
    QTextDocumentWriter writer(filename);
    writer.setFormat("plaintext");
    bool success = writer.write(document());
    setFileName(filename);
    if (success) {
        document()->setModified(false);
    }
    return success;
}

void SrcEditor::setCursorToLine(int ln) {
    QTextCursor cursor(document()->findBlockByLineNumber(ln - 1));
    setTextCursor(cursor);
}

bool SrcEditor::isModified() const {
    return document()->isModified();
}

void SrcEditor::setModified(bool val) {
    document()->setModified(val);
}

void SrcEditor::setSaveAsRequired(bool val) {
    saveAsRequiredFl = val;
}

bool SrcEditor::saveAsRequired() const {
    return saveAsRequiredFl;
}
