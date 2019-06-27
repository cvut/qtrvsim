#ifndef LCDDISPLAYVIEW_H
#define LCDDISPLAYVIEW_H

#include <QWidget>
#include <QImage>

#include "lcddisplay.h"

class LcdDisplayView : public QWidget
{
    Q_OBJECT

    using Super = QWidget;

public:
    explicit LcdDisplayView(QWidget *parent = 0);
    ~LcdDisplayView();

    void setup(machine::LcdDisplay *lcd_display);

public slots:
    void pixel_update(uint x, uint y, uint r, uint g, uint b);

protected:
    virtual void paintEvent(QPaintEvent *event)  override;

private:
     QImage *fb_pixels;
};

#endif // LCDDISPLAYVIEW_H
