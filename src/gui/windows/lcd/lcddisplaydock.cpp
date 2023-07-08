#include "lcddisplaydock.h"

#include "lcddisplayview.h"

#include <QString>
#include <QTextBlock>

LcdDisplayDock::LcdDisplayDock(QWidget *parent, QSettings *settings)
    : Super(parent) {
    (void)settings;
    lcd_display_widget.reset(new LcdDisplayView(this));
    auto *fill_widget = new QWidget(this);

    layout = new QBoxLayout(QBoxLayout::LeftToRight, fill_widget);
    // add spacer, then your widget, then spacer
    layout->addItem(new QSpacerItem(0, 0));
    layout->addWidget(lcd_display_widget.data());
    layout->addItem(new QSpacerItem(0, 0));
    fill_widget->setLayout(layout);
    setWidget(fill_widget);

    setObjectName("LCD Display");
    setWindowTitle("LCD Display");
}

void LcdDisplayDock::setup(machine::LcdDisplay *lcd_display) {
    lcd_display_widget->setup(lcd_display);
    update_layout(width(), height());
}

void LcdDisplayDock::update_layout(int w, int h) {
    // Keeping the aspect ratio based on
    // https://stackoverflow.com/questions/30005540/keeping-the-aspect-ratio-of-a-sub-classed-qwidget-during-resize

    float thisAspectRatio = (float)w / h;
    int widgetStretch, outerStretch;
    float arWidth = lcd_display_widget->fb_width();   // aspect ratio width
    float arHeight = lcd_display_widget->fb_height(); // aspect ratio height

    if ((arWidth == 0) || (arHeight == 0)) {
        outerStretch = 0;
        widgetStretch = 1;
    } else if (thisAspectRatio > (arWidth / arHeight)) { // too wide
        layout->setDirection(QBoxLayout::LeftToRight);
        widgetStretch = height() * (arWidth / arHeight); // i.e., my width
        outerStretch = (width() - widgetStretch) / 2 + 0.5;
    } else { // too tall
        layout->setDirection(QBoxLayout::TopToBottom);
        widgetStretch = width() * (arHeight / arWidth); // i.e., my height
        outerStretch = (height() - widgetStretch) / 2 + 0.5;
    }

    layout->setStretch(0, outerStretch);
    layout->setStretch(1, widgetStretch);
    layout->setStretch(2, outerStretch);
}

void LcdDisplayDock::resizeEvent(QResizeEvent *event) {
    // Keeping the aspect ratio based on
    // https://stackoverflow.com/questions/30005540/keeping-the-aspect-ratio-of-a-sub-classed-qwidget-during-resize

    update_layout(event->size().width(), event->size().height());

    Super::resizeEvent(event);
}
