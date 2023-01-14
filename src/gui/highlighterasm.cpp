/* Based on Qt example released under BSD license */

#include "highlighterasm.h"

#include "QStringList"
#include "machine/csr/controlstate.h"
#include "machine/instruction.h"

HighlighterAsm::HighlighterAsm(QTextDocument *parent) : QSyntaxHighlighter(parent) {
    HighlightingRule rule;

    QTextCharFormat keywordFormat, registerFormat, singleLineCommentFormat, multiLineCommentFormat,
        quotationFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    registerFormat.setFontWeight(QFont::Bold);
    registerFormat.setForeground(Qt::darkMagenta);
    singleLineCommentFormat.setForeground(Qt::darkGray);
    multiLineCommentFormat.setForeground(Qt::darkGray);
    quotationFormat.setForeground(Qt::darkGreen);

    {
        const QStringList keywordPatterns
            = { QStringLiteral("\\.org"),   QStringLiteral("\\.word"), QStringLiteral("\\.text"),
                QStringLiteral("\\.data"),  QStringLiteral("\\.bss"),  QStringLiteral("\\.option"),
                QStringLiteral("\\.globl"), QStringLiteral("\\.set"),  QStringLiteral("\\.equ"),
                QStringLiteral("\\.end"),   QStringLiteral("\\.ent"),  QStringLiteral("\\.ascii"),
                QStringLiteral("\\.asciz"), QStringLiteral("\\.byte"), QStringLiteral("\\.skip"),
                QStringLiteral("\\.space") };

        rule.pattern = QRegularExpression("(" + keywordPatterns.join('|') + ")\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    {
        QStringList inst_list;
        machine::Instruction::append_recognized_instructions(inst_list);
        inst_list.append("nop");
        rule.pattern = QRegularExpression(
            QString("\\b(" + inst_list.join('|') + ")\\b"),
            QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    {
        QStringList reg_list;
        machine::Instruction::append_recognized_registers(reg_list);
        rule.pattern = QRegularExpression("\\b(" + reg_list.join('|') + "|x[0-9]+)\\b");
        rule.format = registerFormat;
        highlightingRules.append(rule);
    }

    {
        QTextCharFormat namedValueFormat;
        namedValueFormat.setFontWeight(QFont::Bold);
        namedValueFormat.setForeground(Qt::darkMagenta);

        QString pattern = "\\b(";
        for (const auto &reg : machine::CSR::REGISTERS) {
            pattern.append(reg.name).append('|');
        }
        pattern = pattern.left(pattern.size() - 1);
        pattern.append(")\\b");
        rule.pattern = QRegularExpression(pattern);
        rule.format = namedValueFormat;
        highlightingRules.append(rule);
    }

    rule.pattern = QRegularExpression(QStringLiteral("(;[^\n]*)|(#[^\n]*)|(//[^\n]*)"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    for (auto &rule : highlightingRules) {
        rule.pattern.optimize();
    }
}

void HighlighterAsm::highlightBlock(const QString &text) {
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        auto matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
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
