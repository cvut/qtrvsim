#ifndef SRCEDITOR_H
#define SRCEDITOR_H

#include "common/memory_ownership.h"
#include "machine/machine.h"

#include <QString>
#include <QSyntaxHighlighter>
#include <QTextEdit>

class SrcEditor : public QTextEdit {
    Q_OBJECT
    using Super = QTextEdit;

public:
    explicit SrcEditor(const QString &text, QWidget *parent = nullptr);
    explicit SrcEditor(QWidget *parent = nullptr);
    QString filename();
    QString title();
    bool loadFile(const QString &filename);
    bool saveFile(QString filename = "");
    bool loadByteArray(const QByteArray &content, const QString &filename = "");
    void setCursorToLine(int ln);
    void setFileName(const QString &filename);
    [[nodiscard]] bool isModified() const;
    void setModified(bool val);
    void setSaveAsRequired(bool val);
    [[nodiscard]] bool saveAsRequired() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    ::Box<QSyntaxHighlighter> highlighter {};
    void setup_common();
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
};

#endif // SRCEDITOR_H
