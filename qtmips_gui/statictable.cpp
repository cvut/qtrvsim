#include "statictable.h"
#include <QPainter>
#include <qtmipsexception.h>

StaticTableLayout::StaticTableLayout(QWidget *parent, int margin, int horizontal_big_spacing, int horizontal_small_spacing, int vertical_spacing) : QLayout(parent) {
    setContentsMargins(margin, margin, margin, margin);
    bhspace = horizontal_big_spacing;
    shspace = horizontal_small_spacing;
    vspace = vertical_spacing;
    setSizeConstraint(QLayout::SetMinAndMaxSize);

    cch_do_layout.size = QSize(0, 0);
    cch_do_layout.count = 0;
    cch_heightForWidth.w = 0;
    cch_heightForWidth.count = 0;
    cch_heightForWidth.width = 0;
    cch_minSize.count = 0;
    cch_minSize.size = QSize(0, 0);
}

StaticTableLayout::~StaticTableLayout() {
    for (int i = 0; i < items.size(); i++) {
        for (int y = 0; y < items[i].size(); y++)
            delete items[i][y];
    }
}

Qt::Orientations StaticTableLayout::expandingDirections() const {
    return Qt::Horizontal;
}

bool StaticTableLayout::hasHeightForWidth() const {
    return true;
}

int StaticTableLayout::heightForWidth(int w) const {
    if (cch_heightForWidth.w != w || cch_heightForWidth.count != items.count())
        cch_heightForWidth.width = layout_height(w);
    cch_heightForWidth.w = w;
    cch_heightForWidth.count = items.count();
    return cch_heightForWidth.width;
}

QSize StaticTableLayout::minimumSize() const {
    if (cch_minSize.count == items.size())
        return cch_minSize.size;
    cch_minSize.count = items.size();

    cch_minSize.size = QSize();
    for (int i = 0; i < items.size(); i++) {
        QSize ss;
        for (int y = 0; y < items[i].size(); y++) {
            ss = cch_minSize.size.expandedTo(items[i][y]->minimumSize() + QSize(shspace, 0));
        }
        cch_minSize.size = cch_minSize.size.expandedTo(ss - QSize(shspace, 0));
    }
    cch_minSize.size += QSize(2*margin(), 2*margin());
    return cch_minSize.size;
}

void StaticTableLayout::setGeometry(const QRect &rect) {
    QLayout::setGeometry(rect);
    do_layout(rect);
}

QSize StaticTableLayout::sizeHint() const {
    return minimumSize();
}

void StaticTableLayout::addItem(QLayoutItem *item __attribute__((unused))) {
    // Just implement it but it does nothing
}

QLayoutItem *StaticTableLayout::itemAt(int index __attribute__((unused))) const {
    return nullptr; // This is just dummy implementation to satisfy reimplementation
}

QLayoutItem *StaticTableLayout::takeAt(int index __attribute__((unused))) {
    return nullptr; // This is just dummy implementation to satisfy reimplementation
}

int StaticTableLayout::count() const {
    return items.size();
}

void StaticTableLayout::addRow(QList<QWidget*> w) {
    items.append(list2vec(w));
}

void StaticTableLayout::insertRow(QList<QWidget*> w, int i) {
    items.insert(i, list2vec(w));
}

void StaticTableLayout::removeRow(int i) {
    for (int y = 0; y < items[i].size(); y++) {
        delete items[i][y]->widget();
        delete items[i][y];
    }
    items.remove(i);
}

void StaticTableLayout::clearRows() {
    for (int i = 0; i < items.size(); i++)
        for (int y = 0; y < items[i].size(); y++) {
            delete items[i][y]->widget();
            delete items[i][y];
        }
    items.clear();
}

void StaticTableLayout::itemRect(QRect &rect, QVector<int> &separators, int i) {
    separators.clear();

    int row = i / columns();
    int col = i % columns();

    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);

    int x = left;
    for (int s = 0; s < col; s++) {
        for (int t = 0; t < row_widths[s].size(); t++)
            x += row_widths[s][t] + shspace;
        x += bhspace - shspace;
    }
    if (col > 0) // otherwise we are on left edge and there was no previous column so no big space
        x -= bhspace/2;
    int y = top + (row * (row_height + vspace));

    int width = 0;
    for (int t = 0; t < row_widths[col].size(); t++) {
        width += row_widths[col][t] + shspace;
        separators.append(width - shspace/2);
    }
    if (col <= 0)
        width -= bhspace/2;

    rect = QRect(x, y - vspace/2, width - shspace + bhspace, row_height + vspace);
    separators.removeLast(); // drop last separator as that one we don't want to see
}

int StaticTableLayout::columns() {
    return row_widths.size();
}

int StaticTableLayout::real_row_height() {
    return row_height + vspace;
}

int StaticTableLayout::layout_count_approx(const QRect &rect) const {
    if (items.size() <= 0 || rect.width() < rect.height())
        return 1;
    // Note: for some reason (probably optimalisation) when qlabel is not visible it reports 0 size. So we have to found at least one that is visible
    int vis = 0;
    while (items[vis].size() <= 0 || !items[vis][0]->widget()->isVisible()) {
        vis++;
        if (vis >= items.size())
            return 1; // If non is visible then just say that it has to be single column
    }

    int w = 0;
    for (int i = 0; i < items[vis].size(); i++)
        w += items[vis][i]->sizeHint().width() + shspace;
    w -= shspace; // subtract lastest spacing
    int width = rect.right() / w; // Note: this always rounds down so this always founds maximal possible count
    return width <= 0 ? 1 : width; // We have to fit at least one column
}

int StaticTableLayout::layout_size(int &row_h, QList<QList<int>> &row_w, int count) const {
    row_h = 0;
    row_w.clear();
    int col = 0;
    for (int i = 0; i < items.size(); i++) {
        if (row_w.size() <= col)
            row_w.append(QList<int>());
        for (int y = 0; y < items[i].size(); y++) {
            QSize s = items[i][y]->sizeHint();
            row_h = qMax(row_h, s.height());
            if (row_w[col].size() <= y)
                row_w[col].append(0);
            row_w[col][y] = qMax(row_w[col][y], s.width());
        }
        if (++col >= count)
            col = 0;
    }
    SANITY_ASSERT(row_w.size() <= count, "We should end up with maximum of count columns");

    int w = 0;
    for (int i = 0; i < row_w.size(); i++) {
        for (int y = 0; y < row_w[i].size(); y++)
            w += row_w[i][y] + shspace;
        w += bhspace - shspace; // subtract latest small spacing and add big spacing
    }
    w -= bhspace; // subtract latest big spacing
    return w;
}

void StaticTableLayout::layout_parms(QRect &rect, int &row_h, QList<QList<int>> &row_w, int &count) const {
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    rect = rect.adjusted(left, top, -right, -bottom);

    // Firt let's do orientation count only on first line
    count = layout_count_approx(rect);
    while (layout_size(row_h, row_w, count) > rect.right() && count > 1) {
        // Not using orientation count go down if we can't fit (if we can then just be happy with what we have)
        count--;
    }
}

void StaticTableLayout::do_layout(const QRect &rect) {
    if (cch_do_layout.size == rect.size() && cch_do_layout.count == items.size())
        // No effective change so don't do layout
        return;
    cch_do_layout.size = rect.size();
    cch_do_layout.count = items.size();

    int row_h;
    QList<QList<int>> row_w;
    int count;

    QRect reff(rect);
    layout_parms(reff, row_h, row_w, count);

    int col = 0;
    int x = reff.x(), y = reff.y();
    for (int i = 0; i < items.size(); i++) {
        for (int ii = 0; ii < items[i].size(); ii++) {
            items[i][ii]->setGeometry(QRect(x, y, row_w[col][ii], row_h));
            x += row_w[col][ii] + shspace;
        }
        x += bhspace - shspace;
        if (++col >= count) {
            col = 0;
            x = reff.x();
            y += row_h + vspace;
        }
    }
    row_height = row_h;
    row_widths = row_w;
}

int StaticTableLayout::layout_height(int width) const {
    QRect reff(0, 0, width, 0);
    int row_h;
    QList<QList<int>> row_w;
    int count;
    layout_parms(reff, row_h, row_w, count);

    return (row_h + vspace) * (items.size() / count);
}

QVector<QLayoutItem*> StaticTableLayout::list2vec(QList<QWidget*> w) {
    QVector<QLayoutItem*> v;
    for (int i = 0; i < w.size(); i++) {
        addChildWidget(w[i]);
        v.append(new QWidgetItem(w[i]));
    }
    return v;
}

StaticTable::StaticTable(QWidget *parent) : QWidget(parent), layout(this) {
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

int StaticTable::count() {
    return layout.count();
}

void StaticTable::addRow(QList<QWidget*> w) {
    layout.addRow(w);
}

void StaticTable::insertRow(QList<QWidget*> w, int i) {
    layout.insertRow(w, i);
}

void StaticTable::removeRow(int i) {
    layout.removeRow(i);
}

void StaticTable::clearRows() {
    layout.clearRows();
}

int StaticTable::columns() {
    return qMax(layout.columns(), 1);
}

int StaticTable::row_size() {
    return layout.real_row_height();
}

void StaticTable::paintEvent(QPaintEvent*) {
    if (layout.columns() <= 0) // Don't paint unless we have at least one column
        return;

    QPainter p(this);
    p.setPen(QPen(QColor(200, 200, 200)));

    QRect rect;
    QVector<int> separators;
    for (int i = 0; i < layout.count(); i++) {
        int row = i / layout.columns();
        int col = i % layout.columns();

        if ((col % 2) == (row % 2))
            p.setBrush(QBrush(QColor(255, 255, 255)));
        else
            p.setBrush(QBrush(QColor(235, 235, 235)));

        layout.itemRect(rect, separators, i);

        int x = rect.left(); // just to store x
        if (col <= 0) // this is left most row
            rect.setLeft(-2);
        if (col >= (layout.columns() - 1))
            rect.setRight(width());
        if (row <= 0)
            rect.setTop(-2);;

        p.drawRect(rect);

        for (int s = 0; s < separators.size(); s++) {
            int sep_x = x + separators[s];
            p.drawLine(sep_x, rect.top(), sep_x, rect.bottom());
        }
    }

}
