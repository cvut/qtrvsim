#include "tlbview.h"

#include <QBrush>
#include <QFont>
#include <QPainter>
#include <QString>

static const int ROW_HEIGHT = 16;
static const int VCOL_WIDTH = 18;
static const int FIELD_WIDTH = 120;

TLBAddressBlock::TLBAddressBlock(machine::TLB *tlb, unsigned width) {
    this->width = width;
    rows = tlb->get_config().get_tlb_num_sets();
    s_row = rows > 1 ? (32 - __builtin_clz(rows - 1)) : 0;
    s_tag = 32 - s_row - 2;
    tag = 0;
    row = 0;

    connect(tlb, &machine::TLB::tlb_update, this, &TLBAddressBlock::tlb_update);
}

QRectF TLBAddressBlock::boundingRect() const {
    return QRectF(0, 0, width, 40);
}

void TLBAddressBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    QFont fnt;
    fnt.setPointSize(8);
    painter->setFont(fnt);

    painter->drawText(QRectF(0, 0, width, 14), Qt::AlignCenter, "TLB Address");
    painter->drawText(QRectF(5, 18, 100, 16), Qt::AlignLeft, QString("Set: %1").arg(row));
}

void TLBAddressBlock::tlb_update(unsigned, unsigned set, bool, unsigned, quint64, quint64, bool) {
    this->row = set;
    update();
}

///////////////////////////////////////////

TLBViewBlock::TLBViewBlock(machine::TLB *tlb, unsigned way) : tlb(tlb), way_index(way) {
    rows = tlb->get_config().get_tlb_num_sets();
    curr_row = 0;
    last_set = 0;
    last_highlighted = false;

    QFont font;
    font.setPixelSize(10);

    validity.reserve(rows);
    asid.reserve(rows);
    vpn.reserve(rows);
    phys.reserve(rows);

    int y = 2;
    for (unsigned i = 0; i < rows; ++i) {
        int x = 2;
        auto *v = new QGraphicsSimpleTextItem("0", this);
        v->setFont(font);
        v->setPos(x, y);
        x += VCOL_WIDTH;

        auto *a = new QGraphicsSimpleTextItem("", this);
        a->setFont(font);
        a->setPos(x, y);
        x += 60;

        auto *vpnItem = new QGraphicsSimpleTextItem("", this);
        vpnItem->setFont(font);
        vpnItem->setPos(x, y);
        x += FIELD_WIDTH;

        auto *physItem = new QGraphicsSimpleTextItem("", this);
        physItem->setFont(font);
        physItem->setPos(x, y);

        validity.push_back(v);
        asid.push_back(a);
        vpn.push_back(vpnItem);
        phys.push_back(physItem);

        y += ROW_HEIGHT;
    }

    auto *l_v = new QGraphicsSimpleTextItem("V", this);
    l_v->setFont(font);
    l_v->setPos(2, -14);

    auto *l_asid = new QGraphicsSimpleTextItem("ASID", this);
    l_asid->setFont(font);
    l_asid->setPos(2 + VCOL_WIDTH + 2, -14);

    auto *l_vpn = new QGraphicsSimpleTextItem("VPN", this);
    l_vpn->setFont(font);
    l_vpn->setPos(2 + VCOL_WIDTH + 62, -14);

    auto *l_phys = new QGraphicsSimpleTextItem("PBASE", this);
    l_phys->setFont(font);
    l_phys->setPos(2 + VCOL_WIDTH + 62 + FIELD_WIDTH, -14);

    connect(tlb, &machine::TLB::tlb_update, this, &TLBViewBlock::tlb_update);
}

QRectF TLBViewBlock::boundingRect() const {
    return QRectF(-2, -18, VCOL_WIDTH + 60 + FIELD_WIDTH + 200, rows * ROW_HEIGHT + 40);
}

void TLBViewBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    int width = boundingRect().width();
    painter->drawRect(0, 0, width, rows * ROW_HEIGHT);
    for (unsigned i = 0; i <= rows; ++i) {
        painter->drawLine(0, i * ROW_HEIGHT, width, i * ROW_HEIGHT);
    }
}

void TLBViewBlock::tlb_update(
    unsigned way,
    unsigned set,
    bool valid,
    unsigned asid_v,
    quint64 vpn_v,
    quint64 phys_v,
    bool write) {
    if (way != way_index) return;
    validity[set]->setText(valid ? "1" : "0");
    asid[set]->setText(valid ? QString::number(asid_v) : QString());
    vpn[set]->setText(
        valid ? QString("0x%1").arg(QString::number(vpn_v, 16).toUpper()) : QString());
    phys[set]->setText(
        valid ? QString("0x%1").arg(QString::number(phys_v, 16).toUpper()) : QString());

    if (last_highlighted) {
        validity[last_set]->setBrush(QBrush(QColor(0, 0, 0)));
        asid[last_set]->setBrush(QBrush(QColor(0, 0, 0)));
        vpn[last_set]->setBrush(QBrush(QColor(0, 0, 0)));
        phys[last_set]->setBrush(QBrush(QColor(0, 0, 0)));
    }
    if (valid) {
        QColor c = write ? QColor(200, 0, 0) : QColor(0, 0, 150);
        validity[set]->setBrush(QBrush(c));
        asid[set]->setBrush(QBrush(c));
        vpn[set]->setBrush(QBrush(c));
        phys[set]->setBrush(QBrush(c));
    }
    last_highlighted = true;
    last_set = set;
}

///////////////////////////////////////////

TLBViewScene::TLBViewScene(machine::TLB *tlb) {
    associativity = tlb->get_config().get_tlb_associativity();
    block.reserve(associativity);
    int offset = 0;
    for (unsigned i = 0; i < associativity; ++i) {
        auto b = std::make_unique<TLBViewBlock>(tlb, i);
        addItem(b.get());
        b->setPos(1, offset);
        offset += b->boundingRect().height();
        block.push_back(std::move(b));
    }
    ablock = std::make_unique<TLBAddressBlock>(
        tlb, block.empty() ? FIELD_WIDTH : static_cast<unsigned>(block[0]->boundingRect().width()));
    addItem(ablock.get());
    ablock->setPos(0, -ablock->boundingRect().height() - 16);
}

TLBViewScene::~TLBViewScene() {
    for (auto &bptr : block) {
        if (bptr) removeItem(bptr.get());
    }
    block.clear();
    if (ablock) {
        removeItem(ablock.get());
        ablock.reset();
    }
}
