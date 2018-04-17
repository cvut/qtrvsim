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
    if (cache->config().enabled()) {
        connect(cache, SIGNAL(hit_update(uint)), this, SLOT(hit_update(uint)));
        connect(cache, SIGNAL(miss_update(uint)), this, SLOT(miss_update(uint)));
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
