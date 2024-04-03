#ifndef FIXMATHEVAL_H
#define FIXMATHEVAL_H

#include "memory/address.h"

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
    explicit FmeNode(int priority);
    virtual ~FmeNode();
    virtual bool
    eval(FmeValue &value, FmeSymbolDb *symdb, QString &error, machine::Address inst_addr)
        = 0;
    virtual bool insert(FmeNode *node);
    virtual FmeNode *child();
    virtual QString dump() = 0;
    FmeNode *find_last_child();
    [[nodiscard]] int priority() const;

private:
    int prio;
};

class FmeNodeConstant : public FmeNode {
public:
    explicit FmeNodeConstant(FmeValue value);
    ~FmeNodeConstant() override;
    bool
    eval(FmeValue &value, FmeSymbolDb *symdb, QString &error, machine::Address inst_addr) override;
    QString dump() override;

private:
    FmeValue value;
};

class FmeNodeSymbol : public FmeNode {
public:
    explicit FmeNodeSymbol(QString &name);
    ~FmeNodeSymbol() override;
    bool
    eval(FmeValue &value, FmeSymbolDb *symdb, QString &error, machine::Address inst_addr) override;
    QString dump() override;

private:
    QString name;
};

class FmeNodeUnaryOp : public FmeNode {
public:
    FmeNodeUnaryOp(
        int priority,
        FmeValue (*op)(FmeValue &a, machine::Address inst_addr),
        QString description = "??");
    ~FmeNodeUnaryOp() override;
    bool
    eval(FmeValue &value, FmeSymbolDb *symdb, QString &error, machine::Address inst_addr) override;
    bool insert(FmeNode *node) override;
    FmeNode *child() override;
    QString dump() override;

private:
    FmeValue (*op)(FmeValue &a, machine::Address inst_addr);
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
    bool
    eval(FmeValue &value, FmeSymbolDb *symdb, QString &error, machine::Address inst_addr) override;
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
    bool
    eval(FmeValue &value, FmeSymbolDb *symdb, QString &error, machine::Address inst_addr) override;
    bool insert(FmeNode *node) override;
    FmeNode *child() override;
    QString dump() override;

private:
    FmeNode *root;
};

} // namespace fixmatheval

#endif /*FIXMATHEVAL_H*/
