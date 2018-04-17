#include "cacheview.h"

//////////////////////
#define ROW_HEIGHT 14
#define VD_WIDTH 10
#define DATA_WIDTH 72
#define PENW 1
//////////////////////

CacheViewBlock::CacheViewBlock(const machine::Cache *cache, unsigned block) : QGraphicsObject(nullptr) {
    this->block = block;
    rows = cache->config().sets();
    columns = cache->config().blocks();

    QFont font;
    font.setPointSize(7);

    validity = new QGraphicsSimpleTextItem*[rows];
    if (cache->config().write_policy() == machine::MachineConfigCache::WP_BACK)
        dirty = new QGraphicsSimpleTextItem*[rows];
    else
        dirty = nullptr;
    tag = new QGraphicsSimpleTextItem*[rows];
    data = new QGraphicsSimpleTextItem**[rows];
    int row_y = 1;
    for (unsigned i = 0; i < rows; i++) {
        int row_x = 2;
        validity[i] = new QGraphicsSimpleTextItem("0", this);
        validity[i]->setPos(row_x, row_y);
        validity[i]->setFont(font);
        row_x += VD_WIDTH;
        if (dirty) {
            dirty[i] = new QGraphicsSimpleTextItem(this);
            dirty[i]->setPos(row_x, row_y);
            dirty[i]->setFont(font);
            row_x += VD_WIDTH;
        }
        tag[i] = new QGraphicsSimpleTextItem(this);
        tag[i]->setPos(row_x, row_y);
        tag[i]->setFont(font);
        row_x += DATA_WIDTH;

        data[i] = new QGraphicsSimpleTextItem*[columns];
        for (unsigned y = 0; y < columns; y++) {
            data[i][y] = new QGraphicsSimpleTextItem(this);
            data[i][y]->setPos(row_x, row_y);
            data[i][y]->setFont(font);
            row_x += DATA_WIDTH;
        }

        row_y += ROW_HEIGHT;
    }

    connect(cache, SIGNAL(cache_update(uint,uint,bool,bool,std::uint32_t,const std::uint32_t*)), this, SLOT(cache_update(uint,uint,bool,bool,std::uint32_t,const std::uint32_t*)));
}

CacheViewBlock::~CacheViewBlock() {
    delete validity;
    delete dirty;
    delete tag;
    for (unsigned y = 0; y < rows; y++)
        delete data[y];
    delete data;
}

QRectF CacheViewBlock::boundingRect() const  {
    return QRectF(
        -PENW / 2,
        -PENW / 2,
        VD_WIDTH + (dirty ? VD_WIDTH : 0) + DATA_WIDTH*(columns+1) + PENW,
        ROW_HEIGHT*rows + PENW
    );
}

void CacheViewBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option __attribute__((unused)), QWidget *widget __attribute__((unused)))  {
    // Draw horizontal lines
    for (unsigned i = 0; i <= rows; i++)
        painter->drawLine(0, i * ROW_HEIGHT, VD_WIDTH + (dirty ? VD_WIDTH : 0) + DATA_WIDTH*(columns + 1), i * ROW_HEIGHT);
    // Draw vertical lines
    painter->drawLine(0, 0, 0, rows*ROW_HEIGHT);
    int c_width = VD_WIDTH;
    painter->drawLine(c_width, 0, c_width, rows*ROW_HEIGHT);
    if (dirty) {
        c_width += VD_WIDTH;
        painter->drawLine(c_width, 0, c_width, rows*ROW_HEIGHT);
    }
    c_width += DATA_WIDTH;
    painter->drawLine(c_width, 0, c_width, rows*ROW_HEIGHT);
    for (unsigned i = 0; i <= columns; i++) {
        c_width += DATA_WIDTH;
        painter->drawLine(c_width, 0, c_width, rows*ROW_HEIGHT);
    }
}

void CacheViewBlock::cache_update(unsigned associat, unsigned set, bool valid, bool dirty, std::uint32_t tag, const std::uint32_t *data) {
    if (associat != block)
        return; // Ignore blocks that are not us
    validity[set]->setText(valid ? "1" : "0");
    if (this->dirty)
        this->dirty[set]->setText(valid ? (dirty ? "1" : "0") : "");
    // TODO calculate correct size of tag
    this->tag[set]->setText(valid ? QString("0x") + QString("%1").arg(tag, 8, 16, QChar('0')).toUpper() : "");
    for (unsigned i = 0; i < columns; i++)
        this->data[set][i]->setText(valid ? QString("0x") + QString("%1").arg(data[i], 8, 16, QChar('0')).toUpper() : "");
}


CacheViewScene::CacheViewScene(const machine::Cache *cache) {
    associativity = cache->config().associativity();
    block = new CacheViewBlock*[associativity];
    int offset = 0;
    for (unsigned i = 0; i < associativity; i++) {
        block[i] = new CacheViewBlock(cache, i);
        addItem(block[i]);
        block[i]->setPos(1, offset);
        offset += block[i]->boundingRect().height() + 3;
    }
}
