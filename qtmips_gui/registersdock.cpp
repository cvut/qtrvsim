#include "registersdock.h"

RegistersDock::RegistersDock(QWidget *parent) : QDockWidget(parent) {
    ui = new Ui::RegistersDock();
    ui->setupUi(this);
}

RegistersDock::~RegistersDock() {
    delete ui;
}
