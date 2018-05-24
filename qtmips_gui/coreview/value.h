#ifndef VALUE_H
#define VALUE_H

#include <QGraphicsObject>
#include <QPainter>

namespace coreview {

class Value : public QGraphicsObject {
    Q_OBJECT
public:
    Value(bool vertical = false, unsigned width = 8, unsigned init_val = 0); // width is for number of character to be shown from number

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public slots:
    void value_update(std::uint32_t);

protected:
    std::uint32_t val;

private:
    unsigned wid;
    bool vertical;
};

}

#endif // VALUE_H
