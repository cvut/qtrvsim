#include "pow2spinbox.h"

Pow2SpinBox::Pow2SpinBox(QWidget *parent) : QSpinBox(parent) {
    setRange(1, 1024);
    setValue(1);
}

QValidator::State Pow2SpinBox::validate(QString &input, int &pos) const {
    Q_UNUSED(pos);

    if (input.isEmpty()) return QValidator::Intermediate;

    bool ok = false;
    qint64 v = input.toLongLong(&ok);
    if (!ok || v <= 0) return QValidator::Invalid;

    if ((v & (v - 1)) == 0) return QValidator::Acceptable;

    return QValidator::Intermediate;
}

int Pow2SpinBox::valueFromText(const QString &text) const {
    return text.toInt();
}

QString Pow2SpinBox::textFromValue(int value) const {
    return QString::number(value);
}

void Pow2SpinBox::stepBy(int steps) {
    int v = value();
    if (v < 1) v = 1;

    auto isPow2 = [](int x) { return x > 0 && (x & (x - 1)) == 0; };

    auto nextPow2 = [](int x) -> int {
        if (x <= 1) return 1;
        int p = 1;
        while (p < x && (p << 1) > 0) {
            p <<= 1;
        }
        return p;
    };

    auto prevPow2 = [](int x) -> int {
        if (x <= 1) return 1;
        int p = 1;
        while ((p << 1) <= x) {
            p <<= 1;
        }
        if (p > x) { p >>= 1; }
        return p;
    };

    if (steps > 0) {
        if (!isPow2(v)) {
            v = nextPow2(v);
        } else {
            for (int i = 0; i < steps; ++i) {
                if (v > (maximum() >> 1)) {
                    v = maximum();
                    break;
                }
                v <<= 1;
            }
        }
    } else {
        if (!isPow2(v)) {
            v = prevPow2(v);
        } else {
            for (int i = 0; i < -steps; ++i) {
                if (v <= 1) {
                    v = 1;
                    break;
                }
                v >>= 1;
            }
        }
    }
    setValue(qBound(minimum(), v, maximum()));
}
