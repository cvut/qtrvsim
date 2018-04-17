#ifndef CACHEVIEW_H
#define CACHEVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsObject>
#include "graphicsview.h"
#include "qtmipsmachine.h"

class CacheViewBlock : public QGraphicsObject {
    Q_OBJECT
public:
    CacheViewBlock(const machine::Cache *cache, unsigned block);
    ~CacheViewBlock();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private slots:
    void cache_update(unsigned associat, unsigned set, bool valid, bool dirty, std::uint32_t tag, const std::uint32_t *data);

private:
    unsigned block;
    unsigned rows, columns;
    QGraphicsSimpleTextItem **validity, **dirty, **tag, ***data;
};

class CacheViewScene : public QGraphicsScene {
    Q_OBJECT
public:
    CacheViewScene(const machine::Cache *cache);

private:
    unsigned associativity;
    CacheViewBlock **block;
};

#endif // CACHEVIEW_H
