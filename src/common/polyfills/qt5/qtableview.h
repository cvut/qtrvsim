#ifndef POLYFILLS_QTABLEVIEW_H
#define POLYFILLS_QTABLEVIEW_H

#include <QTableView>

/**
 * QTableView polyfill
 *
 * initViewItemOption is protected, therefore the whole class needs to be wrapped.
 */
class Poly_QTableView : public QTableView {
public:
    explicit Poly_QTableView(QWidget *parent) : QTableView(parent) {}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
protected:
    void initViewItemOption(QStyleOptionViewItem *viewOpts) {
        *viewOpts = QTableView::viewOptions();
    }
#endif
};

#endif // QTMIPS_QTABLEVIEW_H
