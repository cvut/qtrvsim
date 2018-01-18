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
    // Primary points
    NEW(ProgramMemory, mem_program, 90, 240, machine);
    NEW(DataMemory, mem_data, 600, 300, machine);
    NEW(Registers, regs, 210, 240);
    // Fetch stage
    NEW(ProgramCounter, ft.pc, 2, 280, machine);
    NEW(Latch, ft.latch, 55, 250, machine, 20);
    NEW(Adder, ft.adder, 100, 330);
    struct coreview::Latch::ConnectorPair pc_latch_pair = ft.latch->new_connector(10);
    NEW_B(Constant, ft.adder_4, ft.adder->connector_in_b(), "4");
    NEW(Junction, ft.junction, 80, mem_program->connector_address()->y());
    NEW(Multiplexer, ft.multiplex, 20, 390, 4);
    // Decode stage
    NEW(LogicBlock, dc.ctl_block, 230, 90, {"Control", "unit"});
    dc.ctl_block->setSize(35, 70);
    NEW(LogicBlock, dc.sign_ext, 230, 360, {"Sign", "extension"});
    NEW(LogicBlock, dc.shift2, 290, 390, "<<2");
    NEW(Adder, dc.add, 320, 390);
    const coreview::Connector *dc_con_sign_ext = dc.sign_ext->new_connector(1, 0);
    NEW(Junction, dc.j_sign_ext, 270, dc_con_sign_ext->y());
    // Execute stage
    NEW(Alu, alu, 470, 230);
    // Memory stage
    NEW(Multiplexer, mem_or_reg, 570, 180, 2);

    // Connections //
    coreview::Connection *con;
    // Fetch stage
    new_connection(ft.pc->connector_out(), pc_latch_pair.in);
    new_connection(pc_latch_pair.out, ft.junction->new_connector(0));
    new_connection(ft.junction->new_connector(M_PI_2), ft.adder->connector_in_a());
    new_connection(ft.junction->new_connector(M_PI), mem_program->connector_address());
    new_connection(ft.multiplex->connector_out(), ft.pc->connector_in());
    con = new_connection(ft.adder->connector_out(), ft.multiplex->connector_in(0));
    con->setAxes({CON_AXIS_Y(130), CON_AXIS_X(380), CON_AXIS_Y(10)});
    // Decode stage
    new_connection(dc_con_sign_ext, dc.j_sign_ext->new_connector(0));
    new_connection(dc.j_sign_ext->new_connector(-M_PI_2), dc.shift2->new_connector(-1, 0));
    new_connection(dc.shift2->new_connector(1, 0), dc.add->connector_in_a());

    connect(regs, SIGNAL(open_registers()), this, SIGNAL(request_registers()));
    connect(mem_program, SIGNAL(open_mem()), this, SIGNAL(request_program_memory()));
    connect(mem_data, SIGNAL(open_mem()), this, SIGNAL(request_data_memory()));
}

CoreViewScene::~CoreViewScene() {
    for (int i = 0; i < connections.size(); i++)
        delete connections[i];

    delete ft.pc;
    delete ft.latch;
    delete ft.adder;
    delete ft.adder_4;
    delete ft.junction;
    delete ft.multiplex;
    delete dc.ctl_block;
    delete alu;
    delete mem_program;
    delete mem_data;
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
    NEW(Latch, delay_slot_latch, 160, 50, machine, 150);
}

CoreViewSceneSimple::~CoreViewSceneSimple() {
    delete delay_slot_latch;
}

CoreViewScenePipelined::CoreViewScenePipelined(CoreView *view, machine::QtMipsMachine *machine) : CoreViewScene(view, machine) {
    NEW(Latch, latch_if_id, 158, 90, machine, 400);
    latch_if_id->setTitle("IF/ID");
    NEW(Latch, latch_id_ex, 392, 90, machine, 400);
    latch_id_ex->setTitle("ID/EX");
    NEW(Latch, latch_ex_mem, 536, 90, machine, 400);
    latch_ex_mem->setTitle("EX/MEM");
    NEW(Latch, latch_mem_wb, 680, 90, machine, 400);
    latch_mem_wb->setTitle("MEM/WB");

    NEW_I(inst_fetch, 100, 2, instruction_fetched(const machine::Instruction&));
    NEW_I(inst_dec, 250, 2, instruction_decoded(const machine::Instruction&));
    NEW_I(inst_exec, 400, 2, instruction_executed(const machine::Instruction&));
    NEW_I(inst_mem, 540, 2, instruction_memory(const machine::Instruction&));
    NEW_I(inst_wrb, 670, 2, instruction_writeback(const machine::Instruction&));

    if (machine->config().hazard_unit() != machine::MachineConfig::HU_NONE) {
        NEW(LogicBlock, hazard_unit, SC_WIDTH/2, SC_HEIGHT - 15, "Hazard Unit");
        hazard_unit->setSize(SC_WIDTH - 100, 12);
    }

    struct coreview::Latch::ConnectorPair program_latch_pair = latch_if_id->new_connector(mem_program->connector_instruction()->y() - latch_if_id->y());
    new_connection(mem_program->connector_instruction(), program_latch_pair.in);
}

CoreViewScenePipelined::~CoreViewScenePipelined() {
    delete latch_if_id;
    delete latch_id_ex;
    delete latch_ex_mem;
    delete latch_mem_wb;
    delete inst_fetch;
    delete inst_dec;
    delete inst_exec;
    delete inst_mem;
    delete inst_wrb;
    if (hazard_unit != nullptr)
        delete hazard_unit;
}
