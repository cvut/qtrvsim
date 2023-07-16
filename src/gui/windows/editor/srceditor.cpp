#include "srceditor.h"

#include "common/logging.h"
#include "windows/editor/highlighterasm.h"
#include "windows/editor/highlighterc.h"

#include <QFile>
#include <QFileInfo>
#include <QPalette>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <qglobal.h>
#include <qtextdocument.h>
#include <qtextedit.h>

LOG_CATEGORY("gui.src_editor");

void SrcEditor::setup_common() {
    QFont font;
    saveAsRequiredFl = true;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    setFont(font);
    tname = "Unknown";
    highlighter.reset(new HighlighterAsm(document()));

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

SrcEditor::SrcEditor(const QString &text, QWidget *parent) : Super(text, parent) {
    setup_common();
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
    if ((fi.suffix() == "c") || (fi.suffix() == "C") || (fi.suffix() == "cpp")
        || ((fi.suffix() == "c++"))) {
        highlighter.reset(new HighlighterC(document()));
    } else {
        highlighter.reset(new HighlighterAsm(document()));
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

bool SrcEditor::loadByteArray(const QByteArray &content, const QString &filename) {
    setPlainText(QString::fromUtf8(content.data(), content.size()));
    if (!filename.isEmpty()) { setFileName(filename); }
    return true;
}

bool SrcEditor::saveFile(QString filename) {
    if (filename.isEmpty()) { filename = this->filename(); }
    if (filename.isEmpty()) { return false; }
    QTextDocumentWriter writer(filename);
    writer.setFormat("plaintext");
    bool success = writer.write(document());
    setFileName(filename);
    if (success) { document()->setModified(false); }
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
void SrcEditor::keyPressEvent(QKeyEvent *event) {
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection()) {
        switch (event->key()) {
        default: break;
        }
    }

    switch (event->key()) {
    case Qt::Key_Return: { // Keep indentation
        QString txt = cursor.block().text();
        QString indent;
        for (auto ch : txt) {
            if (ch.isSpace()) {
                indent.append(ch);
            } else {
                break;
            }
        }
        cursor.insertText("\n");
        cursor.insertText(indent);
        setTextCursor(cursor);
        return;
    }
    }

    QTextEdit::keyPressEvent(event);
}