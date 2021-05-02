#ifndef MULTITEXT_H
#define MULTITEXT_H

#include <QGraphicsObject>
#include <QGraphicsSimpleTextItem>
#include <QString>
#include <vector>

namespace coreview {

class MultiText : public QGraphicsObject {
    Q_OBJECT
public:
    explicit MultiText(const std::vector<QString> &texts_table, bool show_nonzero_red = false);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

public slots:
    void multitext_update(size_t new_value);

private:
    QGraphicsSimpleTextItem text;
    size_t value { 0 };
    const std::vector<QString> &texts_table;
    bool show_nonzero_red;
};

} // namespace coreview

#endif // MULTITEXT_H
