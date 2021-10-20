/* Based on Qt example released under BSD license */

#include "highlighterasm.h"

#include "QStringList"
#include "machine/instruction.h"

HighlighterAsm::HighlighterAsm(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[]
        = { QStringLiteral("\\.org\\b"),   QStringLiteral("\\.word\\b"),
            QStringLiteral("\\.text\\b"),  QStringLiteral("\\.data\\b"),
            QStringLiteral("\\.globl\\b"), QStringLiteral("\\.set\\b"),
            QStringLiteral("\\.equ\\b"),   QStringLiteral("\\.end\\b"),
            QStringLiteral("\\.ent\\b"),   QStringLiteral("\\.ascii\\b"),
            QStringLiteral("\\.asciz\\b"), QStringLiteral("\\.byte\\b"),
            QStringLiteral("\\.skip\\b"),  QStringLiteral("\\.space\\b") };

    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    QStringList inst_list;
    machine::Instruction::append_recognized_instructions(inst_list);
    foreach (const QString &str, inst_list) {
        rule.pattern = QRegularExpression("\\b" + str + "\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
        rule.pattern = QRegularExpression("\\b" + str.toUpper() + "\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
    rule.pattern = QRegularExpression("\\bnop\\b");
    rule.format = keywordFormat;
    highlightingRules.append(rule);
    rule.pattern = QRegularExpression("\\bNOP\\b");
    rule.format = keywordFormat;
    highlightingRules.append(rule);

    registerFormat.setFontWeight(QFont::Bold);
    registerFormat.setForeground(Qt::darkMagenta);
    QStringList reg_list;
    machine::Instruction::append_recognized_registers(reg_list);
    foreach (const QString &str, reg_list) {
        rule.pattern = QRegularExpression("\\b" + str + "\\b");
        rule.format = registerFormat;
        highlightingRules.append(rule);
    }
    rule.pattern = QRegularExpression("\\bx[0-9]+\\b");
    rule.format = registerFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression(QStringLiteral(";[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);
    rule.pattern = QRegularExpression(QStringLiteral("#[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // functionFormat.setFontItalic(true);
    // functionFormat.setForeground(Qt::blue);
    // rule.pattern =
    // QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    // rule.format = functionFormat; highlightingRules.append(rule);

    // commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    // commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

void HighlighterAsm::highlightBlock(const QString &text) {
#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
    foreach (const HighlightingRule &rule, highlightingRules)
#else
    for (const HighlightingRule &rule : qAsConst(highlightingRules))
#endif
    {
        QRegularExpressionMatchIterator matchIterator
            = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(
                match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);

#if 0
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
#endif
}
