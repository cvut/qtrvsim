#ifndef TLBVIEW_H
#define TLBVIEW_H

#include "machine/memory/tlb/tlb.h"
#include "svgscene/utils/memory_ownership.h"

#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QPointer>

class TLBAddressBlock : public QGraphicsObject {
    Q_OBJECT
public:
    TLBAddressBlock(machine::TLB *tlb, unsigned width);
    QRectF boundingRect() const override;
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;

public slots:
    void tlb_update(
        unsigned way,
        unsigned set,
        bool valid,
        unsigned asid,
        quint64 vpn,
        quint64 phys,
        bool write);

private:
    unsigned width;
    unsigned rows;
    unsigned tag, row;
    unsigned s_row, s_tag;
};

class TLBViewBlock : public QGraphicsObject {
    Q_OBJECT
public:
    TLBViewBlock(machine::TLB *tlb, unsigned way);
    ~TLBViewBlock() override = default;
    QRectF boundingRect() const override;
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;

public slots:
    void tlb_update(
        unsigned way,
        unsigned set,
        bool valid,
        unsigned asid,
        quint64 vpn,
        quint64 phys,
        bool write);

private:
    QPointer<machine::TLB> tlb;
    unsigned way_index;
    unsigned rows;
    std::vector<BORROWED QGraphicsSimpleTextItem *> validity;
    std::vector<BORROWED QGraphicsSimpleTextItem *> asid;
    std::vector<BORROWED QGraphicsSimpleTextItem *> vpn;
    std::vector<BORROWED QGraphicsSimpleTextItem *> phys;
    unsigned curr_row;
    unsigned last_set;
    bool last_highlighted;
};

class TLBViewScene : public QGraphicsScene {
public:
    TLBViewScene(machine::TLB *tlb);
    ~TLBViewScene() override;

private:
    unsigned associativity;
    std::vector<std::unique_ptr<TLBViewBlock>> block;
    std::unique_ptr<TLBAddressBlock> ablock;
};

#endif // TLBVIEW_H
