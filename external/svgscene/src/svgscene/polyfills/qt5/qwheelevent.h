#pragma once

#include <QWheelEvent>

class QWheelEvent_poly final : public QWheelEvent {
public:
    explicit QWheelEvent_poly(QWheelEvent *event) : QWheelEvent(*event) {}

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QPointF position() const {
        return this->pos();
    }
#endif
};