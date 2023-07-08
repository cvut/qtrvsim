#ifndef SRCEDITOR_H
#define SRCEDITOR_H

#include "machine/machine.h"

#include <QString>
#include <QSyntaxHighlighter>
#include <QTextEdit>

class SrcEditor : public QTextEdit {
    Q_OBJECT
    using Super = QTextEdit;

public:
    SrcEditor(const QString &text, QWidget *parent = nullptr);
    SrcEditor(QWidget *parent = nullptr);
    ~SrcEditor() override;
    QString filename();
    QString title();
    bool loadFile(const QString &filename);
    bool saveFile(QString filename = "");
    bool loadByteArray(const QByteArray &content, const QString &filename = "");
    void setCursorToLine(int ln);
    void setFileName(const QString &filename);
    bool isModified() const;
    void setModified(bool val);
    void setSaveAsRequired(bool val);
    bool saveAsRequired() const;

private:
    QSyntaxHighlighter *highlighter {};
    void setup_common();
    QString fname;
    QString tname;
    bool saveAsRequiredFl {};
};

#endif // SRCEDITOR_H
