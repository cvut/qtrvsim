/* Based on Qt example released under BSD license */

#ifndef HIGHLIGHTERASM_H
#define HIGHLIGHTERASM_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class HighlighterAsm : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit HighlighterAsm(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
};

#endif // HIGHLIGHTERASM_H
