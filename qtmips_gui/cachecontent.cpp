#include "cachecontent.h"

CacheContentDock::CacheContentDock(QWidget *parent) : QDockWidget(parent) {
    ui = new Ui::CacheContent();
    ui->setupUi(this);
}

CacheContentDock::~CacheContentDock() {
    delete ui;
}
