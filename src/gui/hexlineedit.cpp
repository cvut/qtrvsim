#include "hexlineedit.h"

HexLineEdit::HexLineEdit(
    QWidget *parent,
    int digits,
    int base,
    const QString &prefix)
    : Super(parent) {
    this->base = base;
    this->digits = digits;
    this->prefix = prefix;
    last_set = 0;
    QChar dmask;
    QString t = "";
    QString mask = "";

    for (int i = 0; i < prefix.count(); i++) {
        mask += "\\" + QString(prefix.at(i));
    }
    switch (base) {
    case 10:
        mask += "D";
        dmask = 'd';
        break;
    case 2:
        mask += "B";
        dmask = 'b';
        break;
    case 16:
    case 0:
    default:
        mask += "H";
        dmask = 'h';
        break;
    }
    if (digits > 1) {
        t.fill(dmask, digits - 1);
    }

    mask += t;

    setInputMask(mask);

    connect(
        this, &QLineEdit::editingFinished, this,
        &HexLineEdit::on_edit_finished);

    set_value(0);
}

void HexLineEdit::set_value(uint32_t value) {
    QString s, t = "";
    last_set = value;
    s = QString::number(value, base);
    if (s.count() < digits) {
        t.fill('0', digits - s.count());
    }
    setText(prefix + t + s);
}

void HexLineEdit::on_edit_finished() {
    bool ok;
    uint32_t val;
    val = text().toULong(&ok, 16);
    if (!ok) {
        set_value(last_set);
        return;
    }
    last_set = val;
    emit value_edit_finished(val);
}
