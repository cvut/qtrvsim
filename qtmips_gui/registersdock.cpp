#include "registersdock.h"

RegistersDock::RegistersDock(QWidget *parent) : QDockWidget(parent) {
    scrollarea = new QScrollArea(this);
    widg = new QWidget(scrollarea);
    layout = new QFormLayout(widg);
    layout->setSizeConstraint(QLayout::SetMinAndMaxSize);

#define INIT(X, LABEL) do{ \
        X = new QLabel(widg); \
        X->setTextInteractionFlags(Qt::TextSelectableByMouse); \
        layout->addRow(LABEL, X); \
    } while(false)

    INIT(pc, "PC:");
    for (int i = 0; i < 32; i++)
        INIT(gp[i], QString("GP") + QString::number(i) + QString(" ($") + QString::number(i) + QString("):"));
    INIT(lo, "LO:");
    INIT(hi, "HI:");
#undef INIT
    widg->setLayout(layout);
    scrollarea->setWidget(widg);

    setWidget(scrollarea);
    setObjectName("Registers");
    setWindowTitle("Registers");
}

RegistersDock::~RegistersDock() {
    delete pc, hi, lo;
    for (int i = 0; i < 32; i++)
        delete gp[i];
    delete layout;
    delete widg;
    delete scrollarea;
}

void RegistersDock::setup(machine::QtMipsMachine *machine) {
    if (machine == nullptr) {
        // Reset data
        pc->setText("");
        hi->setText("");
        lo->setText("");
        for (int i = 0; i < 32; i++)
            gp[i]->setText("");
    }

    const machine::Registers *regs = machine->registers();
    connect(regs, SIGNAL(pc_update(std::uint32_t)), this, SLOT(pc_changed(std::uint32_t)));
    connect(regs, SIGNAL(gp_update(std::uint8_t,std::uint32_t)), this, SLOT(gp_changed(std::uint8_t,std::uint32_t)));
    connect(regs, SIGNAL(hi_lo_update(bool,std::uint32_t)), this, SLOT(hi_lo_changed(bool,std::uint32_t)));

    // Load values
    labelVal(pc, regs->read_pc(), false);
    labelVal(hi, regs->read_hi_lo(true), true);
    labelVal(lo, regs->read_hi_lo(false), true);
    for (int i = 0; i < 32; i++)
        labelVal(gp[i], regs->read_gp(i), true);
}

void RegistersDock::pc_changed(std::uint32_t val) {
    labelVal(pc, val, false);
}

void RegistersDock::gp_changed(std::uint8_t i, std::uint32_t val) {
    SANITY_ASSERT(i < 32, QString("RegistersDock received signal with invalid gp register: ") + QString::number(i));
    labelVal(gp[i], val, true);
}

void RegistersDock::hi_lo_changed(bool hi, std::uint32_t val) {
    if (hi)
        labelVal(this->hi, val, true);
    else
        labelVal(lo, val, true);
}

void RegistersDock::labelVal(QLabel *label, std::uint32_t value, bool dec) {
    QString t = QString("0x") + QString::number(value, 16);
    if (dec)
        t += QString(" (") + QString::number(value) + QString(")");
    label->setText(t);
}
