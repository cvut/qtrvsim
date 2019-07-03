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

#include "fixmatheval.h"

#include "limits.h"

using namespace fixmatheval;

FmeSymbolDb::~FmeSymbolDb() {
}

bool FmeSymbolDb::getValue(FmeValue &value, QString name) {
    (void)value;
    (void)name;
    return false;
}

FmeNode::FmeNode(int priority) {
    prio = priority;
}

FmeNode::~FmeNode() {
}

int FmeNode::priority() {
    return prio;
}

bool FmeNode::insert(FmeNode *node) {
    (void)node;
    return false;
}

FmeNode *FmeNode::child() {
    return nullptr;
}


FmeNodeConstant::FmeNodeConstant(FmeValue value) : FmeNode(INT_MAX) {
    this->value = value;
}

FmeNodeConstant::~FmeNodeConstant() {

}

bool FmeNodeConstant::eval(FmeValue &value, FmeSymbolDb *symdb, QString &error) {
    (void)symdb;
    (void)error;
    value = this->value;
    return true;
}

QString FmeNodeConstant::dump() {
    return QString::number(value);
}

FmeNodeSymbol::FmeNodeSymbol(QString &name) : FmeNode(INT_MAX) {
    this->name = name;
}

FmeNodeSymbol::~FmeNodeSymbol() {
}

bool FmeNodeSymbol::eval(FmeValue &value, FmeSymbolDb *symdb, QString &error) {
    if (!symdb) {
        error = QString("no symbol table to find value for %1").arg(name);
        return false;
    }
    bool ok = symdb->getValue(value, name);
    if (!ok)
        error = QString("value for symbol \"%1\" not found").arg(name);
    return ok;
}

QString FmeNodeSymbol::dump() {
    return name;
}

FmeNodeUnaryOp::FmeNodeUnaryOp(int priority, FmeValue (*op)(FmeValue &a)) :
                FmeNode(priority) {
    this->operand_a = nullptr;
    this->op = op;
}

FmeNodeUnaryOp::~FmeNodeUnaryOp() {
    delete operand_a;
}

bool FmeNodeUnaryOp::FmeNodeUnaryOp::eval(FmeValue &value, FmeSymbolDb *symdb,
                                          QString &error) {
    FmeValue value_a;
    if (!operand_a)
        return false;
    if (!operand_a->eval(value_a, symdb, error))
        return false;
    value = op(value_a);
    return true;
}

FmeNode *FmeNodeUnaryOp::child() {
    return operand_a;
}

bool FmeNodeUnaryOp::insert(FmeNode *node) {
   operand_a = node;
   return true;
}

QString FmeNodeUnaryOp::dump() {
    return "(OP " + (operand_a? operand_a->dump(): "nullptr") + ")";
}

FmeNodeBinaryOp::FmeNodeBinaryOp(int priority, FmeValue (*op)(FmeValue &a, FmeValue &b),
                                 FmeNode *left) : FmeNode(priority) {
    this->operand_a = left;
    this->operand_b = nullptr;
    this->op = op;
}

FmeNodeBinaryOp::~FmeNodeBinaryOp() {
    delete operand_a;
    delete operand_b;
}

bool FmeNodeBinaryOp::eval(FmeValue &value, FmeSymbolDb *symdb, QString &error) {
    FmeValue value_a;
    FmeValue value_b;
    if (!operand_a || !operand_b)
        return false;
    if (!operand_a->eval(value_a, symdb, error) ||
        !operand_b->eval(value_b, symdb, error))
        return false;
    value = op(value_a, value_b);
    return true;
}

FmeNode *FmeNodeBinaryOp::child() {
    return operand_b;
}

bool FmeNodeBinaryOp::insert(FmeNode *node) {
    operand_b = node;
    return true;
}

QString FmeNodeBinaryOp::dump() {
    return "(" + (operand_a? operand_a->dump(): "nullptr") +
            " OP " + (operand_b? operand_b->dump(): "nullptr") + ")";
}

FmeExpression::FmeExpression() : FmeNode(0) {
    root = nullptr;
}

bool FmeExpression::parse(const QString &expression, QString &error) {
    delete root;
    int base_prio = 100;
    root = nullptr;
    bool ok = true;
    int i;
    int word_start = 0;
    bool in_word = false;
    bool is_unary = true;
    for (i = 0; true; i++) {
        QChar ch = 0;
        if (i < expression.size())
            ch = expression.at(i);
        if(!(ch.isLetterOrNumber() || (ch == '_')) || (i >= expression.size())) {
            if (in_word) {
                FmeNode *new_node = nullptr;
                QString word = expression.mid(word_start, i - word_start);
                if (word.at(0).isDigit()) {
                    new_node = new FmeNodeConstant(word.toLongLong(&ok, 0));
                    if (!ok) {
                       error = QString("cannot convert \"%1\" to number").arg(word);
                       break;
                    }
                } else {
                   new_node = new FmeNodeSymbol(word);
                }
                FmeNode *node;
                FmeNode *child;
                for (node = this; (child = node->child()) != nullptr; node = child);
                ok = node->insert(new_node);
                if (!ok) {
                    error = QString("parse stuck at \"%1\"").arg(word);
                    break;
                }

                in_word = false;
                is_unary = false;
                if (i >= expression.size())
                    break;
            }
            if (ch.isSpace())
                continue;
            FmeValue (*binary_op)(FmeValue &a, FmeValue &b) = nullptr;
            FmeValue (*unary_op)(FmeValue &a) = nullptr;
            int prio = base_prio;

            if (ch == '~') {
                prio += 90;
                unary_op = [](FmeValue &a) -> FmeValue { return ~a; };
            } else if (ch == '-') {
                if (is_unary) {
                    prio += 90;
                    unary_op = [](FmeValue &a) -> FmeValue { return -a; };
                } else {
                    binary_op = [](FmeValue &a, FmeValue &b) -> FmeValue { return a - b; };
                    prio += 20;
                }
            } else if (ch == '+') {
                if (is_unary)
                    continue;
                binary_op = [](FmeValue &a, FmeValue &b) -> FmeValue { return a + b; };
                prio += 20;
            } else if (ch == '*') {
                binary_op = [](FmeValue &a, FmeValue &b) -> FmeValue { return a * b; };
                prio += 30;
            } else if (ch == '/') {
                binary_op = [](FmeValue &a, FmeValue &b) -> FmeValue { return a / b; };
                prio += 30;
            } else if (ch == '|') {
                binary_op = [](FmeValue &a, FmeValue &b) -> FmeValue { return a | b; };
                prio += 10;
            } else if (ch == '&') {
                binary_op = [](FmeValue &a, FmeValue &b) -> FmeValue { return a & b; };
                prio += 15;
            } else if (ch == '^') {
                binary_op = [](FmeValue &a, FmeValue &b) -> FmeValue { return a ^ b; };
                prio += 15;
            } else if (ch == '(') {
                base_prio += 100;
            } else if (ch == ')') {
                base_prio -= 100;
                if (base_prio <= 0) {
                    ok = false;
                    error = QString("Unbalanced brackets.");
                    break;
                }
            } else {
                error = QString("Unknow character \"%1\" in expression.").arg(ch);
                ok = false;
                break;
            }
            if ((binary_op != nullptr) || (unary_op != nullptr)) {
                FmeNode *node;
                FmeNode *child;
                for (node = this; (child = node->child()) != nullptr; node = child) {
                    if (child->priority() >= prio)
                        break;
                }
                if (binary_op != nullptr) {
                    ok = node->insert(new FmeNodeBinaryOp(prio, binary_op, child));
                    is_unary = true;
                } else {
                    ok = node->insert(new FmeNodeUnaryOp(prio, unary_op));
                }
                if (!ok) {
                    error = QString("parse stuck at \"%1\"").arg(QString(ch));
                    break;
                }
            }
        } else {
            if (!in_word)
                word_start = i;
            in_word = true;
        }
    }

    return ok;
}

FmeExpression::~FmeExpression() {
    delete root;
    root = nullptr;
}

bool FmeExpression::eval(FmeValue &value, FmeSymbolDb *symdb, QString &error) {
    if (!root)
        return false;
    return root->eval(value, symdb, error);
}

bool FmeExpression::insert(FmeNode *node) {
    root = node;
    return true;
}

FmeNode *FmeExpression::child() {
    return root;
}

QString FmeExpression::dump() {
    return "(" + (root? root->dump(): "nullptr") + ")";
}

