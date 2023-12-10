#include "cachedock.h"

CacheDock::CacheDock(QWidget *parent, const QString &type)
    : QDockWidget(parent) {
    top_widget = new QWidget(this);
    setWidget(top_widget);
    layout_box = new QVBoxLayout(top_widget);

    top_form = new QWidget(top_widget);
    top_form->setVisible(false);
    layout_box->addWidget(top_form);
    layout_top_form = new QFormLayout(top_form);

    l_hit = new QLabel("0", top_form);
    layout_top_form->addRow("Hit:", l_hit);
    l_miss = new QLabel("0", top_form);
    layout_top_form->addRow("Miss:", l_miss);
    l_m_reads = new QLabel("0", top_form);
    layout_top_form->addRow("Memory reads:", l_m_reads);
    l_m_writes = new QLabel("0", top_form);
    layout_top_form->addRow("Memory writes:", l_m_writes);
    l_stalled = new QLabel("0", top_form);
    layout_top_form->addRow("Memory stall cycles:", l_stalled);
    l_hit_rate = new QLabel("0.000%", top_form);
    layout_top_form->addRow("Hit rate:", l_hit_rate);
    l_speed = new QLabel("100%", top_form);
    layout_top_form->addRow("Improved speed:", l_speed);

    graphicsview = new GraphicsView(top_widget);
    graphicsview->setVisible(false);
    layout_box->addWidget(graphicsview);
    cachescene = nullptr;

    no_cache = new QLabel("No " + type + " Cache configured", top_widget);
    layout_box->addWidget(no_cache);

    setObjectName(type + "Cache");
    setWindowTitle(type + " Cache");
}

void CacheDock::setup(const machine::Cache *cache, bool cache_after_cache) {
    l_hit->setText("0");
    l_miss->setText("0");
    l_stalled->setText("0");
    l_m_reads->setText("0");
    l_m_writes->setText("0");
    l_hit_rate->setText("0.000%");
    l_speed->setText("100%");
    l_speed->setHidden(cache_after_cache);
    if (cache != nullptr) {
        connect(
            cache, &machine::Cache::hit_update, this, &CacheDock::hit_update);
        connect(
            cache, &machine::Cache::miss_update, this, &CacheDock::miss_update);
        connect(
            cache, &machine::Cache::memory_reads_update, this,
            &CacheDock::memory_reads_update);
        connect(
            cache, &machine::Cache::memory_writes_update, this,
            &CacheDock::memory_writes_update);
        connect(
            cache, &machine::Cache::statistics_update, this,
            &CacheDock::statistics_update);
    }
    top_form->setVisible(cache != nullptr);
    no_cache->setVisible(cache == nullptr || !cache->get_config().enabled());

    delete cachescene;
    cachescene = new CacheViewScene(cache);
    graphicsview->setScene(cachescene);
    graphicsview->setVisible(cache != nullptr && cache->get_config().enabled());
}

void CacheDock::hit_update(unsigned val) {
    l_hit->setText(QString::number(val));
}

void CacheDock::miss_update(unsigned val) {
    l_miss->setText(QString::number(val));
}

void CacheDock::memory_reads_update(unsigned val) {
    l_m_reads->setText(QString::number(val));
}

void CacheDock::memory_writes_update(unsigned val) {
    l_m_writes->setText(QString::number(val));
}

void CacheDock::statistics_update(
    unsigned stalled_cycles,
    double speed_improv,
    double hit_rate) {
    l_stalled->setText(QString::number(stalled_cycles));
    l_hit_rate->setText(QString::number(hit_rate, 'f', 3) + QString("%"));
    l_speed->setText(QString::number(speed_improv, 'f', 0) + QString("%"));
}
