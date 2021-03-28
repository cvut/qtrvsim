#pragma once

#include <QGraphicsScene>

namespace svgscene {

/**
 * Graphics scene with extended support for SVG.
 *
 * Current support:
 * - hyperlinks (doubleclick)
 *    Links in svg are parsed as groups and therefore they are always
 *    obscured in qt scene. Therefore we have to travers the while tree up
 *    and find the closest hyperlink element (if one exists).
 */
class SvgGraphicsScene : public QGraphicsScene {
protected:
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
};

} // namespace svgscene
