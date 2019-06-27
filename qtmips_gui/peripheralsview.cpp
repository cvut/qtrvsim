#include "peripheralsview.h"
#include "ui_peripheralsview.h"

PeripheralsView::PeripheralsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PeripheralsView)
{
    ui->setupUi(this);

    ui->dialRed->setStyleSheet("QDial { background-color: red }");
    ui->dialGreen->setStyleSheet("QDial { background-color: green }");
    ui->dialBlue->setStyleSheet("QDial { background-color: blue }");

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

    connect(ui->checkRed, SIGNAL(clicked(bool)), perip_spi_led, SLOT(red_knob_push(bool)));
    connect(ui->checkGreen, SIGNAL(clicked(bool)), perip_spi_led, SLOT(green_knob_push(bool)));
    connect(ui->checkBlue, SIGNAL(clicked(bool)), perip_spi_led, SLOT(blue_knob_push(bool)));

    ui->checkRed->setChecked(false);
    ui->checkGreen->setChecked(false);;
    ui->checkBlue->setChecked(false);;

    ui->labelRgb1->setAutoFillBackground(true);
    ui->labelRgb2->setAutoFillBackground(true);

    connect(perip_spi_led, SIGNAL(led_line_changed(uint)), this, SLOT(led_line_changed(uint)));
    connect(perip_spi_led, SIGNAL(led_rgb1_changed(uint)), this, SLOT(led_rgb1_changed(uint)));
    connect(perip_spi_led, SIGNAL(led_rgb2_changed(uint)), this, SLOT(led_rgb2_changed(uint)));

    led_line_changed(0);
    led_rgb1_changed(0);
    led_rgb2_changed(0);
}

void PeripheralsView::led_line_changed(uint val) {
    QString s, t;
    s = QString::number(val, 16);
    t.fill('0', 8 - s.count());
    ui->lineEditHex->setText(t + s);
    s = QString::number(val, 10);
    ui->lineEditDec->setText(s);
    s = QString::number(val, 2);
    t.fill('0', 32 - s.count());
    ui->lineEditBin->setText(t + s);
}

static void set_widget_background_color(QWidget *w, uint val) {
    int r = (val >> 16) & 0xff;
    int g = (val >> 8) & 0xff;
    int b = (val >> 0) & 0xff;
    QPalette::ColorRole brole = w->backgroundRole();
    QPalette pal = w->palette();
    pal.setColor(brole, QColor(r, g, b));
    w->setPalette(pal);
}

void PeripheralsView::led_rgb1_changed(uint val) {
    QString s, t;
    s = QString::number(val, 16);
    t.fill('0', 8 - s.count());
    ui->lineEditRgb1->setText(t + s);

    set_widget_background_color(ui->labelRgb1, val);
}

void PeripheralsView::led_rgb2_changed(uint val) {
    QString s, t;
    s = QString::number(val, 16);
    t.fill('0', 8 - s.count());
    ui->lineEditRgb2->setText(t + s);

    set_widget_background_color(ui->labelRgb2, val);
}
