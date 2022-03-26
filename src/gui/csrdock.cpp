#include "csrdock.h"

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

    csr_view[0] = nullptr;
    for (int i = 1; i < machine::ControlState::CSR_REGS_CNT; i++) {
        INIT(csr_view[i], machine::ControlState::csr_name((machine::ControlState::CsrRegisters)i));
        csr_highlighted[i] = false;
    }
#undef INIT
    scrollarea->setWidget(widg);

    setWidget(scrollarea);
    setObjectName("Control and Status Registers");
    setWindowTitle("Control and Status Registers");

    pal_normal = QPalette(csr_view[1]->palette());
    pal_updated = QPalette(csr_view[1]->palette());
    pal_read = QPalette(csr_view[1]->palette());
    pal_normal.setColor(QPalette::WindowText, QColor(0, 0, 0));
    pal_updated.setColor(QPalette::WindowText, QColor(240, 0, 0));
    pal_read.setColor(QPalette::WindowText, QColor(0, 0, 240));
    csr_highlighted_any = false;
}

Cop0Dock::~Cop0Dock() {
    for (int i = 1; i < machine::ControlState::CSR_REGS_CNT; i++) {
        delete csr_view[i];
    }
    delete widg;
    delete scrollarea;
}

void Cop0Dock::setup(machine::Machine *machine) {
    if (machine == nullptr) {
        // Reset data
        for (int i = 1; i < machine::ControlState::CSR_REGS_CNT; i++) {
            csr_view[i]->setText("");
        }
        return;
    }

    const machine::ControlState *cop0state = machine->control_state();

    for (int i = 1; i < machine::ControlState::CSR_REGS_CNT; i++) {
        labelVal(
            csr_view[i],
            cop0state->read_csr((machine::ControlState::CsrRegisters)i));
    }

    connect(
        cop0state, &machine::ControlState::csr_update, this,
        &Cop0Dock::csr_changed);
    connect(
        cop0state, &machine::ControlState::csr_read, this,
        &Cop0Dock::csr_read);
    connect(
        machine, &machine::Machine::tick, this, &Cop0Dock::clear_highlights);
}

void Cop0Dock::csr_changed(enum machine::ControlState::CsrRegisters reg,
    uint64_t val) {
    SANITY_ASSERT(
        (uint)reg < machine::ControlState::CSR_REGS_CNT && (uint)reg,
        QString("Cop0Dock received signal with invalid cop0 register: ")
            + QString::number((uint)reg));
    labelVal(csr_view[(uint)reg], val);
    csr_view[reg]->setPalette(pal_updated);
    csr_highlighted[reg] = true;
    csr_highlighted_any = true;
}

void Cop0Dock::csr_read(
    enum machine::ControlState::CsrRegisters reg,
    uint64_t val) {
    (void)val;
    SANITY_ASSERT(
        (uint)reg < machine::ControlState::CSR_REGS_CNT && (uint)reg,
        QString("Cop0Dock received signal with invalid cop0 register: ")
            + QString::number((uint)reg));
    if (!csr_highlighted[reg]) {
        csr_view[reg]->setPalette(pal_read);
    }
    csr_highlighted[reg] = true;
    csr_highlighted_any = true;
}

void Cop0Dock::clear_highlights() {
    if (!csr_highlighted_any) {
        return;
    }
    for (int i = 1; i < machine::ControlState::CSR_REGS_CNT; i++) {
        if (csr_highlighted[i]) {
            csr_view[i]->setPalette(pal_normal);
            csr_highlighted[i] = false;
        }
    }
    csr_highlighted_any = false;
}

void Cop0Dock::labelVal(QLabel *label, uint64_t value) {
    QString t = QString("0x") + QString::number(value, 16);
    label->setText(t);
}
