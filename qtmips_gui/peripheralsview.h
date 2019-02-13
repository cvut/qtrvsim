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

private:
    Ui::PeripheralsView *ui;
};

#endif // PERIPHERALSVIEW_H
