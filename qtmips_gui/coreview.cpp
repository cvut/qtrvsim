#include "coreview.h"

CoreView::CoreView(QWidget *parent, machine::QtMipsMachine *machine) : QGraphicsView(parent) {
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    this->machine = machine;
    // Identification cross
    scene.addLine(400, 0, 400, 800);
    scene.addLine(0, 400, 800, 400);

    pc = new coreview::ProgramCounter(machine);
    pc_multiplexer = new coreview::Multiplexer(4);
    testlatch = new coreview::Latch(machine, 300);

    pc2pc = new coreview::Connection(pc_multiplexer->connector_out(), pc->connector_in());

    scene.addItem(pc);
    scene.addItem(pc_multiplexer);
    scene.addItem(testlatch);
    scene.addItem(pc2pc);

    pc->setPos(100,100);
    pc_multiplexer->setPos(60, 100);
    pc_multiplexer->set(2);

    setScene(&scene);
    // TODO fitInView doesn't work as I want so reimplement or do something with it
    //fitInView(0, 0, 201, 201, Qt::KeepAspectRatioByExpanding);
}

void CoreView::resizeEvent(QResizeEvent *event) {
    // fitInView(0, 0, 201, 201, Qt::KeepAspectRatioByExpanding);
}
