#ifndef PERIPHERALSVIEW_H
#define PERIPHERALSVIEW_H

#include "machine/memory/backend/peripspiled.h"

#include <QWidget>

namespace Ui {
class PeripheralsView;
}

class PeripheralsView : public QWidget {
    Q_OBJECT

public:
    explicit PeripheralsView(QWidget *parent = nullptr);
    ~PeripheralsView() override;

    void setup(const machine::PeripSpiLed *perip_spi_led);

public slots:
    void led_line_changed(uint val);
    void led_rgb1_changed(uint val);
    void led_rgb2_changed(uint val);

private:
    Ui::PeripheralsView *ui {};
};

#endif // PERIPHERALSVIEW_H
