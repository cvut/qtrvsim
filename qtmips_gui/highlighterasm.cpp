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
/* Based on Qt example released under BSD license */

#include "QStringList"
#include "highlighterasm.h"
#include "instruction.h"

HighlighterAsm::HighlighterAsm(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        QStringLiteral("\\.org\\b"), QStringLiteral("\\.word\\b"),
        QStringLiteral("\\.text\\b"), QStringLiteral("\\.data\\b"),
        QStringLiteral("\\.globl\\b"), QStringLiteral("\\.set\\b"),
        QStringLiteral("\\.equ\\b"), QStringLiteral("\\.end\\b"),
        QStringLiteral("\\.ent\\b"), QStringLiteral("\\.ascii\\b"),
        QStringLiteral("\\.asciz\\b"), QStringLiteral("\\.byte\\b"),
        QStringLiteral("\\.skip\\b"), QStringLiteral("\\.space\\b")
    };

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
        rule.pattern = QRegularExpression("\\b" + str.toLower() + "\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    registerFormat.setFontWeight(QFont::Bold);
    registerFormat.setForeground(Qt::darkMagenta);
    QStringList reg_list;
    machine::Instruction::append_recognized_registers(reg_list);
    foreach (const QString &str, reg_list) {
        rule.pattern = QRegularExpression("\\$" + str + "\\b");
        rule.format = registerFormat;
        highlightingRules.append(rule);
    }
    rule.pattern = QRegularExpression("\\$[0-9]+\\b");
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

    //functionFormat.setFontItalic(true);
    //functionFormat.setForeground(Qt::blue);
    //rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    //rule.format = functionFormat;
    //highlightingRules.append(rule);

    //commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    //commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

void HighlighterAsm::highlightBlock(const QString &text)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
    foreach(const HighlightingRule &rule, highlightingRules)
#else
    for (const HighlightingRule &rule : qAsConst(highlightingRules))
#endif
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
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
