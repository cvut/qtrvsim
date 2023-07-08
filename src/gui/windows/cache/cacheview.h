#ifndef CACHEVIEW_H
#define CACHEVIEW_H

#include "common/endian.h"
#include "graphicsview.h"
#include "machine/machine.h"

#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QGraphicsView>

class CacheAddressBlock : public QGraphicsObject {
    Q_OBJECT
public:
    CacheAddressBlock(const machine::Cache *cache, unsigned width);

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget) override;

private slots:
    void cache_update(
        unsigned associat,
        unsigned set,
        unsigned col,
        bool valid,
        bool dirty,
        uint32_t tag,
        const uint32_t *data,
        bool write);

private:
    unsigned rows, columns;
    unsigned tag, row, col;
    unsigned s_tag, s_row, s_col;
    unsigned width;
};

class CacheViewBlock : public QGraphicsObject {
    Q_OBJECT
public:
    CacheViewBlock(const machine::Cache *cache, unsigned block, bool last);
    ~CacheViewBlock() override;

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget) override;

private slots:
    virtual void cache_update(
        unsigned associat,
        unsigned set,
        unsigned col,
        bool valid,
        bool dirty,
        uint32_t tag,
        const uint32_t *data,
        bool write);

private:
    const Endian simulated_machine_endian;
    bool islast;
    unsigned block;
    unsigned rows, columns;
    QGraphicsSimpleTextItem **validity, **dirty, **tag, ***data;
    unsigned curr_row;
    bool last_highlighted;
    unsigned last_set;
    unsigned last_col;
};

class CacheViewScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit CacheViewScene(const machine::Cache *cache);
    ~CacheViewScene() override;

private:
    unsigned associativity;
    CacheViewBlock **block;
    CacheAddressBlock *ablock;
};

#endif // CACHEVIEW_H
