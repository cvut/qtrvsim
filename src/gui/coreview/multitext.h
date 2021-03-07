#ifndef MULTITEXT_H
#define MULTITEXT_H

#include <QGraphicsObject>
#include <QGraphicsSimpleTextItem>
#include <QMap>
#include <QString>

namespace coreview {

class MultiText : public QGraphicsObject {
    Q_OBJECT
public:
    MultiText(QMap<uint32_t, QString> value_map, bool nonzero_red = false);

    QRectF boundingRect() const override;
    void paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget) override;

public slots:
    void multitext_update(uint32_t value);

private:
    QGraphicsSimpleTextItem text;
    uint32_t value {};
    QMap<uint32_t, QString> value_map;
    uint32_t nonzero_red;
};

} // namespace coreview

#endif // MULTITEXT_H
