#ifndef LCDDISPLAYDOCK_H
#define LCDDISPLAYDOCK_H

#include "lcddisplayview.h"
#include "machine/machine.h"

#include <QBoxLayout>
#include <QDockWidget>

class LcdDisplayDock : public QDockWidget {
    Q_OBJECT

    using Super = QDockWidget;

public:
    LcdDisplayDock(QWidget *parent, QSettings *settings);
    void resizeEvent(QResizeEvent *event) override;

    void setup(machine::LcdDisplay *lcd_display);

    // public slots:

private:
    void update_layout(int w, int h);

    QT_OWNED QBoxLayout *layout;
    Box<LcdDisplayView> lcd_display_widget;
};

#endif // LCDDISPLAYDOCK_H
