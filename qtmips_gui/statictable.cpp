#include "statictable.h"
#include <QPainter>
#include <qtmipsexception.h>

#include <iostream>
using namespace std;

StaticTableLayout::StaticTableLayout(QWidget *parent, int margin, int horizontal_big_spacing, int horizontal_small_spacing, int vertical_spacing) : QLayout(parent) {
    setContentsMargins(margin, margin, margin, margin);
    bhspace = horizontal_big_spacing;
    shspace = horizontal_small_spacing;
    vspace = vertical_spacing;
    setSizeConstraint(QLayout::SetMinAndMaxSize);
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
    // TODO cache value
    return layout_height(w);
}

QSize StaticTableLayout::minimumSize() const {
    QSize s;
    for (int i = 0; i < items.size(); i++) {
        QSize ss;
        for (int y = 0; y < items[i].size(); y++) {
            ss = s.expandedTo(items[i][y]->minimumSize() + QSize(shspace, 0));
        }
        s = s.expandedTo(ss - QSize(shspace, 0));
    }
    s += QSize(2*margin(), 2*margin());
    return s;
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
    QVector<QLayoutItem*> v;
    for (int i = 0; i < w.size(); i++) {
        addChildWidget(w[i]);
        v.append(new QWidgetItem(w[i]));
    }

    items.append(v);
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

int StaticTableLayout::layout_count_approx(const QRect &rect) const {
    if (items.size() <= 0)
        return 1;
    int w = 0;
    for (int i = 0; i < items[0].size(); i++)
        w += items[0][i]->sizeHint().width() + shspace;
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
    if (layout_size(row_h, row_w, count) > rect.right()) {
        // Not using orientation count go down if we can't fit (if we can then just be happy with what we have)
        do {
            if (count <= 1)
                // TODO meaby warning?
                break; // We can't fit it but ignore that.
            count--;
        } while (layout_size(row_h, row_w, count) <= rect.width());
    }
}

void StaticTableLayout::do_layout(const QRect &rect) {
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

StaticTable::StaticTable(QWidget *parent) : QWidget(parent), layout(this) {
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

void StaticTable::addRow(QList<QWidget*> w) {
    layout.addRow(w);
}

void StaticTable::paintEvent(QPaintEvent*) {
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
            p.setBrush(QBrush(QColor(230, 230, 230)));

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
