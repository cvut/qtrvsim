#ifndef PERIPHERALSDOCK_H
#define PERIPHERALSDOCK_H

#include "machine/machine.h"
#include "machine/memory/backend/peripheral.h"
#include "machine/memory/backend/peripspiled.h"
#include "peripheralsview.h"

#include <QDockWidget>
#include <QFormLayout>
#include <QLabel>

class PeripheralsDock : public QDockWidget {
    Q_OBJECT
public:
    PeripheralsDock(QWidget *parent, QSettings *settings);

    void setup(const machine::PeripSpiLed *perip_spi_led);

private:
    QVBoxLayout *layout_box;
    QWidget *top_widget, *top_form {};
    QFormLayout *layout_top_form {};
    PeripheralsView *periph_view;
};

#endif // PERIPHERALSDOCK_H
