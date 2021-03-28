#include "simpletextitem.h"

#include <QPainter>

namespace svgscene {

SimpleTextItem::SimpleTextItem(const CssAttributes &css, QGraphicsItem *parent) : Super(parent) {
    const QString anchor = css.value(QStringLiteral("text-anchor"));
    if (anchor == QLatin1String("middle"))
        m_alignment = Qt::AlignHCenter;
    else if (anchor == QLatin1String("end"))
        m_alignment = Qt::AlignRight;
    else
        m_alignment = Qt::AlignLeft;
}

void SimpleTextItem::setText(const QString &text) {
    if (!m_origTransformLoaded) {
        m_origTransformLoaded = true;
        m_origTransform = transform();
    }
    Super::setText(text);
    if (m_alignment != Qt::AlignLeft) {
        qreal w = boundingRect().width();
        QTransform t = m_origTransform;
        if (m_alignment == Qt::AlignHCenter)
            t.translate(-w / 2, 0);
        else if (m_alignment == Qt::AlignRight)
            t.translate(-w, 0);
        setTransform(t);
    }
}

void SimpleTextItem::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget) {
    Super::paint(painter, option, widget);
    // painter->setPen(Qt::green);
    // painter->drawRect(boundingRect());
}

} // namespace svgscene
