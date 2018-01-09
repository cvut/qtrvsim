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

#define NEW_B(TYPE, VAR, ...) do { \
        VAR = new coreview::TYPE(__VA_ARGS__);\
        addItem(VAR);\
    } while(false)
#define NEW(TYPE, VAR, X, Y, ...) do { \
        NEW_B(TYPE, VAR, __VA_ARGS__); \
        VAR->setPos(X, Y); \
    } while(false)

    // Elements //
    NEW(ProgramCounter, pc.pc, 2, 330, machine);
    NEW(Latch, pc.latch, 50, 370, machine, 20);
    NEW(Adder, pc.adder, 100, 350);
    struct coreview::Latch::ConnectorPair pc_latch_pair = pc.latch->new_connector(10);
    NEW_B(Constant, pc.adder_4, pc.adder->connector_in_b(), "4");
    NEW(Junction, pc.junction, 80, pc_latch_pair.out->y());
    NEW(Multiplexer, pc.multiplex, 60, 100, 4);
    NEW(Alu, alu, 470, 230);
    NEW(Memory, mem, 20, 510, machine);
    NEW(Registers, regs, 20, 0);
    NEW(Multiplexer, mem_or_reg, 570, 180, 2);

    // Connections //
    coreview::Connection *con;
    new_connection(pc.pc->connector_out(), pc_latch_pair.in);
    new_connection(pc_latch_pair.out, pc.junction->new_connector(0));
    new_connection(pc.junction->new_connector(M_PI_2), pc.adder->connector_in_a());
    con = new_connection(pc.junction->new_connector(-M_PI_2), mem->connector_pc());
    con->setAxes({CON_AXIS_X(430)});
    con = new_connection(pc.multiplex->connector_out(), pc.pc->connector_in());
    con->setAxes({CON_AXIS_Y(90), CON_AXIS_X(80)});
    con = new_connection(pc.adder->connector_out(), pc.multiplex->connector_in(3));
    con->setAxes({CON_AXIS_Y(130), CON_AXIS_X(280), CON_AXIS_Y(55)});

    connect(regs, SIGNAL(open_registers()), this, SIGNAL(request_registers()));
    connect(mem, SIGNAL(open_data_mem()), this, SIGNAL(request_data_memory()));
    connect(mem, SIGNAL(open_program_mem()), this, SIGNAL(request_program_memory()));
}

CoreViewScene::~CoreViewScene() {
    for (int i = 0; i < connections.size(); i++)
        delete connections[i];

    delete pc.pc;
    delete pc.latch;
    delete pc.adder;
    delete pc.adder_4;
    delete pc.junction;
    delete pc.multiplex;
    delete alu;
    delete mem;
    delete regs;
    delete mem_or_reg;
}

coreview::Connection *CoreViewScene::new_connection(const coreview::Connector *a, const coreview::Connector *b) {
    coreview::Connection *c = new coreview::Connection(a, b);
    connections.append(c);
    addItem(c);
    return c;
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
