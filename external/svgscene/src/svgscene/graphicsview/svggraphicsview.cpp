#include "svggraphicsview.h"

#include "polyfills/qt5/qwheelevent.h"
#include "utils/logging.h"

#include <QWheelEvent>

LOG_CATEGORY("svgscene.view");

SvgGraphicsView::SvgGraphicsView(QWidget *parent) : Super(parent) {}

void SvgGraphicsView::zoomToFit() {
    if (scene()) {
        QRectF sr = scene()->sceneRect();
        fitInView(sr, Qt::KeepAspectRatio);
    }
}

void SvgGraphicsView::zoom(double delta, const QPointF &mouse_pos) {
    LOG() << "delta:" << delta << "center_pos:" << mouse_pos.x() << mouse_pos.y();
    double factor = delta / 100;
    factor = 1 + factor;
    if (factor < 0) factor = 0.1;
    scale(factor, factor);

    QRect view_rect = QRect(viewport()->pos(), viewport()->size());
    QPoint view_center = view_rect.center();
    QSize view_d(view_center.x() - mouse_pos.x(), view_center.y() - mouse_pos.y());
    view_d /= factor;
    view_center = QPoint(mouse_pos.x() + view_d.width(), mouse_pos.y() + view_d.height());
    QPointF new_scene_center = mapToScene(view_center);
    centerOn(new_scene_center);
}

void SvgGraphicsView::paintEvent(QPaintEvent *event) {
    Super::paintEvent(event);
}

void SvgGraphicsView::wheelEvent(QWheelEvent *ev) {
    if (ev->angleDelta().y() != 0) { // vertical orientation
        if (ev->modifiers() == Qt::ControlModifier) {
            double delta = ev->angleDelta().y();
            zoom(delta / 10, QWheelEvent_poly(ev).position());
            ev->accept();
            return;
        }
    }
    Super::wheelEvent(ev);
}

void SvgGraphicsView::mousePressEvent(QMouseEvent *ev) {
    if (ev->button() == Qt::LeftButton && ev->modifiers() == Qt::ControlModifier) {
        m_dragMouseStartPos = ev->pos();
        setCursor(QCursor(Qt::ClosedHandCursor));
        ev->accept();
        return;
    }
    Super::mousePressEvent(ev);
}

void SvgGraphicsView::mouseReleaseEvent(QMouseEvent *ev) {
    if (ev->button() == Qt::LeftButton && ev->modifiers() == Qt::ControlModifier) {
        setCursor(QCursor());
    }
    Super::mouseReleaseEvent(ev);
}

void SvgGraphicsView::mouseMoveEvent(QMouseEvent *ev) {
    if (ev->buttons() == Qt::LeftButton && ev->modifiers() == Qt::ControlModifier) {
        QPoint pos = ev->pos();
        QRect view_rect = QRect(viewport()->pos(), viewport()->size());
        QPoint view_center = view_rect.center();
        QPoint d(pos.x() - m_dragMouseStartPos.x(), pos.y() - m_dragMouseStartPos.y());
        view_center -= d;
        QPointF new_scene_center = mapToScene(view_center);
        centerOn(new_scene_center);
        m_dragMouseStartPos = pos;
        ev->accept();
        return;
    }
    Super::mouseMoveEvent(ev);
}
