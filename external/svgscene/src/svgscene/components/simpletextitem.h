#pragma once

#include "svgscene/svghandler.h"

#include <QGraphicsItem>

namespace svgscene {

class SimpleTextItem : public QGraphicsSimpleTextItem {
    using Super = QGraphicsSimpleTextItem;

public:
    explicit SimpleTextItem(const CssAttributes &css, QGraphicsItem *parent = nullptr);

    void setText(const QString &text);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    int m_alignment = Qt::AlignLeft;
    QTransform m_origTransform;
    bool m_origTransformLoaded = false;
};

} // namespace svgscene