#include "cop0dock.h"

Cop0Dock::Cop0Dock(QWidget *parent) : QDockWidget(parent) {
    scrollarea = new QScrollArea(this);
    scrollarea->setWidgetResizable(true);
    widg = new StaticTable(scrollarea);

#define INIT(X, LABEL)                                                         \
    do {                                                                       \
        (X) = new QLabel("0x00000000", widg);                                  \
        (X)->setFixedSize((X)->sizeHint());                                    \
        (X)->setText("");                                                      \
        (X)->setTextInteractionFlags(Qt::TextSelectableByMouse);               \
        widg->addRow({ new QLabel(LABEL, widg), X });                          \
    } while (false)

    cop0reg[0] = nullptr;
    for (int i = 1; i < machine::Cop0State::COP0REGS_CNT; i++) {
        INIT(
            cop0reg[i], machine::Cop0State::cop0reg_name(
                            (machine::Cop0State::Cop0Registers)i));
        cop0reg_highlighted[i] = false;
    }
#undef INIT
    scrollarea->setWidget(widg);

    setWidget(scrollarea);
    setObjectName("Coprocessor0");
    setWindowTitle("Coprocessor0");

    pal_normal = QPalette(cop0reg[1]->palette());
    pal_updated = QPalette(cop0reg[1]->palette());
    pal_read = QPalette(cop0reg[1]->palette());
    pal_normal.setColor(QPalette::WindowText, QColor(0, 0, 0));
    pal_updated.setColor(QPalette::WindowText, QColor(240, 0, 0));
    pal_read.setColor(QPalette::WindowText, QColor(0, 0, 240));
    cop0reg_highlighted_any = false;
}

Cop0Dock::~Cop0Dock() {
    for (int i = 1; i < machine::Cop0State::COP0REGS_CNT; i++) {
        delete cop0reg[i];
    }
    delete widg;
    delete scrollarea;
}

void Cop0Dock::setup(machine::Machine *machine) {
    if (machine == nullptr) {
        // Reset data
        for (int i = 1; i < machine::Cop0State::COP0REGS_CNT; i++) {
            cop0reg[i]->setText("");
        }
        return;
    }

    const machine::Cop0State *cop0state = machine->cop0state();

    for (int i = 1; i < machine::Cop0State::COP0REGS_CNT; i++) {
        labelVal(
            cop0reg[i],
            cop0state->read_cop0reg((machine::Cop0State::Cop0Registers)i));
    }

    connect(
        cop0state, &machine::Cop0State::cop0reg_update, this,
        &Cop0Dock::cop0reg_changed);
    connect(
        cop0state, &machine::Cop0State::cop0reg_read, this,
        &Cop0Dock::cop0reg_read);
    connect(
        machine, &machine::Machine::tick, this, &Cop0Dock::clear_highlights);
}

void Cop0Dock::cop0reg_changed(
    enum machine::Cop0State::Cop0Registers reg,
    uint32_t val) {
    SANITY_ASSERT(
        (uint)reg < machine::Cop0State::COP0REGS_CNT && (uint)reg,
        QString("Cop0Dock received signal with invalid cop0 register: ")
            + QString::number((uint)reg));
    labelVal(cop0reg[(uint)reg], val);
    cop0reg[reg]->setPalette(pal_updated);
    cop0reg_highlighted[reg] = true;
    cop0reg_highlighted_any = true;
}

void Cop0Dock::cop0reg_read(
    enum machine::Cop0State::Cop0Registers reg,
    uint32_t val) {
    (void)val;
    SANITY_ASSERT(
        (uint)reg < machine::Cop0State::COP0REGS_CNT && (uint)reg,
        QString("Cop0Dock received signal with invalid cop0 register: ")
            + QString::number((uint)reg));
    if (!cop0reg_highlighted[reg]) {
        cop0reg[reg]->setPalette(pal_read);
    }
    cop0reg_highlighted[reg] = true;
    cop0reg_highlighted_any = true;
}

void Cop0Dock::clear_highlights() {
    if (!cop0reg_highlighted_any) {
        return;
    }
    for (int i = 1; i < machine::Cop0State::COP0REGS_CNT; i++) {
        if (cop0reg_highlighted[i]) {
            cop0reg[i]->setPalette(pal_normal);
            cop0reg_highlighted[i] = false;
        }
    }
    cop0reg_highlighted_any = false;
}

void Cop0Dock::labelVal(QLabel *label, uint32_t value) {
    QString t = QString("0x") + QString::number(value, 16);
    label->setText(t);
}
