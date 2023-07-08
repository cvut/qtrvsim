#include "peripheralsdock.h"

PeripheralsDock::PeripheralsDock(QWidget *parent, QSettings *settings)
    : QDockWidget(parent) {
    (void)settings;
    top_widget = new QWidget(this);
    setWidget(top_widget);
    layout_box = new QVBoxLayout(top_widget);
    periph_view = new PeripheralsView(nullptr);
    layout_box->addWidget(periph_view);

    setObjectName("Peripherals");
    setWindowTitle("Peripherals");
}

void PeripheralsDock::setup(const machine::PeripSpiLed *perip_spi_led) {
    periph_view->setup(perip_spi_led);
}
