#include "cachedock.h"

CacheDock::CacheDock(QWidget *parent, const QString &type) : QDockWidget(parent) {
    top_widget = new QWidget(this);
    setWidget(top_widget);
    layout_box = new QVBoxLayout(top_widget);

    no_cache = new QLabel("No " + type + " Cache configured", top_widget);
    layout_box->addWidget(no_cache);

    top_form = new QWidget(top_widget);
    top_form->setVisible(false);
    layout_box->addWidget(top_form);
    layout_top_form = new QFormLayout(top_form);

    l_hit = new QLabel("0", top_form);
    layout_top_form->addRow("Hit:", l_hit);
    l_miss = new QLabel("0", top_form);
    layout_top_form->addRow("Miss:", l_miss);
    l_stalled = new QLabel("0", top_form);
    layout_top_form->addRow("Memory stall cycles:", l_stalled);
    l_usage = new QLabel("0.000%", top_form);
    layout_top_form->addRow("Usage effectiveness:", l_usage);
    l_speed  = new QLabel("100%", top_form);
    layout_top_form->addRow("Speed improvement:", l_speed);

    graphicsview = new GraphicsView(top_widget);
    graphicsview->setVisible(false);
    layout_box->addWidget(graphicsview);
    cachescene = nullptr;

    setObjectName(type + "Cache");
    setWindowTitle(type + " Cache");
}

void CacheDock::setup(const machine::Cache *cache) {
    l_hit->setText("0");
    l_miss->setText("0");
    l_stalled->setText("0");
    l_usage->setText("0.000%");
    l_speed->setText("100%");
    if (cache->config().enabled()) {
        connect(cache, SIGNAL(hit_update(uint)), this, SLOT(hit_update(uint)));
        connect(cache, SIGNAL(miss_update(uint)), this, SLOT(miss_update(uint)));
        connect(cache, SIGNAL(statistics_update(uint,double,double)), this, SLOT(statistics_update(uint,double,double)));
    }
    top_form->setVisible(cache->config().enabled());
    no_cache->setVisible(!cache->config().enabled());

    if (cachescene)
        delete cachescene;
    cachescene = new CacheViewScene(cache);
    graphicsview->setScene(cachescene);
    graphicsview->setVisible(cache->config().enabled());
}

void CacheDock::hit_update(unsigned val) {
    l_hit->setText(QString::number(val));
}

void CacheDock::miss_update(unsigned val) {
    l_miss->setText(QString::number(val));
}

void CacheDock::statistics_update(unsigned stalled_cycles, double speed_improv, double usage_effic) {
    l_stalled->setText(QString::number(stalled_cycles));
    l_usage->setText(QString::number(usage_effic, 'f', 3) + QString("%"));
    l_speed->setText(QString::number(speed_improv, 'f', 0) + QString("%"));
}
