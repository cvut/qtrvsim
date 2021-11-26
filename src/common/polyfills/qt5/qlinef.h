#ifndef POLYFILLS_QLINEF_H
#define POLYFILLS_QLINEF_H

#include <QLine>

QLineF::IntersectType
QLineF_intersect(const QLineF &l1, const QLineF &l2, QPointF *intersectionPoint) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    return l1.intersects(l2, intersectionPoint);
#else
    return l1.intersect(l2, intersectionPoint);
#endif
}

#endif // QTMIPS_QLINEF_H
