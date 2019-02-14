#ifndef PERIPHERALSVIEW_H
#define PERIPHERALSVIEW_H

#include <QWidget>

#include "peripspiled.h"

namespace Ui {
class PeripheralsView;
}

class PeripheralsView : public QWidget
{
    Q_OBJECT

public:
    explicit PeripheralsView(QWidget *parent = 0);
    ~PeripheralsView();

    void setup(const machine::PeripSpiLed *perip_spi_led);

public slots:
    void led_line_changed(uint val);
    void led_rgb1_changed(int val);
    void led_rgb2_changed(int val);

private:
    Ui::PeripheralsView *ui;
};

#endif // PERIPHERALSVIEW_H
