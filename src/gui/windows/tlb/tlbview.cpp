#include "tlbview.h"

#include <QBrush>
#include <QFont>
#include <QPainter>
#include <QString>

static const int ROW_HEIGHT = 16;
static const int VCOL_WIDTH = 18;
static const int FLAG_WIDTH = 14;
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

void TLBAddressBlock::tlb_update(
    unsigned,
    unsigned set,
    bool,
    unsigned,
    quint64,
    quint64,
    bool,
    bool,
    bool,
    bool,
    bool,
    bool,
    bool) {
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

    r_bit.reserve(rows);
    w_bit.reserve(rows);
    x_bit.reserve(rows);
    u_bit.reserve(rows);
    g_bit.reserve(rows);
    a_bit.reserve(rows);
    d_bit.reserve(rows);

    int y = 2;
    for (unsigned i = 0; i < rows; ++i) {
        int x = 2;
        auto *v = new QGraphicsSimpleTextItem("0", this);
        v->setFont(font);
        v->setPos(x, y);
        x += VCOL_WIDTH;

        auto *rItem = new QGraphicsSimpleTextItem("", this);
        rItem->setFont(font);
        rItem->setPos(x, y);
        x += FLAG_WIDTH;

        auto *wItem = new QGraphicsSimpleTextItem("", this);
        wItem->setFont(font);
        wItem->setPos(x, y);
        x += FLAG_WIDTH;

        auto *xItem = new QGraphicsSimpleTextItem("", this);
        xItem->setFont(font);
        xItem->setPos(x, y);
        x += FLAG_WIDTH;

        auto *uItem = new QGraphicsSimpleTextItem("", this);
        uItem->setFont(font);
        uItem->setPos(x, y);
        x += FLAG_WIDTH;

        auto *gItem = new QGraphicsSimpleTextItem("", this);
        gItem->setFont(font);
        gItem->setPos(x, y);
        x += FLAG_WIDTH + 4;

        auto *aItem = new QGraphicsSimpleTextItem("", this);
        aItem->setFont(font);
        aItem->setPos(x, y);
        x += FLAG_WIDTH;

        auto *dItem = new QGraphicsSimpleTextItem("", this);
        dItem->setFont(font);
        dItem->setPos(x, y);
        x += FLAG_WIDTH + 4;

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
        r_bit.push_back(rItem);
        w_bit.push_back(wItem);
        x_bit.push_back(xItem);
        u_bit.push_back(uItem);
        g_bit.push_back(gItem);
        a_bit.push_back(aItem);
        d_bit.push_back(dItem);
        asid.push_back(a);
        vpn.push_back(vpnItem);
        phys.push_back(physItem);

        y += ROW_HEIGHT;
    }

    auto *l_v = new QGraphicsSimpleTextItem("V", this);
    l_v->setFont(font);
    l_v->setPos(2, -14);

    auto *l_r = new QGraphicsSimpleTextItem("R", this);
    l_r->setFont(font);
    l_r->setPos(2 + VCOL_WIDTH, -14);

    auto *l_w = new QGraphicsSimpleTextItem("W", this);
    l_w->setFont(font);
    l_w->setPos(2 + VCOL_WIDTH + FLAG_WIDTH, -14);

    auto *l_x = new QGraphicsSimpleTextItem("X", this);
    l_x->setFont(font);
    l_x->setPos(2 + VCOL_WIDTH + FLAG_WIDTH * 2, -14);

    auto *l_u = new QGraphicsSimpleTextItem("U", this);
    l_u->setFont(font);
    l_u->setPos(2 + VCOL_WIDTH + FLAG_WIDTH * 3, -14);

    auto *l_g = new QGraphicsSimpleTextItem("G", this);
    l_g->setFont(font);
    l_g->setPos(2 + VCOL_WIDTH + FLAG_WIDTH * 4, -14);

    auto *l_a = new QGraphicsSimpleTextItem("A", this);
    l_a->setFont(font);
    l_a->setPos(2 + VCOL_WIDTH + FLAG_WIDTH * 5, -14);

    auto *l_d = new QGraphicsSimpleTextItem("D", this);
    l_d->setFont(font);
    l_d->setPos(2 + VCOL_WIDTH + FLAG_WIDTH * 6, -14);

    auto *l_asid = new QGraphicsSimpleTextItem("ASID", this);
    l_asid->setFont(font);
    l_asid->setPos(2 + VCOL_WIDTH + FLAG_WIDTH * 7 + 4, -14);

    auto *l_vpn = new QGraphicsSimpleTextItem("VPN", this);
    l_vpn->setFont(font);
    l_vpn->setPos(2 + VCOL_WIDTH + FLAG_WIDTH * 7 + 4 + 60, -14);

    auto *l_phys = new QGraphicsSimpleTextItem("PBASE", this);
    l_phys->setFont(font);
    l_phys->setPos(2 + VCOL_WIDTH + FLAG_WIDTH * 7 + 4 + 60 + FIELD_WIDTH, -14);

    connect(tlb, &machine::TLB::tlb_update, this, &TLBViewBlock::tlb_update);
}

QRectF TLBViewBlock::boundingRect() const {
    int width = VCOL_WIDTH + FLAG_WIDTH * 5 + 4 + 60 + FIELD_WIDTH + 200;
    return QRectF(-2, -18, width, rows * ROW_HEIGHT + 40);
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
    bool R,
    bool W,
    bool X,
    bool U,
    bool G,
    bool A,
    bool D) {
    if (way != way_index) return;
    validity[set]->setText(valid ? "1" : "0");
    asid[set]->setText(valid ? QString::number(asid_v) : QString());
    vpn[set]->setText(
        valid ? QString("0x%1").arg(QString::number(vpn_v, 16).toUpper()) : QString());
    phys[set]->setText(
        valid ? QString("0x%1").arg(QString::number(phys_v, 16).toUpper()) : QString());
    r_bit[set]->setText(valid ? (R ? "1" : "0") : QString());
    w_bit[set]->setText(valid ? (W ? "1" : "0") : QString());
    x_bit[set]->setText(valid ? (X ? "1" : "0") : QString());
    u_bit[set]->setText(valid ? (U ? "1" : "0") : QString());
    g_bit[set]->setText(valid ? (G ? "1" : "0") : QString());
    a_bit[set]->setText(valid ? (A ? "1" : "0") : QString());
    d_bit[set]->setText(valid ? (D ? "1" : "0") : QString());

    if (last_highlighted) {
        validity[last_set]->setBrush(QBrush(QColor(0, 0, 0)));
        r_bit[last_set]->setBrush(QBrush(QColor(0, 0, 0)));
        w_bit[last_set]->setBrush(QBrush(QColor(0, 0, 0)));
        x_bit[last_set]->setBrush(QBrush(QColor(0, 0, 0)));
        u_bit[last_set]->setBrush(QBrush(QColor(0, 0, 0)));
        g_bit[last_set]->setBrush(QBrush(QColor(0, 0, 0)));
        a_bit[last_set]->setBrush(QBrush(QColor(0, 0, 0)));
        d_bit[last_set]->setBrush(QBrush(QColor(0, 0, 0)));
        asid[last_set]->setBrush(QBrush(QColor(0, 0, 0)));
        vpn[last_set]->setBrush(QBrush(QColor(0, 0, 0)));
        phys[last_set]->setBrush(QBrush(QColor(0, 0, 0)));
    }
    if (valid) {
        QColor c = QColor(0, 0, 150);
        validity[set]->setBrush(QBrush(c));
        r_bit[set]->setBrush(QBrush(c));
        w_bit[set]->setBrush(QBrush(c));
        x_bit[set]->setBrush(QBrush(c));
        u_bit[set]->setBrush(QBrush(c));
        g_bit[set]->setBrush(QBrush(c));
        a_bit[set]->setBrush(QBrush(c));
        d_bit[set]->setBrush(QBrush(c));
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
