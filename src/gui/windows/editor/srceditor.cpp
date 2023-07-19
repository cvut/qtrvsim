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
#include <qnamespace.h>
#include <qplaintextedit.h>
#include <qstyle.h>
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

    QTextCharFormat fmt;
    fmt.setForeground(Qt::black);
    mergeCurrentCharFormat(fmt);

    // Set tab width to 4 spaces
    setTabStopDistance(fontMetrics().horizontalAdvance(' ') * TAB_WIDTH);
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
        case Qt::Key_Tab: {
            indent_selection(cursor);
            return;
        }
        case Qt::Key_Backtab: {
            unindent_selection(cursor);
            return;
        }
        case Qt::Key_Slash:
            if (event->modifiers() & Qt::ControlModifier) {
                toggle_selection_comment(cursor, is_selection_comment());
                return;
            }
            break;
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
    case Qt::Key_Slash: {
        if (event->modifiers() & Qt::ControlModifier) {
            // Toggle comment
            if (cursor.block().text().startsWith("//")) {
                cursor.movePosition(QTextCursor::StartOfLine);
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 2);
                cursor.removeSelectedText();
            } else {
                cursor.movePosition(QTextCursor::StartOfLine);
                cursor.insertText("//");
            }
            return;
        }
        break;
    }
    }

    QPlainTextEdit::keyPressEvent(event);
}

void SrcEditor::indent_selection(QTextCursor &cursor) {
    auto end = cursor.selectionEnd();
    cursor.beginEditBlock();
    cursor.setPosition(cursor.selectionStart());
    cursor.movePosition(QTextCursor::StartOfLine);
    while (cursor.position() < end) {
        cursor.insertText("\t");
        cursor.movePosition(QTextCursor::Down);
    }
    cursor.endEditBlock();
}

void SrcEditor::unindent_selection(QTextCursor &cursor) {
    cursor.beginEditBlock();
    auto end_line = document()->findBlock(cursor.selectionEnd()).blockNumber();
    cursor.setPosition(cursor.selectionStart());
    cursor.movePosition(QTextCursor::StartOfLine);
    while (cursor.blockNumber() <= end_line) {
        auto txt = cursor.block().text();
        if (txt.isEmpty()) {
            // Empty line, skip
        } else if (txt.startsWith("\t")) {
            cursor.deleteChar();
        } else if (txt.startsWith(" ")) {
            // Delete at most TAB_WIDTH spaces
            unsigned to_delete = std::min<unsigned>(txt.size(), TAB_WIDTH);
            while (to_delete > 0 && cursor.block().text().startsWith(" ")) {
                cursor.deleteChar();
                to_delete--;
            }
        }
        cursor.movePosition(QTextCursor::Down);
    }
    cursor.endEditBlock();
}

bool SrcEditor::is_selection_comment() {
    QTextCursor cursor = textCursor();
    bool all_commented = true;

    auto end_line = document()->findBlock(cursor.selectionEnd()).blockNumber();
    while (cursor.blockNumber() <= end_line) {
        auto txt = cursor.block().text();
        if (!txt.startsWith("//")) {
            all_commented = false;
            break;
        }
        cursor.movePosition(QTextCursor::Down);
    }

    return all_commented;
}

void SrcEditor::toggle_selection_comment(QTextCursor &cursor, bool is_comment) {
    auto end_line = document()->findBlock(cursor.selectionEnd()).blockNumber();
    cursor.beginEditBlock();
    cursor.setPosition(cursor.selectionStart());
    cursor.movePosition(QTextCursor::StartOfLine);
    while (cursor.blockNumber() <= end_line) {
        if (is_comment) {
            cursor.deleteChar();
            cursor.deleteChar();
        } else {
            cursor.insertText("//");
        }
        cursor.movePosition(QTextCursor::Down);
    }
    cursor.endEditBlock();
};
