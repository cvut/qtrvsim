#ifndef SRCEDITOR_H
#define SRCEDITOR_H

#include "common/memory_ownership.h"
#include "linenumberarea.h"
#include "machine/machine.h"

#include <QString>
#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <QMimeData>
#include <qplaintextedit.h>
#include <qwidget.h>

class SrcEditor : public QPlainTextEdit {
    Q_OBJECT
    using Super = QPlainTextEdit;

public:
    explicit SrcEditor(QWidget *parent);
    [[nodiscard]] QString filename() const;
    QString title();
    bool loadFile(const QString &filename);
    bool saveFile(QString filename = "");
    bool loadByteArray(const QByteArray &content, const QString &filename = "");
    void setCursorToLine(int ln);
    void setCursorTo(int ln, int col);
    void setFileName(const QString &filename);
    [[nodiscard]] bool isModified() const;
    void setModified(bool val);
    void setSaveAsRequired(bool val);
    [[nodiscard]] bool saveAsRequired() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void insertFromMimeData(const QMimeData *source) override;
    bool canInsertFromMimeData(const QMimeData *source) const override;

signals:
    void file_name_change();

public slots:
    void setShowLineNumbers(bool visible);

private slots:
    void updateMargins(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    ::Box<QSyntaxHighlighter> highlighter {};
    LineNumberArea *line_number_area;
    bool line_numbers_visible = true;
    QString fname;
    QString tname;
    bool saveAsRequiredFl {};
    /** Width of a tab character in spaces. */
    static constexpr unsigned TAB_WIDTH = 4;

    /** Indents selected lines by one tab. */
    void indent_selection(QTextCursor &cursor);

    /** Unindents selected lines by one tab or 4 spaces from the beginning of each line.
     * If only some lines contain less prefix whitespace, remove as much as possible to mimic
     * VS Code behavior. */
    void unindent_selection(QTextCursor &cursor);

    /** Returns true if all lines in the selection are commented out. */
    bool is_selection_comment();

    /** Comments out all lines in the selection. */
    void toggle_selection_comment(QTextCursor &cursor, bool is_comment);

    friend class LineNumberArea;
};

#endif // SRCEDITOR_H
