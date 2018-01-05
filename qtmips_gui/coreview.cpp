#include "coreview.h"

CoreView::CoreView(QWidget *parent) : QGraphicsView(parent) {
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    // TODO fitInView doesn't work as I want so reimplement or do something with it
    //fitInView(0, 0, 201, 201, Qt::KeepAspectRatioByExpanding);
}

void CoreView::resizeEvent(QResizeEvent *event __attribute__((unused))) {
    // fitInView(0, 0, 201, 201, Qt::KeepAspectRatioByExpanding);
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

    QGraphicsScene *old_scene = view->scene();
    view->setScene(this);
    if (old_scene != nullptr)
        delete old_scene;
}
