#ifndef CACHEVIEW_H
#define CACHEVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsObject>
#include "graphicsview.h"
#include "qtmipsmachine.h"


class CacheAddressBlock : public QGraphicsObject {
    Q_OBJECT
public:
    CacheAddressBlock(const machine::Cache *cache, unsigned width);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private slots:
    void cache_update(unsigned associat, unsigned set, bool valid, bool dirty, std::uint32_t tag, const std::uint32_t *data);

private:
    unsigned tag, row, col;
    unsigned s_tag, s_row, s_col;
    unsigned width;
};

class CacheViewBlock : public QGraphicsObject {
    Q_OBJECT
public:
    CacheViewBlock(const machine::Cache *cache, unsigned block, bool last);
    ~CacheViewBlock();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private slots:
    void cache_update(unsigned associat, unsigned set, bool valid, bool dirty, std::uint32_t tag, const std::uint32_t *data);

private:
    bool islast;
    unsigned block;
    unsigned rows, columns;
    QGraphicsSimpleTextItem **validity, **dirty, **tag, ***data;
    unsigned curr_row;
};

class CacheViewScene : public QGraphicsScene {
    Q_OBJECT
public:
    CacheViewScene(const machine::Cache *cache);

private:
    unsigned associativity;
    CacheViewBlock **block;
    CacheAddressBlock *ablock;
};

#endif // CACHEVIEW_H
