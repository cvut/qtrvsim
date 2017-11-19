#ifndef REGISTERSDOCK_H
#define REGISTERSDOCK_H

#include <QDockWidget>
#include "ui_registersdock.h"

class RegistersDock : public QDockWidget {
public:
    RegistersDock(QWidget *parent);
    ~RegistersDock();

private:
    Ui::RegistersDock *ui;

};

#endif // REGISTERSDOCK_H
