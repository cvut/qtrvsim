#ifndef TLBDOCK_H
#define TLBDOCK_H

#include "graphicsview.h"
#include "machine/machine.h"
#include "tlbview.h"

#include <QDockWidget>
#include <QFormLayout>
#include <QLabel>
#include <QPointer>

class TLBDock : public QDockWidget {
    Q_OBJECT
public:
    TLBDock(QWidget *parent, const QString &type);

    void setup(machine::TLB *tlb);

    void paintEvent(QPaintEvent *event) override;

private slots:
    void hit_update(unsigned val);
    void miss_update(unsigned val);
    void statistics_update(unsigned stalled_cycles, double speed_improv, double hit_rate);
    void memory_reads_update(unsigned val);
    void memory_writes_update(unsigned val);

private:
    QVBoxLayout *layout_box;
    QWidget *top_widget;
    QWidget *top_form;
    QFormLayout *layout_top_form;

    QLabel *l_hit;
    QLabel *l_miss;
    QLabel *l_stalled;
    QLabel *l_speed;
    QLabel *l_hit_rate;
    QLabel *no_tlb;
    QLabel *l_m_reads;
    QLabel *l_m_writes;

    GraphicsView *graphicsview;
    TLBViewScene *tlbscene;

    unsigned memory_reads = 0;
    unsigned memory_writes = 0;
    unsigned hit = 0;
    unsigned miss = 0;
    unsigned stalled = 0;
    double speed_improv = 0.0;
    double hit_rate = 0.0;

    QPointer<machine::TLB> connected_tlb;
};
#endif // TLBDOCK_H
