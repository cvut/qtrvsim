#include "coreview.h"

CoreView::CoreView(QWidget *parent) : QGraphicsView(parent) {
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    setSceneRect(0, 0, scene_width, scene_height);
    update_scale();
}

void CoreView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    update_scale();
}

void CoreView::update_scale() {
    // Note: there is somehow three pixels error when viewing so we have to always compensate
    const int w = scene_width + 3;
    const int h = scene_height + 3;

    qreal scale = 1;
    if (height() > h && width() > w) {
        if (height() > width()) {
            scale = (qreal)width() / w;
        } else {
            scale = (qreal)height() / h;
        }
    }
    QTransform t;
    t.scale(scale, scale);
    setTransform(t, false);
}

CoreViewScene::CoreViewScene(CoreView *view, machine::QtMipsMachine *machine) : QGraphicsScene(view) {
    this->machine = machine;
    // Identification cross
    addLine(400, 0, 400, 800);
    addLine(0, 400, 800, 400);

    pc = new coreview::ProgramCounter(machine);
    alu = new coreview::Alu();
    pc_multiplexer = new coreview::Multiplexer(4);
    testlatch = new coreview::Latch(machine, 300);

    pc2pc = new coreview::Connection(pc_multiplexer->connector_out(), pc->connector_in());

    addItem(pc);
    addItem(alu);
    addItem(pc_multiplexer);
    addItem(testlatch);
    addItem(pc2pc);

    pc->setPos(100,100);
    alu->setPos(200, 100);
    pc_multiplexer->setPos(60, 100);
    pc_multiplexer->set(2);
}
