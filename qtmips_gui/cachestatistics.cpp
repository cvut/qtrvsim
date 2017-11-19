#include "cachestatistics.h"

CacheStatisticsDock::CacheStatisticsDock(QWidget *parent) : QDockWidget(parent) {
    ui = new Ui::CacheStatistics();
    ui->setupUi(this);
}

CacheStatisticsDock::~CacheStatisticsDock() {
    delete ui;
}
