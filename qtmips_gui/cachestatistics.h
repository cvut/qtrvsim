#ifndef CACHESTATISTICS_H
#define CACHESTATISTICS_H

#include <QDockWidget>
#include "ui_CacheStatistics.h"

class CacheStatisticsDock : public QDockWidget {
    Q_OBJECT
public:
    CacheStatisticsDock(QWidget *parent);
    ~CacheStatisticsDock();

private:
    Ui::CacheStatistics *ui;

};

#endif // CACHESTATISTICS_H
