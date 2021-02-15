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

#ifndef FIXMATHEVAL_H
#define FIXMATHEVAL_H

#include <QString>

namespace fixmatheval {

typedef int64_t FmeValue;

class FmeSymbolDb {
public:
    virtual ~FmeSymbolDb();
    virtual bool getValue(FmeValue &value, QString name) = 0;
};

class FmeNode {
public:
    FmeNode(int priority);
    virtual ~FmeNode();
    virtual bool eval(FmeValue &value, FmeSymbolDb *symdb, QString &error) = 0;
    virtual bool insert(FmeNode *node);
    virtual FmeNode *child();
    virtual QString dump() = 0;
    int priority() const;

private:
    int prio;
};

class FmeNodeConstant : public FmeNode {
public:
    FmeNodeConstant(FmeValue value);
    ~FmeNodeConstant() override;
    bool eval(FmeValue &value, FmeSymbolDb *symdb, QString &error) override;
    QString dump() override;

private:
    FmeValue value;
};

class FmeNodeSymbol : public FmeNode {
public:
    FmeNodeSymbol(QString &name);
    ~FmeNodeSymbol() override;
    bool eval(FmeValue &value, FmeSymbolDb *symdb, QString &error) override;
    QString dump() override;

private:
    QString name;
};

class FmeNodeUnaryOp : public FmeNode {
public:
    FmeNodeUnaryOp(
        int priority,
        FmeValue (*op)(FmeValue &a),
        QString description = "??");
    ~FmeNodeUnaryOp() override;
    bool eval(FmeValue &value, FmeSymbolDb *symdb, QString &error) override;
    bool insert(FmeNode *node) override;
    FmeNode *child() override;
    QString dump() override;

private:
    FmeValue (*op)(FmeValue &a);
    FmeNode *operand_a;
    QString description;
};

class FmeNodeBinaryOp : public FmeNode {
public:
    FmeNodeBinaryOp(
        int priority,
        FmeValue (*op)(FmeValue &a, FmeValue &b),
        FmeNode *left,
        QString description = "??");
    ~FmeNodeBinaryOp() override;
    bool eval(FmeValue &value, FmeSymbolDb *symdb, QString &error) override;
    bool insert(FmeNode *node) override;
    FmeNode *child() override;
    QString dump() override;

private:
    FmeValue (*op)(FmeValue &a, FmeValue &b);
    FmeNode *operand_a;
    FmeNode *operand_b;
    QString description;
};

class FmeExpression : public FmeNode {
public:
    FmeExpression();
    ~FmeExpression() override;
    virtual bool parse(const QString &expression, QString &error);
    bool eval(FmeValue &value, FmeSymbolDb *symdb, QString &error) override;
    bool insert(FmeNode *node) override;
    FmeNode *child() override;
    QString dump() override;

private:
    FmeNode *root;
};

} // namespace fixmatheval

#endif /*FIXMATHEVAL*/
