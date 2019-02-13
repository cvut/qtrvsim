#include "peripheralsview.h"
#include "ui_peripheralsview.h"

PeripheralsView::PeripheralsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PeripheralsView)
{
    ui->setupUi(this);

    connect(ui->dialRed, SIGNAL(valueChanged(int)), ui->spinRed, SLOT(setValue(int)));
    connect(ui->dialGreen, SIGNAL(valueChanged(int)), ui->spinGreen, SLOT(setValue(int)));
    connect(ui->dialBlue, SIGNAL(valueChanged(int)), ui->spinBlue, SLOT(setValue(int)));
    connect(ui->spinRed, SIGNAL(valueChanged(int)), ui->dialRed, SLOT(setValue(int)));
    connect(ui->spinGreen, SIGNAL(valueChanged(int)), ui->dialGreen, SLOT(setValue(int)));
    connect(ui->spinBlue, SIGNAL(valueChanged(int)), ui->dialBlue, SLOT(setValue(int)));
}

PeripheralsView::~PeripheralsView()
{
    delete ui;
}

void PeripheralsView::setup(const machine::PeripSpiLed *perip_spi_led) {
    int val;

    connect(ui->spinRed, SIGNAL(valueChanged(int)), perip_spi_led, SLOT(red_knob_update(int)));
    connect(ui->spinGreen, SIGNAL(valueChanged(int)), perip_spi_led, SLOT(green_knob_update(int)));
    connect(ui->spinBlue, SIGNAL(valueChanged(int)), perip_spi_led, SLOT(blue_knob_update(int)));

    val = ui->spinRed->value();
    ui->spinRed->setValue(val - 1);
    ui->spinRed->setValue(val);

    val = ui->spinGreen->value();
    ui->spinGreen->setValue(val - 1);
    ui->spinGreen->setValue(val);

    val = ui->spinBlue->value();
    ui->spinBlue->setValue(val - 1);
    ui->spinBlue->setValue(val);
}
