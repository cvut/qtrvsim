#include "fixmatheval.h"

#include "common/math/bit_ops.h"
#include "memory/address.h"

#include <climits>
#include <utility>

using namespace fixmatheval;

// Consumes part of string that is a valid symbol name, returns it and removes it from the input
// string.
QStringView tokenize_symbol(QStringView &expression) {
    QStringView symbol = expression;
    int i = 0;
    for (QChar ch : expression) {
        if (!(ch.isLetterOrNumber() || (ch == '_'))) { break; }
        i++;
    }
    expression = expression.mid(i);
    return symbol;
}

FmeSymbolDb::~FmeSymbolDb() = default;

bool FmeSymbolDb::getValue(FmeValue &value, QString name) {
    (void)value;
    (void)name;
    return false;
}

FmeNode::FmeNode(int priority) {
    prio = priority;
}

FmeNode::~FmeNode() = default;

int FmeNode::priority() const {
    return prio;
}

bool FmeNode::insert(FmeNode *node) {
    (void)node;
    return false;
}

FmeNode *FmeNode::child() {
    return nullptr;
}

FmeNode *FmeNode::find_last_child() {
    FmeNode *current = this;
    FmeNode *child = this->child();
    while (child != nullptr) {
        current = child;
        child = child->child();
    }
    return current;
}

FmeNodeConstant::FmeNodeConstant(FmeValue value) : FmeNode(INT_MAX) {
    this->value = value;
}

FmeNodeConstant::~FmeNodeConstant() = default;

bool FmeNodeConstant::eval(
    FmeValue &value,
    FmeSymbolDb *symdb,
    QString &error,
    machine::Address inst_addr) {
    std::ignore = symdb;
    std::ignore = error;
    std::ignore = inst_addr;
    value = this->value;
    return true;
}

QString FmeNodeConstant::dump() {
    return QString::number(value);
}

FmeNodeSymbol::FmeNodeSymbol(QString &name) : FmeNode(INT_MAX) {
    this->name = name;
}

FmeNodeSymbol::~FmeNodeSymbol() = default;

bool FmeNodeSymbol::eval(
    FmeValue &value,
    FmeSymbolDb *symdb,
    QString &error,
    machine::Address inst_addr) {
    std::ignore = inst_addr;

    if (!symdb) {
        error = QString("no symbol table to find value for %1").arg(name);
        return false;
    }
    bool ok = symdb->getValue(value, name);
    if (!ok) { error = QString("value for symbol \"%1\" not found").arg(name); }
    return ok;
}

QString FmeNodeSymbol::dump() {
    return name;
}

FmeNodeUnaryOp::FmeNodeUnaryOp(
    int priority,
    FmeValue (*op)(FmeValue &a, machine::Address inst_addr),
    QString description)
    : FmeNode(priority) {
    this->operand_a = nullptr;
    this->op = op;
    this->description = std::move(description);
}

FmeNodeUnaryOp::~FmeNodeUnaryOp() {
    delete operand_a;
}

bool FmeNodeUnaryOp::FmeNodeUnaryOp::eval(
    FmeValue &value,
    FmeSymbolDb *symdb,
    QString &error,
    machine::Address inst_addr) {
    FmeValue value_a;
    if (!operand_a) { return false; }
    if (!operand_a->eval(value_a, symdb, error, inst_addr)) { return false; }
    value = op(value_a, inst_addr);
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
    return "(" + description + " " + (operand_a ? operand_a->dump() : "nullptr") + ")";
}

FmeNodeBinaryOp::FmeNodeBinaryOp(
    int priority,
    FmeValue (*op)(FmeValue &a, FmeValue &b),
    FmeNode *left,
    QString description)
    : FmeNode(priority) {
    this->operand_a = left;
    this->operand_b = nullptr;
    this->op = op;
    this->description = std::move(description);
}

FmeNodeBinaryOp::~FmeNodeBinaryOp() {
    delete operand_a;
    delete operand_b;
}

bool FmeNodeBinaryOp::eval(
    FmeValue &value,
    FmeSymbolDb *symdb,
    QString &error,
    machine::Address inst_addr) {
    FmeValue value_a;
    FmeValue value_b;
    if (!operand_a || !operand_b) { return false; }
    if (!operand_a->eval(value_a, symdb, error, inst_addr)
        || !operand_b->eval(value_b, symdb, error, inst_addr)) {
        return false;
    }
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
    return "(" + (operand_a ? operand_a->dump() : "nullptr") + " " + description + " "
           + (operand_b ? operand_b->dump() : "nullptr") + ")";
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
    QString optxtx;
    for (i = 0; true; i++) {
        QChar ch {};
        if (i < expression.size()) { ch = expression.at(i); }
        if (!(ch.isLetterOrNumber() || (ch == '_')) || (i >= expression.size())) {
            if (ch.isSpace()) { continue; }
            if (in_word) {
                FmeNode *new_node = nullptr;
                QString word = expression.mid(word_start, i - word_start);
                if (word.at(0).isDigit()) {
                    new_node = new FmeNodeConstant(word.toLongLong(&ok, 0));
                    if (!ok) {
                        error = QString("cannot convert \"%1\" to number").arg(word);
                        delete new_node;
                        break;
                    }
                } else {
                    new_node = new FmeNodeSymbol(word);
                }
                FmeNode *node = this->find_last_child();
                ok = node->insert(new_node);
                if (!ok) {
                    error = QString("parse stuck at \"%1\"").arg(word);
                    break;
                }

                in_word = false;
                is_unary = false;
            }
            if (i >= expression.size()) { break; }
            FmeValue (*binary_op)(FmeValue &a, FmeValue &b) = nullptr;
            FmeValue (*unary_op)(FmeValue &a, machine::Address inst_addr) = nullptr;
            int prio = base_prio;

            optxtx = ch;
            if (ch == '%') {
                // `%` MODIFIER `(` SYMBOL `)`
                // MODIFIER := `hi` | `lo` | `pcrel_hi` | `pcrel_lo`

                prio += 90;
                // The opening parenthesis is peeked and asserted but not consumed.
                QStringView expr = QStringView(expression).mid(i + 1);
                if (expr.startsWith(QStringLiteral("hi("))) {
                    i += 2;
                    optxtx = QStringLiteral("%hi");
                    unary_op = [](FmeValue &a, machine::Address) -> FmeValue {
                        return get_bits(a, 31, 12);
                    };
                } else if (expr.startsWith(QStringLiteral("lo("))) {
                    i += 2;
                    optxtx = QStringLiteral("%lo");
                    unary_op = [](FmeValue &a, machine::Address) -> FmeValue {
                        return sign_extend(get_bits(a, 11, 0), 12);
                    };
                } else if (expr.startsWith(QStringLiteral("pcrel_hi("))) {
                    i += 8;
                    optxtx = QStringLiteral("%pcrel_hi");
                    unary_op = [](FmeValue &a, machine::Address inst_addr) -> FmeValue {
                        return get_bits(a - inst_addr.get_raw(), 31, 12)
                               + get_bit(a - inst_addr.get_raw(), 11);
                    };
                } else if (expr.startsWith(QStringLiteral("pcrel_lo("))) {
                    i += 8;
                    optxtx = QStringLiteral("%pcrel_lo");
                    unary_op = [](FmeValue &a, machine::Address inst_addr) -> FmeValue {
                        return sign_extend(get_bits(a - inst_addr.get_raw() + 4, 11, 0), 12);
                    };
                } else {
                    auto modifier = tokenize_symbol(expr);
                    error = QString("Unknown modifier \"%1\"").arg(modifier);
                    ok = false;
                    break;
                }
            } else if (ch == '~') {
                prio += 90;
                unary_op = [](FmeValue &a, machine::Address) -> FmeValue { return ~a; };
            } else if (ch == '-') {
                if (is_unary) {
                    prio += 90;
                    unary_op = [](FmeValue &a, machine::Address) -> FmeValue { return -a; };
                } else {
                    binary_op = [](FmeValue &a, FmeValue &b) -> FmeValue { return a - b; };
                    prio += 20;
                }
            } else if (ch == '+') {
                if (is_unary) { continue; }
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
                    if (child->priority() >= prio) { break; }
                }
                if (binary_op != nullptr) {
                    ok = node->insert(new FmeNodeBinaryOp(prio, binary_op, child, optxtx));
                    is_unary = true;
                } else {
                    ok = node->insert(new FmeNodeUnaryOp(prio, unary_op, optxtx));
                }
                if (!ok) {
                    error = QString("parse stuck at \"%1\"").arg(QString(ch));
                    break;
                }
            }
        } else {
            if (!in_word) { word_start = i; }
            in_word = true;
        }
    }

    return ok;
}

FmeExpression::~FmeExpression() {
    delete root;
    root = nullptr;
}

bool FmeExpression::eval(
    FmeValue &value,
    FmeSymbolDb *symdb,
    QString &error,
    machine::Address inst_addr) {
    if (!root) { return false; }
    return root->eval(value, symdb, error, inst_addr);
}

bool FmeExpression::insert(FmeNode *node) {
    root = node;
    return true;
}

FmeNode *FmeExpression::child() {
    return root;
}

QString FmeExpression::dump() {
    return "(" + (root ? root->dump() : "nullptr") + ")";
}
