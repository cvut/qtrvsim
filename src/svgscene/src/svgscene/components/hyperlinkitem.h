#pragma once

#include "groupitem.h"
namespace svgscene {

/**
 * Represents SVG element <a>.
 *
 * Works exactly as the group item but it adds emits event on doubleclick.
 *
 * @see https://developer.mozilla.org/en-US/docs/Web/SVG/Element/a
 * @see https://www.w3.org/TR/SVG11/linking.html#Links
 */
class HyperlinkItem : public QObject, public GroupItem {
    Q_OBJECT
public:
    explicit HyperlinkItem();
    QString getTargetName() const;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void triggered();
};

} // namespace svgscene
