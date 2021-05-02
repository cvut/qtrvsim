#ifndef VALUE_H
#define VALUE_H

#include <QChar>
#include <QGraphicsObject>
#include <QPainter>

namespace coreview {

class Value : public QGraphicsObject {
    Q_OBJECT
public:
    Value(
        bool vertical = false,
        unsigned width = 8, // width is for number of character to be shown from
                            // number
        unsigned init_val = 0,
        unsigned base = 16,
        QChar fillchr = '0',
        bool frame = true);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

public slots:
    void value_update(uint32_t);

protected:
    uint32_t val;

private:
    unsigned wid;
    unsigned base;
    bool vertical;
    QChar fillchr;
    bool frame;
};

} // namespace coreview

#endif // VALUE_H
