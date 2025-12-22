#include "tlbdock.h"

TLBDock::TLBDock(QWidget *parent, const QString &type)
    : QDockWidget(parent)
    , tlbscene(nullptr)
    , connected_tlb(nullptr) {
    top_widget = new QWidget(this);
    setWidget(top_widget);
    layout_box = new QVBoxLayout(top_widget);

    top_form = new QWidget(top_widget);
    top_form->setVisible(false);
    layout_box->addWidget(top_form);
    layout_top_form = new QFormLayout(top_form);

    l_hit = new QLabel("0", top_form);
    l_hit->setTextFormat(Qt::PlainText);
    layout_top_form->addRow("Hit:", l_hit);

    l_miss = new QLabel("0", top_form);
    l_miss->setTextFormat(Qt::PlainText);
    layout_top_form->addRow("Miss:", l_miss);

    l_m_reads = new QLabel("0", top_form);
    l_m_reads->setTextFormat(Qt::PlainText);
    layout_top_form->addRow("Memory reads:", l_m_reads);

    l_m_writes = new QLabel("0", top_form);
    l_m_writes->setTextFormat(Qt::PlainText);
    layout_top_form->addRow("Memory writes:", l_m_writes);

    l_stalled = new QLabel("0", top_form);
    l_stalled->setTextFormat(Qt::PlainText);
    layout_top_form->addRow("Memory stall cycles:", l_stalled);

    l_hit_rate = new QLabel("0.000%", top_form);
    l_hit_rate->setTextFormat(Qt::PlainText);
    layout_top_form->addRow("Hit rate:", l_hit_rate);

    l_speed = new QLabel("100%", top_form);
    l_speed->setTextFormat(Qt::PlainText);
    layout_top_form->addRow("Improved speed:", l_speed);

    graphicsview = new GraphicsView(top_widget);
    graphicsview->setVisible(false);
    layout_box->addWidget(graphicsview);
    tlbscene = nullptr;

    no_tlb = new QLabel("No " + type + " TLB configured", top_widget);
    layout_box->addWidget(no_tlb);

    setObjectName(type + "TLB");
    setWindowTitle(type + " TLB");
}

void TLBDock::setup(machine::TLB *tlb) {
    memory_reads = 0;
    memory_writes = 0;
    hit = 0;
    miss = 0;
    stalled = 0;
    speed_improv = 0.0;
    hit_rate = 0.0;

    l_hit->setText("0");
    l_miss->setText("0");
    l_stalled->setText("0");
    l_m_reads->setText("0");
    l_m_writes->setText("0");
    l_hit_rate->setText("0.000%");
    l_speed->setText("100%");

    if (tlb != nullptr) {
        connect(tlb, &machine::TLB::hit_update, this, &TLBDock::hit_update, Qt::UniqueConnection);
        connect(tlb, &machine::TLB::miss_update, this, &TLBDock::miss_update, Qt::UniqueConnection);
        connect(
            tlb, &machine::TLB::statistics_update, this, &TLBDock::statistics_update,
            Qt::UniqueConnection);
        connect(
            tlb, &machine::TLB::memory_reads_update, this, &TLBDock::memory_reads_update,
            Qt::UniqueConnection);
        connect(
            tlb, &machine::TLB::memory_writes_update, this, &TLBDock::memory_writes_update,
            Qt::UniqueConnection);
    }
    connected_tlb = const_cast<machine::TLB *>(tlb);
    top_form->setVisible(tlb != nullptr);
    no_tlb->setVisible(tlb == nullptr);

    delete tlbscene;
    tlbscene = nullptr;
    if (tlb != nullptr) {
        tlbscene = new TLBViewScene(tlb);
        graphicsview->setScene(tlbscene);
    } else {
        graphicsview->setScene(nullptr);
    }
    graphicsview->setVisible(tlb != nullptr);
}

void TLBDock::paintEvent(QPaintEvent *event) {
    l_stalled->setText(QString::number(stalled));
    l_hit_rate->setText(QString::number(hit_rate, 'f', 3) + QString("%"));
    l_speed->setText(QString::number(speed_improv, 'f', 0) + QString("%"));
    l_hit->setText(QString::number(hit));
    l_miss->setText(QString::number(miss));
    l_m_reads->setText(QString::number(memory_reads));
    l_m_writes->setText(QString::number(memory_writes));
    QDockWidget::paintEvent(event);
}

void TLBDock::hit_update(unsigned val) {
    hit = val;
    update();
}

void TLBDock::miss_update(unsigned val) {
    miss = val;
    update();
}

void TLBDock::statistics_update(unsigned stalled_cycles, double speed_improv_v, double hit_rate_v) {
    stalled = stalled_cycles;
    speed_improv = speed_improv_v;
    hit_rate = hit_rate_v;
    update();
}

void TLBDock::memory_reads_update(unsigned val) {
    memory_reads = val;
    l_m_reads->setText(QString::number(memory_reads));
    update();
}

void TLBDock::memory_writes_update(unsigned val) {
    memory_writes = val;
    l_m_writes->setText(QString::number(memory_writes));
    update();
}
