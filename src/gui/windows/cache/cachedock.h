#ifndef CACHEDOCK_H
#define CACHEDOCK_H

#include "cacheview.h"
#include "graphicsview.h"
#include "machine/machine.h"

#include <QDockWidget>
#include <QFormLayout>
#include <QLabel>

class CacheDock : public QDockWidget {
    Q_OBJECT
public:
    CacheDock(QWidget *parent, const QString &type);

    void setup(const machine::Cache *cache, bool cache_after_cache = false);

    void paintEvent(QPaintEvent *event) override;

private slots:
    void hit_update(unsigned);
    void miss_update(unsigned);
    void memory_reads_update(unsigned val);
    void memory_writes_update(unsigned val);
    void statistics_update(unsigned stalled_cycles, double speed_improv, double hit_rate);

private:
    QVBoxLayout *layout_box;
    QWidget *top_widget, *top_form;
    QFormLayout *layout_top_form;
    QLabel *l_hit, *l_miss, *l_stalled, *l_speed, *l_hit_rate;
    QLabel *no_cache;
    QLabel *l_m_reads, *l_m_writes;
    GraphicsView *graphicsview;
    CacheViewScene *cachescene;

    // Statistics
    unsigned memory_reads = 0;
    unsigned memory_writes = 0;
    unsigned hit = 0;
    unsigned miss = 0;
    unsigned stalled = 0;
    double speed_improv = 0.0;
    double hit_rate = 0.0;
};

#endif // CACHEDOCK_H
