#include "coreview.h"
#include <cmath>

//////////////////////////////////////////////////////////////////////////////
/// Size of visible view area
#define SC_WIDTH 720
#define SC_HEIGHT 540
//////////////////////////////////////////////////////////////////////////////

CoreView::CoreView(QWidget *parent) : QGraphicsView(parent) {
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
}

void CoreView::setScene(QGraphicsScene *scene) {
    QGraphicsView::setScene(scene);
    update_scale();
}

void CoreView::resizeEvent(QResizeEvent *event) {
    QGraphicsView::resizeEvent(event);
    update_scale();
}

void CoreView::update_scale() {
    if (scene() == nullptr)
        return; // Skip if we have no scene

    // Note: there is somehow three pixels error when viewing so we have to always compensate
    const int w = scene()->width() + 3;
    const int h = scene()->height() + 3;

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
    setSceneRect(0, 0, SC_WIDTH, SC_HEIGHT);

    // Identification cross (TODO remove)
    QPen pen;
    pen.setBrush(QBrush(QColor(220, 220, 220)));
    addLine(width()/2, 0, width()/2, height(), pen);
    addLine(0, height()/2, width(), height()/2, pen);
    addRect(0.5, 0.5, width() - 0.5, height() - 0.5, pen);
    // TODO remove

#define NEW(TYPE, VAR, ...) do { \
        VAR = new coreview::TYPE(__VA_ARGS__);\
        addItem(VAR);\
    } while(false)
#define NEW_CON(VAR, ...) NEW(Connection, VAR, __VA_ARGS__)

    NEW(ProgramCounter, pc, machine);
    NEW(Latch, pc_latch, machine, 20);
    NEW(Adder, pc_adder);
    NEW(Constant, pc_adder_4, pc_adder->connector_in_b(), "4");
    NEW(Junction, pc_junction);
    NEW(Alu, alu);
    NEW(Memory, mem, machine);
    NEW(Registers, regs);
    NEW(Multiplexer, pc_multiplexer, 4);
    NEW(Multiplexer, mem_or_reg, 2);

    struct coreview::Latch::ConnectorPair pc_latch_pair = pc_latch->new_connector(10);
    NEW_CON(pc2pc_latch, pc->connector_out(), pc_latch_pair.in);
    NEW_CON(pc_latch2pc_joint, pc_latch_pair.out, pc_junction->new_connector(0));
    NEW_CON(pc_joint2pc_adder, pc_junction->new_connector(M_PI_2), pc_adder->connector_in_a());
    NEW_CON(pc_joint2mem, pc_junction->new_connector(-M_PI_2), mem->connector_pc());
    pc_joint2mem->setAxes({CON_AXIS_X(430)});
    NEW_CON(pc_multiplexer2pc, pc_multiplexer->connector_out(), pc->connector_in());
    pc_multiplexer2pc->setAxes({CON_AXIS_Y(90), CON_AXIS_X(80)});

    pc->setPos(2, 330);
    pc_latch->setPos(50, 370);
    pc_adder->setPos(100, 350);
    pc_junction->setPos(80, pc_latch_pair.out->y());
    alu->setPos(470, 230);
    mem->setPos(20, 510);
    regs->setPos(20, 0);
    pc_multiplexer->setPos(60, 100);
    mem_or_reg->setPos(570, 180);

    connect(regs, SIGNAL(open_registers()), this, SIGNAL(request_registers()));
    connect(mem, SIGNAL(open_data_mem()), this, SIGNAL(request_data_memory()));
    connect(mem, SIGNAL(open_program_mem()), this, SIGNAL(request_program_memory()));
}

CoreViewScene::~CoreViewScene() {
    delete pc2pc_latch;
    delete pc_latch2pc_joint;
    delete pc_joint2pc_adder;
    delete pc_joint2mem;
    delete pc_multiplexer2pc;

    delete pc;
    delete pc_latch;
    delete pc_adder;
    delete pc_adder_4;
    delete pc_junction;
    delete alu;
    delete pc_multiplexer;
}

CoreViewSceneSimple::CoreViewSceneSimple(CoreView *view, machine::QtMipsMachine *machine) : CoreViewScene(view, machine) {
    delay_slot_latch = new coreview::Latch(machine, 150);

    addItem(delay_slot_latch);

    delay_slot_latch->setPos(160, 50);
}

CoreViewSceneSimple::~CoreViewSceneSimple() {

}

CoreViewScenePipelined::CoreViewScenePipelined(CoreView *view, machine::QtMipsMachine *machine) : CoreViewScene(view, machine) {
    latch_if_id = new coreview::Latch(machine, 350);
    latch_id_ex = new coreview::Latch(machine, 350);
    latch_ex_mem = new coreview::Latch(machine, 350);
    latch_mem_wb = new coreview::Latch(machine, 350);

    latch_if_id->setTitle("IF/ID");
    latch_id_ex->setTitle("ID/EX");
    latch_ex_mem->setTitle("EX/MEM");
    latch_mem_wb->setTitle("MEM/WB");

    addItem(latch_if_id);
    addItem(latch_id_ex);
    addItem(latch_ex_mem);
    addItem(latch_mem_wb);

    latch_if_id->setPos(158, 90);
    latch_id_ex->setPos(392, 90);
    latch_ex_mem->setPos(536, 90);
    latch_mem_wb->setPos(680, 90);
}

CoreViewScenePipelined::~CoreViewScenePipelined() {

}
