#include "hidingtabwidget.h"

void HidingTabWidget::tabInserted(int index) {
    QTabWidget::tabInserted(index);
    if (count() == 1) {
        show();
        requestAddRemoveTab(this, true);
    }
}
void HidingTabWidget::tabRemoved(int index) {
    QTabWidget::tabRemoved(index);
    if (count() == 0) {
        hide();
        requestAddRemoveTab(this, false);
    }
}
void HidingTabWidget::addRemoveTabRequested(QWidget *tab, bool exists) {
    if (!exists) {
        removeTab(indexOf(tab));
    } else {
        addTab(tab, tab->windowTitle());
    }
}
