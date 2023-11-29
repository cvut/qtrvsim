#include "srceditor.h"

#include "common/logging.h"
#include "editordock.h"
#include "editortab.h"
#include "linenumberarea.h"
#include "windows/editor/highlighterasm.h"
#include "windows/editor/highlighterc.h"

#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QPainter>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <qglobal.h>
#include <qpalette.h>
#include <qplaintextedit.h>
#include <qstyle.h>

LOG_CATEGORY("gui.src_editor");

SrcEditor::SrcEditor(QWidget *parent) : Super(parent), line_number_area(new LineNumberArea(this)) {
    QFont font1;
    saveAsRequiredFl = true;
    font1.setFamily("Courier");
    font1.setFixedPitch(true);
    font1.setPointSize(10);
    setFont(font1);
    tname = "Unknown";
    highlighter.reset(new HighlighterAsm(document()));

    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::white);
    p.setColor(QPalette::Text, Qt::black);
    p.setColor(QPalette::WindowText, Qt::darkGray);
    setPalette(p);

    // Set tab width to 4 spaces
    setTabStopDistance(fontMetrics().horizontalAdvance(' ') * TAB_WIDTH);

    connect(this, &SrcEditor::blockCountChanged, this, &SrcEditor::updateMargins);
    connect(this, &SrcEditor::updateRequest, this, &SrcEditor::updateLineNumberArea);

    // Clear error highlight on typing
    connect(this, &SrcEditor::textChanged, [this]() { setExtraSelections({}); });

    updateMargins(0);
}

QString SrcEditor::filename() const {
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

    emit file_name_change();
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
    QTextCursor cursor(document()->findBlockByNumber(ln - 1));
    setTextCursor(cursor);
}

void SrcEditor::setCursorTo(int ln, int col) {
    QTextCursor cursor(document()->findBlockByNumber(ln - 1));
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, col - 1);
    setTextCursor(cursor);
    setFocus();
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
        if (!cursor.movePosition(QTextCursor::Down)) break;
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
        if (!cursor.movePosition(QTextCursor::Down)) break;
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
        if (!cursor.movePosition(QTextCursor::Down)) break;
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
        if (!cursor.movePosition(QTextCursor::Down)) break;
    }
    cursor.endEditBlock();
}

void SrcEditor::updateMargins(int /* newBlockCount */) {
    setViewportMargins(line_number_area->sizeHint().width(), 0, 0, 0);
}

void SrcEditor::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy) {
        line_number_area->scroll(0, dy);
    } else {
        line_number_area->update(0, rect.y(), line_number_area->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) updateMargins(0);
}

void SrcEditor::resizeEvent(QResizeEvent *event) {
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    line_number_area->setGeometry(
        QRect(cr.left(), cr.top(), line_number_area->sizeHint().width(), cr.height()));
}

void SrcEditor::setShowLineNumbers(bool show) {
    line_number_area->set(show);
    updateMargins(0);
}

void SrcEditor::insertFromMimeData(const QMimeData *source) {
    if (source->hasText()) { insertPlainText(source->text()); }
}

bool SrcEditor::canInsertFromMimeData(const QMimeData *source) const {
    return source->hasText();
}
