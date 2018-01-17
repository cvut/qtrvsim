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

#define NEW_B(TYPE, VAR, ...) do { \
        VAR = new coreview::TYPE(__VA_ARGS__);\
        addItem(VAR);\
    } while(false)
#define NEW(TYPE, VAR, X, Y, ...) do { \
        NEW_B(TYPE, VAR, __VA_ARGS__); \
        VAR->setPos(X, Y); \
    } while(false)
#define NEW_I(VAR, X, Y, SIG) do { \
        NEW(InstructionView, VAR, X, Y); \
        connect(machine->core(), SIGNAL(SIG), VAR, SLOT(instruction_update(const machine::Instruction&))); \
    } while(false)

CoreViewScene::CoreViewScene(CoreView *view, machine::QtMipsMachine *machine) : QGraphicsScene(view) {
    setSceneRect(0, 0, SC_WIDTH, SC_HEIGHT);

    // Identification cross (TODO remove)
    QPen pen;
    pen.setBrush(QBrush(QColor(220, 220, 220)));
    addLine(width()/2, 0, width()/2, height(), pen);
    addLine(0, height()/2, width(), height()/2, pen);
    addRect(0.5, 0.5, width() - 0.5, height() - 0.5, pen);
    // TODO remove

    // Elements //
    NEW(ProgramCounter, pc.pc, 2, 330, machine);
    NEW(Latch, pc.latch, 50, 370, machine, 20);
    NEW(Adder, pc.adder, 100, 350);
    struct coreview::Latch::ConnectorPair pc_latch_pair = pc.latch->new_connector(10);
    NEW_B(Constant, pc.adder_4, pc.adder->connector_in_b(), "4");
    NEW(Junction, pc.junction, 80, pc_latch_pair.out->y());
    NEW(Multiplexer, pc.multiplex, 60, 100, 4);
    NEW(LogicBlock, ctl_block, 300, 100, {"CPU", "Control"});
    ctl_block->setSize(35, 70);
    NEW(Alu, alu, 470, 230);
    NEW(Memory, mem, 20, 510, machine);
    NEW(Registers, regs, 20, 0);
    NEW(Multiplexer, mem_or_reg, 570, 180, 2);
    NEW_I(instr_fetch, 100, 50, instruction_fetched(const machine::Instruction&));

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
    delete ctl_block;
    delete alu;
    delete mem;
    delete regs;
    delete mem_or_reg;
    delete instr_fetch;
}

coreview::Connection *CoreViewScene::new_connection(const coreview::Connector *a, const coreview::Connector *b) {
    coreview::Connection *c = new coreview::Connection(a, b);
    connections.append(c);
    addItem(c);
    return c;
}

CoreViewSceneSimple::CoreViewSceneSimple(CoreView *view, machine::QtMipsMachine *machine) : CoreViewScene(view, machine) {
    NEW(Latch, delay_slot_latch, 160, 50, machine, 150);
}

CoreViewSceneSimple::~CoreViewSceneSimple() {
    delete delay_slot_latch;
}

CoreViewScenePipelined::CoreViewScenePipelined(CoreView *view, machine::QtMipsMachine *machine) : CoreViewScene(view, machine) {
    NEW(Latch, latch_if_id, 158, 90, machine, 350);
    latch_if_id->setTitle("IF/ID");
    NEW(Latch, latch_id_ex, 392, 90, machine, 350);
    latch_id_ex->setTitle("ID/EX");
    NEW(Latch, latch_ex_mem, 536, 90, machine, 350);
    latch_ex_mem->setTitle("EX/MEM");
    NEW(Latch, latch_mem_wb, 680, 90, machine, 350);
    latch_mem_wb->setTitle("MEM/WB");

    NEW_I(inst_dec, 250, 50, instruction_decoded(const machine::Instruction&));
    NEW_I(inst_exec, 400, 50, instruction_executed(const machine::Instruction&));
    NEW_I(inst_mem, 540, 50, instruction_memory(const machine::Instruction&));
    NEW_I(inst_wrb, 670, 50, instruction_writeback(const machine::Instruction&));
}

CoreViewScenePipelined::~CoreViewScenePipelined() {
    delete latch_if_id;
    delete latch_id_ex;
    delete latch_ex_mem;
    delete latch_mem_wb;
    delete inst_dec;
    delete inst_exec;
    delete inst_mem;
    delete inst_wrb;
}
