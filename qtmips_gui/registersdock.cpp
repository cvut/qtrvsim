#include "registersdock.h"

static const QString labels[] = {
    "zero",
    "at",
    "v0",
    "v1",
    "a0",
    "a1",
    "a2",
    "a3",
    "t0",
    "t1",
    "t2",
    "t3",
    "t4",
    "t5",
    "t6",
    "t7",
    "s0",
    "s1",
    "s2",
    "s3",
    "s4",
    "s5",
    "s6",
    "s7",
    "t8",
    "t9",
    "k0",
    "k1",
    "gp",
    "sp",
    "fp",
    "ra"
};

RegistersDock::RegistersDock(QWidget *parent) : QDockWidget(parent) {
    scrollarea = new QScrollArea(this);
    scrollarea->setWidgetResizable(true);
    widg = new StaticTable(scrollarea);

#define INIT(X, LABEL) do{ \
        X = new QLabel("0x00000000", widg); \
        X->setFixedSize(X->sizeHint()); \
        X->setText(""); \
        X->setTextInteractionFlags(Qt::TextSelectableByMouse); \
        widg->addRow({new QLabel(LABEL, widg), X}); \
    } while(false)

    for (int i = 0; i < 32; i++)
        INIT(gp[i], QString("$") + QString::number(i) + QString("/") + labels[i]);
    INIT(pc, "pc");
    INIT(lo, "lo");
    INIT(hi, "hi");
#undef INIT
    scrollarea->setWidget(widg);

    setWidget(scrollarea);
    setObjectName("Registers");
    setWindowTitle("Registers");
}

RegistersDock::~RegistersDock() {
    delete pc;
    delete hi;
    delete lo;
    for (int i = 0; i < 32; i++)
        delete gp[i];
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
    labelVal(pc, regs->read_pc());
    labelVal(hi, regs->read_hi_lo(true));
    labelVal(lo, regs->read_hi_lo(false));
    for (int i = 0; i < 32; i++)
        labelVal(gp[i], regs->read_gp(i));
}

void RegistersDock::pc_changed(std::uint32_t val) {
    labelVal(pc, val);
}

void RegistersDock::gp_changed(std::uint8_t i, std::uint32_t val) {
    SANITY_ASSERT(i < 32, QString("RegistersDock received signal with invalid gp register: ") + QString::number(i));
    labelVal(gp[i], val);
}

void RegistersDock::hi_lo_changed(bool hi, std::uint32_t val) {
    if (hi)
        labelVal(this->hi, val);
    else
        labelVal(lo, val);
}

void RegistersDock::labelVal(QLabel *label, std::uint32_t value) {
    QString t = QString("0x") + QString::number(value, 16);
    label->setText(t);
}
