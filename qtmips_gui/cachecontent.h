#ifndef CACHECONTENT_H
#define CACHECONTENT_H

#include <QDockWidget>
#include "ui_CacheContent.h"

class CacheContentDock : public QDockWidget {
public:
    CacheContentDock(QWidget *parent);
    ~CacheContentDock();

private:
    Ui::CacheContent *ui;
};

#endif // CACHECONTENT_H
