#ifndef BIGSLIDER_H
#define BIGSLIDER_H

#include <QSlider>

// Qt widget subclass exposing 64-bit slider range/value and custom painting,
// designed for selecting a base virtual memory address from 0X00000000 up to 0XFFFFF000.
class BigSlider : public QSlider {
    Q_OBJECT

public:
    explicit BigSlider(QWidget* parent = nullptr);

    qint64 value64() const;
    qint64 alignment64() const;
    void setRange64(qint64 min, qint64 max);
    void setValue64(qint64 v);
    void setAlignment64(qint64 a);

    signals:
        void value64Changed(qint64 newValue);

protected:
    void paintEvent(QPaintEvent* ev) override;
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseMoveEvent(QMouseEvent* ev) override;

private:
    qint64 m_min = 0;
    qint64 m_max = 0xFFFFFFFFULL;
    qint64 m_value = 0;
    qint64 m_align = 0x1000;

    int span() const;
    void handleMousePos(const QPoint& pt);
};

#endif // BIGSLIDER_H
