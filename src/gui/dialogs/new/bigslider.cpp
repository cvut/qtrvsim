#include "bigslider.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionSlider>

BigSlider::BigSlider(QWidget *parent) : QSlider(Qt::Horizontal, parent) {
    QSlider::setRange(0, 1);
    int thickness = style()->pixelMetric(QStyle::PM_SliderThickness, nullptr, this);
    setMinimumHeight(thickness * 2);
}

qint64 BigSlider::value64() const {
    return m_value;
}

void BigSlider::setRange64(qint64 min, qint64 max) {
    m_min = min;
    m_max = max;
    if (m_value < m_min) m_value = m_min;
    if (m_value > m_max) m_value = m_max;
    updateGeometry();
    update();
}

void BigSlider::setValue64(qint64 v) {
    qint64 clamped = qBound(m_min, v, m_max);
    clamped = m_min + ((clamped - m_min) / m_align) * m_align;
    if (clamped == m_value) return;
    m_value = clamped;
    update();
    emit value64Changed(m_value);
}

int BigSlider::span() const {
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    int handleLen = style()->pixelMetric(QStyle::PM_SliderLength, &opt, this);
    return width() - handleLen;
}

void BigSlider::paintEvent(QPaintEvent *ev) {
    int s = span();
    qint64 r = m_max - m_min;
    int posPx = (r > 0) ? int((m_value - m_min) * s / r) : 0;

    QSlider::setRange(0, s);
    QSlider::setValue(posPx);

    QSlider::paintEvent(ev);
}

void BigSlider::mousePressEvent(QMouseEvent *ev) {
    if (ev->button() == Qt::LeftButton) {
        handleMousePos(ev->pos());
        ev->accept();
    } else {
        QSlider::mousePressEvent(ev);
    }
}

void BigSlider::mouseMoveEvent(QMouseEvent *ev) {
    if (ev->buttons() & Qt::LeftButton) {
        handleMousePos(ev->pos());
        ev->accept();
    } else {
        QSlider::mouseMoveEvent(ev);
    }
}

void BigSlider::handleMousePos(const QPoint &pt) {
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    int handleLen = style()->pixelMetric(QStyle::PM_SliderLength, &opt, this);
    int s = span();

    int x = pt.x() - handleLen / 2;
    x = qBound(0, x, s);

    qint64 newVal = m_min + qint64(x) * (m_max - m_min) / qMax(1, s);
    newVal = m_min + ((newVal - m_min) / m_align) * m_align;
    setValue64(newVal);
}

void BigSlider::setAlignment64(qint64 a) {
    m_align = qMax<qint64>(1, a);
    setValue64(m_value);
}

qint64 BigSlider::alignment64() const {
    return m_align;
}