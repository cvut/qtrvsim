#include "hidingtabwidget.h"

void HidingTabWidget::tabInserted(int index) {
    QTabWidget::tabInserted(index);
    if (count() == 1) {
        show();
        requestSetVisible(this, true);
    }
}
void HidingTabWidget::tabRemoved(int index) {
    QTabWidget::tabRemoved(index);
    if (count() == 0) {
        hide();
        requestSetVisible(this, false);
    }
}
void HidingTabWidget::setTabVisibleRequested(QWidget *tab, bool visible) {
    setTabVisible(indexOf(tab), visible);
}
