#include "registersdock.h"

// This table also exists in machine/instruction.cpp...
const QString labels[32] = {
    "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
    "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
    "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
};

RegistersDock::RegistersDock(QWidget *parent) : QDockWidget(parent) {
    static const QColor defaultTextColor(0, 0, 0);

    scrollarea = new QScrollArea(this);
    scrollarea->setWidgetResizable(true);
    widg = new StaticTable(scrollarea);
    gp_highlighted = 0;
    hi_highlighted = false;
    lo_highlighted = false;

    pal_normal = palette();
    pal_normal.setColor(QPalette::WindowText, defaultTextColor);

#define INIT(X, LABEL)                                                         \
    do {                                                                       \
        (X) = new QLabel("0x00000000", widg);                                  \
        (X)->setFixedSize((X)->sizeHint());                                    \
        (X)->setText("");                                                      \
        (X)->setPalette(pal_normal);                                           \
        (X)->setTextInteractionFlags(Qt::TextSelectableByMouse);               \
        QLabel *l = new QLabel(LABEL, widg);                                   \
        l->setPalette(pal_normal);                                             \
        widg->addRow({ l, X });                                                \
    } while (false)

    for (int i = 0; i < 32; i++) {
        INIT(gp[i], QString("x") + QString::number(i) + QString("/") + labels[i]);
    }
    INIT(pc, "pc");
    INIT(lo, "lo");
    INIT(hi, "hi");
#undef INIT
    scrollarea->setWidget(widg);

    setWidget(scrollarea);
    setObjectName("Registers");
    setWindowTitle("Registers");

    pal_updated = pal_normal;
    pal_read = pal_normal;
    pal_updated.setColor(QPalette::WindowText, QColor(240, 0, 0));
    pal_read.setColor(QPalette::WindowText, QColor(0, 0, 240));
}

RegistersDock::~RegistersDock() {
    delete pc;
    delete hi;
    delete lo;
    for (auto &i : gp) {
        delete i;
    }
    delete widg;
    delete scrollarea;
}

void RegistersDock::setup(machine::Machine *machine) {
    if (machine == nullptr) {
        // Reset data
        pc->setText("");
        hi->setText("");
        lo->setText("");
        for (auto &i : gp) {
            i->setText("");
        }
        return;
    }

    const machine::Registers *regs = machine->registers();

    // Load values
    labelVal(pc, regs->read_pc().get_raw());
    labelVal(hi, regs->read_hi_lo(true).as_u32());
    labelVal(lo, regs->read_hi_lo(false).as_u32());
    for (int i = 0; i < 32; i++) {
        labelVal(gp[i], regs->read_gp(i).as_u32());
    }

    connect(
        regs, &machine::Registers::pc_update, this, &RegistersDock::pc_changed);
    connect(
        regs, &machine::Registers::gp_update, this, &RegistersDock::gp_changed);
    connect(
        regs, &machine::Registers::hi_lo_update, this,
        &RegistersDock::hi_lo_changed);
    connect(regs, &machine::Registers::gp_read, this, &RegistersDock::gp_read);
    connect(
        regs, &machine::Registers::hi_lo_read, this,
        &RegistersDock::hi_lo_read);
    connect(
        machine, &machine::Machine::tick, this,
        &RegistersDock::clear_highlights);
}

void RegistersDock::pc_changed(machine::Address val) {
    labelVal(pc, val.get_raw());
}

void RegistersDock::gp_changed(
    machine::RegisterId i,
    machine::RegisterValue val) {
    SANITY_ASSERT(
        i.data < 32,
        QString("RegistersDock received signal with invalid gp register: ")
            + QString::number(i.data));
    labelVal(gp[i.data], val.as_u32());
    gp[i.data]->setPalette(pal_updated);
    gp_highlighted |= 1 << i.data;
}

void RegistersDock::gp_read(machine::RegisterId i, machine::RegisterValue val) {
    (void)val;
    SANITY_ASSERT(
        i.data < 32,
        QString("RegistersDock received signal with invalid gp register: ")
            + QString::number(i.data));
    if (!(gp_highlighted & (1 << i.data))) {
        gp[i.data]->setPalette(pal_read);
        gp_highlighted |= 1 << i.data;
    }
}

void RegistersDock::hi_lo_changed(bool hi, machine::RegisterValue val) {
    if (hi) {
        labelVal(this->hi, val.as_u32());
        this->hi->setPalette(pal_updated);
        hi_highlighted = true;
    } else {
        labelVal(lo, val.as_u32());
        this->lo->setPalette(pal_updated);
        lo_highlighted = true;
    }
}

void RegistersDock::hi_lo_read(bool hi, machine::RegisterValue val) {
    (void)val;
    if (hi) {
        if (!hi_highlighted) {
            this->hi->setPalette(pal_read);
        }
        hi_highlighted = true;
    } else {
        if (!lo_highlighted) {
            this->lo->setPalette(pal_read);
        }
        lo_highlighted = true;
    }
}

void RegistersDock::clear_highlights() {
    if (hi_highlighted) {
        this->hi->setPalette(pal_normal);
    }
    if (lo_highlighted) {
        this->lo->setPalette(pal_normal);
    }
    if (gp_highlighted != 0) {
        for (int i = 0; i < 32; i++) {
            if (gp_highlighted & (1 << i)) {
                gp[i]->setPalette(pal_normal);
            }
        }
    }
    gp_highlighted = 0;
    hi_highlighted = false;
    lo_highlighted = false;
}

void RegistersDock::labelVal(QLabel *label, uint32_t value) {
    QString t = QString("0x") + QString::number(value, 16);
    label->setText(t);
}
