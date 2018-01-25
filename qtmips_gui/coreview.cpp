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
    /*
    QPen pen;
    pen.setBrush(QBrush(QColor(220, 220, 220)));
    addLine(width()/2, 0, width()/2, height(), pen);
    addLine(0, height()/2, width(), height()/2, pen);
    addRect(0.5, 0.5, width() - 0.5, height() - 0.5, pen);
    */
    // TODO remove

    // Elements //
    // Primary points
    NEW(ProgramMemory, mem_program, 90, 240, machine);
    NEW(DataMemory, mem_data, 580, 265, machine);
    NEW(Registers, regs, 230, 240);
    NEW(Alu, alu, 470, 225);
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
    NEW(LogicBlock, dc.sign_ext, 250, 400, {"Sign", "extension"});
    NEW(LogicBlock, dc.shift2, 310, 428, "<<2");
    NEW(Adder, dc.add, 340, 428);
    const coreview::Connector *dc_con_sign_ext = dc.sign_ext->new_connector(1, 0);
    NEW(Junction, dc.j_sign_ext, 290, dc_con_sign_ext->y());
    NEW(LogicBlock, dc.cmp, 320, 200, "=");
    NEW(And, dc.and_branch, 350, 190);
    dc.cmp->setSize(24, 12);
    NEW(Junction, dc.j_inst_up, 190, 126);
    NEW(Junction, dc.j_inst_down, 190, dc_con_sign_ext->y());
    // Execute stage
    NEW(Junction, ex.j_mux, 420, 316);
    NEW(Multiplexer, ex.mux_imm, 450, 306, 2);
    // Memory
    NEW(Junction, mm.j_addr, 570, mem_data->connector_address()->y());
    // WriteBack stage
    NEW(Multiplexer, wb.mem_or_reg, 690, 252, 2);

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
    new_bus(dc.instr_bus->new_connector(regs->connector_read1_reg()->point()), regs->connector_read1_reg(), 2);
    new_bus(dc.instr_bus->new_connector(regs->connector_read2_reg()->point()), regs->connector_read2_reg(), 2);
    // Execute stage
    new_bus(ex.j_mux->new_connector(CON_AX_X), ex.mux_imm->connector_in(0));
    new_bus(ex.mux_imm->connector_out(), alu->connector_in_b());
    // Memory stage
    new_bus(mm.j_addr->new_connector(CON_AX_X), mem_data->connector_address());
    // From write back stage to decode stage
    con = new_bus(wb.mem_or_reg->connector_out(), regs->connector_write());
    con->setAxes({CON_AXIS_Y(710), CON_AXIS_X(510), CON_AXIS_Y(172)});

    connect(regs, SIGNAL(open_registers()), this, SIGNAL(request_registers()));
    connect(mem_program, SIGNAL(open_mem()), this, SIGNAL(request_program_memory()));
    connect(mem_data, SIGNAL(open_mem()), this, SIGNAL(request_data_memory()));
    connect(ft.pc, SIGNAL(open_program()), this, SIGNAL(request_program_memory()));
    connect(ft.pc, SIGNAL(jump_to_pc(std::uint32_t)), this, SIGNAL(request_jump_to_program_counter(std::uint32_t)));
}

CoreViewScene::~CoreViewScene() {
    // We add all items to scene and they are removed in QGraphicsScene descructor so we don't have to care about them here
}

coreview::Connection *CoreViewScene::new_connection(const coreview::Connector *a, const coreview::Connector *b) {
    coreview::Connection *c = new coreview::Connection(a, b);
    addItem(c);
    c->setZValue(-1); // hide connections under neath the items
    return c;
}

coreview::Bus *CoreViewScene::new_bus(const coreview::Connector *a, const coreview::Connector *b, unsigned width) {
    coreview::Bus *c = new coreview::Bus(a, b, width);
    addItem(c);
    c->setZValue(-2); // hide connections under neath the items
    return c;
}

coreview::Signal *CoreViewScene::new_signal(const coreview::Connector *a, const coreview::Connector *b) {
    coreview::Signal *c = new coreview::Signal(a, b);
    addItem(c);
    c->setZValue(-3); // hide connections under neath the items
    return c;
}

CoreViewSceneSimple::CoreViewSceneSimple(CoreView *view, machine::QtMipsMachine *machine) : CoreViewScene(view, machine) {
    NEW_I(instr_prim, 230, 60, instruction_fetched(const machine::Instruction&));
    if (machine->config().delay_slot()) {
        NEW(Latch, delay_slot_latch, 55, 470, machine, 25);
        NEW_I(instr_delay, 60, 500, instruction_program_counter(const machine::Instruction&));
    }

    coreview::Connection *con;
    // Fetch stage
    new_bus(mem_program->connector_instruction(), dc.instr_bus->new_connector(mem_program->connector_instruction()->point()));
    con = new_bus(ft.junc_pc_4->new_connector(coreview::Connector::AX_Y), dc.add->connector_in_b());
    con->setAxes({CON_AXIS_Y(270)});
    // Decode stage
    con = new_bus(regs->connector_read1(), alu->connector_in_a());
    con->setAxes({CON_AXIS_Y(445)});
    con = new_bus(regs->connector_read2(), ex.mux_imm->connector_in(0));
    con->setAxes({CON_AXIS_Y(420)});
    con = new_bus(dc.j_sign_ext->new_connector(coreview::Connector::AX_X), ex.mux_imm->connector_in(1));
    con->setAxes({CON_AXIS_Y(440)});
    // Execute
    new_bus(alu->connector_out(), mm.j_addr->new_connector(CON_AX_X));
    con = new_bus(ex.j_mux->new_connector(CON_AX_Y), mem_data->connector_data_in());
    con->setAxes({CON_AXIS_X(360), CON_AXIS_Y(560)});
    // Memory
    con = new_bus(mm.j_addr->new_connector(CON_AX_Y), wb.mem_or_reg->connector_in(0));
    con->setAxes({CON_AXIS_X(250), CON_AXIS_Y(678)});
    con = new_bus(mem_data->connector_data_out(), wb.mem_or_reg->connector_in(1));
    con->setAxes({CON_AXIS_Y(678)});
    // WriteBack
    // From decode stage to fetch stage
    if (machine->config().delay_slot()) {
        struct coreview::Latch::ConnectorPair lp_addr = delay_slot_latch->new_connector(10);
        struct coreview::Latch::ConnectorPair lp_branch = delay_slot_latch->new_connector(20);
        con = new_signal(dc.and_branch->connector_out(), lp_branch.out);
        con->setAxes({CON_AXIS_Y(365)});
        new_signal(lp_branch.in, ft.multiplex->connector_ctl());
        con = new_bus(dc.add->connector_out(), lp_addr.out);
        con->setAxes({CON_AXIS_Y(360)});
        con = new_bus(lp_addr.in, ft.multiplex->connector_in(1));
        con->setAxes({CON_AXIS_Y(10)});
    } else {
        con = new_signal(dc.and_branch->connector_out(), ft.multiplex->connector_ctl());
        con->setAxes({CON_AXIS_Y(365), CON_AXIS_X(490)});
        con = new_bus(dc.add->connector_out(), ft.multiplex->connector_in(1));
        con->setAxes({CON_AXIS_Y(360), CON_AXIS_X(480), CON_AXIS_Y(10)});
    }
}

CoreViewScenePipelined::CoreViewScenePipelined(CoreView *view, machine::QtMipsMachine *machine) : CoreViewScene(view, machine) {
    NEW(Latch, latch_if_id, 158, 70, machine, 400);
    latch_if_id->setTitle("IF/ID");
    NEW(Latch, latch_id_ex, 392, 70, machine, 400);
    latch_id_ex->setTitle("ID/EX");
    NEW(Latch, latch_ex_mem, 536, 70, machine, 400);
    latch_ex_mem->setTitle("EX/MEM");
    NEW(Latch, latch_mem_wb, 660, 70, machine, 400);
    latch_mem_wb->setTitle("MEM/WB");

    NEW_I(inst_fetch, 79, 2, instruction_fetched(const machine::Instruction&));
    NEW_I(inst_dec, 275, 2, instruction_decoded(const machine::Instruction&));
    NEW_I(inst_exec, 464, 2, instruction_executed(const machine::Instruction&));
    NEW_I(inst_mem, 598, 2, instruction_memory(const machine::Instruction&));
    NEW_I(inst_wrb, 660, 18, instruction_writeback(const machine::Instruction&));

    if (machine->config().hazard_unit() != machine::MachineConfig::HU_NONE) {
        NEW(LogicBlock, hazard_unit, SC_WIDTH/2, SC_HEIGHT - 15, "Hazard Unit");
        hazard_unit->setSize(SC_WIDTH - 100, 12);
    }

    coreview::Connection *con;
    // Fetch stage
    struct coreview::Latch::ConnectorPair lp_ft_inst = latch_if_id->new_connector(mem_program->connector_instruction()->y() - latch_if_id->y());
    new_bus(mem_program->connector_instruction(), lp_ft_inst.in);
    struct coreview::Latch::ConnectorPair lp_ft_pc = latch_if_id->new_connector(390);
    new_bus(ft.junc_pc_4->new_connector(coreview::Connector::AX_Y), lp_ft_pc.in);
    // Decode stage
    new_bus(lp_ft_inst.out, dc.instr_bus->new_connector(lp_ft_inst.out->point()));
    con = new_bus(lp_ft_pc.out, dc.add->connector_in_b());
    con->setAxes({CON_AXIS_Y(270)});
    struct coreview::Latch::ConnectorPair lp_dc_rs = latch_id_ex->new_connector(regs->connector_read1()->y() - latch_id_ex->y());
    struct coreview::Latch::ConnectorPair lp_dc_rt = latch_id_ex->new_connector(regs->connector_read2()->y() - latch_id_ex->y());
    new_bus(regs->connector_read1(), lp_dc_rs.in);
    new_bus(regs->connector_read2(), lp_dc_rt.in);
    struct coreview::Latch::ConnectorPair lp_dc_immed = latch_id_ex->new_connector(dc.j_sign_ext->y() - latch_id_ex->y());
    new_bus(dc.j_sign_ext->new_connector(coreview::Connector::AX_X), lp_dc_immed.in);
    // Execute
    con = new_bus(lp_dc_rs.out, alu->connector_in_a());
    con->setAxes({CON_AXIS_Y(445)});
    con = new_bus(lp_dc_rt.out, ex.mux_imm->connector_in(0));
    con->setAxes({CON_AXIS_Y(420)});
    con = new_bus(lp_dc_immed.out, ex.mux_imm->connector_in(1));
    con->setAxes({CON_AXIS_Y(440)});
    struct coreview::Latch::ConnectorPair lp_ex_alu = latch_ex_mem->new_connector(alu->connector_out()->y() - latch_ex_mem->y());
    struct coreview::Latch::ConnectorPair lp_ex_dt = latch_ex_mem->new_connector(290);
    new_bus(alu->connector_out(), lp_ex_alu.in);
    new_bus(ex.j_mux->new_connector(CON_AX_Y), lp_ex_dt.in);
    // Memory
    new_bus(lp_ex_alu.out, mm.j_addr->new_connector(CON_AX_X));
    con = new_bus(lp_ex_dt.out, mem_data->connector_data_in());
    con->setAxes({CON_AXIS_Y(560)});
    struct coreview::Latch::ConnectorPair lp_mem_alu = latch_mem_wb->new_connector(180);
    struct coreview::Latch::ConnectorPair lp_mem_mem = latch_mem_wb->new_connector(mem_data->connector_data_out()->y() - latch_mem_wb->y());
    new_bus(mm.j_addr->new_connector(CON_AX_Y), lp_mem_alu.in);
    new_bus(mem_data->connector_data_out(), lp_mem_mem.in);
    // WriteBack
    con = new_bus(lp_mem_alu.out, wb.mem_or_reg->connector_in(0));
    con->setAxes({CON_AXIS_Y(678)});
    con = new_bus(lp_mem_mem.out, wb.mem_or_reg->connector_in(1));
    con->setAxes({CON_AXIS_Y(678)});
    // From decode stage to fetch stage
    con = new_signal(dc.and_branch->connector_out(), ft.multiplex->connector_ctl());
    con->setAxes({CON_AXIS_Y(365), CON_AXIS_X(490)});
    con = new_bus(dc.add->connector_out(), ft.multiplex->connector_in(1));
    con->setAxes({CON_AXIS_Y(360), CON_AXIS_X(480), CON_AXIS_Y(10)});
}
