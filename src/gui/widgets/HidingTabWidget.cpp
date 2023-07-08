#include "HidingTabWidget.h"

void HidingTabWidget::tabInserted(int index) {
    QTabWidget::tabInserted(index);
    if (count() == 1) { show(); }
}
void HidingTabWidget::tabRemoved(int index) {
    QTabWidget::tabRemoved(index);
    if (count() == 0) { hide(); }
}
