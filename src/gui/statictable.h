#ifndef STATICTABLE_H
#define STATICTABLE_H

#include <QLayout>
#include <QList>
#include <QVector>
#include <QWidget>

/*
 * This implements new layout and widget in the same time.
 * The Basic idea is that we need some table view that can also fill in horizontal space. This
 * widget paints simple table underneath the widgets and lays them out in to them. It shows more
 * than one column when there is enough horizontal space.
 */

class StaticTableLayout : public QLayout {
public:
    explicit StaticTableLayout(
        QWidget *parent,
        int margin = 4,
        int horizontal_big_spacing = 4,
        int horizontal_small_spacing = 8,
        int vertical_spacing = 4);
    ~StaticTableLayout() override;

    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int) const override;
    QSize minimumSize() const override;
    void setGeometry(const QRect &rect) override;
    QSize sizeHint() const override;

    void addItem(QLayoutItem *item) override;
    QLayoutItem *itemAt(int index) const override;
    QLayoutItem *takeAt(int index) override;
    int count() const override;                      // This returns number of item blocks

    void addRow(const QList<QWidget *> &);           // This adds a row of widgets
    void insertRow(const QList<QWidget *> &, int i); // Insert row to given position while shifting
                                                     // all others up
    void removeRow(int i);                           // Remove row
    void clearRows();                                // Clear all rows from table

    void itemRect(QRect &rect, QVector<int> &separators, int i); // This returns a single item
                                                                 // rectangle (if expand_margin, and
                                                                 // it's on edge also count in
                                                                 // margin)
    int columns();
    int real_row_height() const;

protected:
    int shspace, bhspace, vspace;
    QVector<QVector<QLayoutItem *>> items;

    int row_height {};
    QList<QList<int>> row_widths;

    int layout_count_approx(const QRect &rect) const;
    int layout_size(int &row_h, QList<QList<int>> &row_w, int count) const;
    void layout_parms(QRect &rect, int &row_h, QList<QList<int>> &row_w, int &count) const;
    void do_layout(const QRect &rect);
    int layout_height(int width) const;

    QVector<QLayoutItem *> list2vec(const QList<QWidget *> &);

    struct {
        QSize size;
        int count {};
    } cch_do_layout;
    mutable struct {
        int w, count;
        int width;
    } cch_heightForWidth {};
    mutable struct {
        int count {};
        QSize size;
    } cch_minSize;
};

class StaticTable : public QWidget {
public:
    explicit StaticTable(QWidget *parent = nullptr);

    int count();
    void addRow(const QList<QWidget *> &);
    void insertRow(const QList<QWidget *> &, int i);
    void removeRow(int i);
    void clearRows();

    int columns();
    int row_size(); // return real row size (height) including spacing

protected:
    void paintEvent(QPaintEvent *) override;

    StaticTableLayout layout;
};

#endif // STATICTABLE_H
