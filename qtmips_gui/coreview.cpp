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
    NEW(Junction, ft.junc_pc, 80, mem_program->connector_address()->y());
    NEW(Junction, ft.junc_pc_4, 130, 380);
    NEW(Multiplexer, ft.multiplex, 20, 390, 2);
    // Decode stage
    NEW(LogicBlock, dc.ctl_block, 230, 90, {"Control", "unit"});
    dc.ctl_block->setSize(35, 70);
    NEW(LogicBlock, dc.sign_ext, 230, 360, {"Sign", "extension"});
    NEW(LogicBlock, dc.shift2, 290, 390, "<<2");
    NEW(Adder, dc.add, 320, 390);
    const coreview::Connector *dc_con_sign_ext = dc.sign_ext->new_connector(1, 0);
    NEW(Junction, dc.j_sign_ext, 270, dc_con_sign_ext->y());
    NEW(LogicBlock, dc.cmp, 320, 200, "=");
    NEW(And, dc.and_branch, 350, 190);
    dc.cmp->setSize(24, 12);
    NEW(Junction, dc.j_inst_up, 180, 126);
    NEW(Junction, dc.j_inst_down, 180, dc_con_sign_ext->y());
    // Execute stage
    NEW(Alu, alu, 470, 230);
    // Memory stage
    NEW(Multiplexer, mem_or_reg, 570, 180, 2);

    // Connections //
    coreview::Connection *con;
    // Fetch stage
    new_bus(ft.pc->connector_out(), pc_latch_pair.in);
    new_bus(pc_latch_pair.out, ft.junc_pc->new_connector(coreview::Connector::AX_X));
    new_bus(ft.junc_pc->new_connector(coreview::Connector::AX_Y), ft.adder->connector_in_a());
    new_bus(ft.junc_pc->new_connector(coreview::Connector::AX_X), mem_program->connector_address());
    new_bus(ft.multiplex->connector_out(), ft.pc->connector_in());
    new_bus(ft.adder->connector_out(), ft.junc_pc_4->new_connector(coreview::Connector::AX_Y));
    con = new_bus(ft.junc_pc_4->new_connector(coreview::Connector::AX_X), ft.multiplex->connector_in(0));
    con->setAxes({CON_AXIS_Y(10)});
    // Decode stage
    new_bus(dc_con_sign_ext, dc.j_sign_ext->new_connector(coreview::Connector::AX_X));
    new_bus(dc.j_sign_ext->new_connector(coreview::Connector::AX_Y), dc.shift2->new_connector(-1, 0));
    new_bus(dc.shift2->new_connector(1, 0), dc.add->connector_in_a());
    con = new_signal(dc.cmp->new_connector(1, 0), dc.and_branch->connector_in(1));
    con->setAxes({CON_AXIS_Y(343)});
    con = new_signal(dc.ctl_block->new_connector(1, 0.9), dc.and_branch->connector_in(0));
    con->setAxes({CON_AXIS_Y(343)});
    dc.instr_bus = new_bus(dc.j_inst_up->new_connector(coreview::Connector::AX_Y), dc.j_inst_down->new_connector(coreview::Connector::AX_Y));
    new_bus(dc.j_inst_up->new_connector(coreview::Connector::AX_X), dc.ctl_block->new_connector(-1, 0), 2);
    new_bus(dc.j_inst_down->new_connector(coreview::Connector::AX_X), dc.sign_ext->new_connector(-1, 0), 2);
    // From decode stage to fetch stage
    con = new_signal(dc.and_branch->connector_out(), ft.multiplex->connector_ctl());
    con->setAxes({CON_AXIS_Y(365), CON_AXIS_X(490)});
    con = new_bus(dc.add->connector_out(), ft.multiplex->connector_in(1));
    con->setAxes({CON_AXIS_Y(340), CON_AXIS_X(480), CON_AXIS_Y(10)});

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
    delete ft.junc_pc;
    delete ft.junc_pc_4;
    delete ft.multiplex;
    delete dc.ctl_block;
    delete dc.sign_ext;
    delete dc.shift2;
    delete dc.cmp;
    delete dc.add;
    delete dc.j_sign_ext;
    delete dc.and_branch;
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

coreview::Bus *CoreViewScene::new_bus(const coreview::Connector *a, const coreview::Connector *b, unsigned width) {
    coreview::Bus *c = new coreview::Bus(a, b, width);
    connections.append(c);
    addItem(c);
    return c;
}

coreview::Signal *CoreViewScene::new_signal(const coreview::Connector *a, const coreview::Connector *b) {
    coreview::Signal *c = new coreview::Signal(a, b);
    connections.append(c);
    addItem(c);
    c->setZValue(-1); // hide connections under neath the items
    return c;
}

CoreViewSceneSimple::CoreViewSceneSimple(CoreView *view, machine::QtMipsMachine *machine) : CoreViewScene(view, machine) {
    NEW(Latch, delay_slot_latch, 160, 50, machine, 150);
}

CoreViewSceneSimple::~CoreViewSceneSimple() {
    delete delay_slot_latch;
}

CoreViewScenePipelined::CoreViewScenePipelined(CoreView *view, machine::QtMipsMachine *machine) : CoreViewScene(view, machine) {
    NEW(Latch, latch_if_id, 158, 90, machine, 380);
    latch_if_id->setTitle("IF/ID");
    NEW(Latch, latch_id_ex, 392, 90, machine, 380);
    latch_id_ex->setTitle("ID/EX");
    NEW(Latch, latch_ex_mem, 536, 90, machine, 380);
    latch_ex_mem->setTitle("EX/MEM");
    NEW(Latch, latch_mem_wb, 680, 90, machine, 380);
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

    coreview::Connection *con;
    // Fetch stage
    struct coreview::Latch::ConnectorPair lp_ft_inst = latch_if_id->new_connector(mem_program->connector_instruction()->y() - latch_if_id->y());
    new_bus(mem_program->connector_instruction(), lp_ft_inst.in);
    struct coreview::Latch::ConnectorPair lp_ft_pc = latch_if_id->new_connector(370);
    new_bus(ft.junc_pc_4->new_connector(coreview::Connector::AX_Y), lp_ft_pc.in);
    // Decode stage
    //new_bus(lp_ft_inst.out, dc.instr_bus->new_connector(lp_ft_inst.out->point()));
    con = new_bus(lp_ft_pc.out, dc.add->connector_in_b());
    con->setAxes({CON_AXIS_Y(270)});
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
