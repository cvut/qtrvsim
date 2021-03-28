#include "coreview.h"

#include "fontsize.h"

//////////////////////////////////////////////////////////////////////////////
/// Size of visible view area
constexpr size_t SC_WIDTH = 720;
constexpr size_t SC_HEIGHT = 540;
//////////////////////////////////////////////////////////////////////////////

static QMap<uint32_t, QString> excause_map
    = { { machine::EXCAUSE_NONE, "NONE" },         { machine::EXCAUSE_INT, "INT" },
        { machine::EXCAUSE_ADDRL, "ADDRL" },       { machine::EXCAUSE_ADDRS, "ADDRS" },
        { machine::EXCAUSE_IBUS, "IBUS" },         { machine::EXCAUSE_DBUS, "DBUS" },
        { machine::EXCAUSE_SYSCALL, "SYSCALL" },   { machine::EXCAUSE_BREAK, "BREAK" },
        { machine::EXCAUSE_OVERFLOW, "OVERFLOW" }, { machine::EXCAUSE_TRAP, "TRAP" },
        { machine::EXCAUSE_HWBREAK, "HWBREAK" } };

#define NEW_B(TYPE, VAR, ...)                                                  \
    do {                                                                       \
        VAR = new coreview::TYPE(__VA_ARGS__);                                 \
        addItem(VAR);                                                          \
    } while (false)
#define NEW(TYPE, VAR, X, Y, ...)                                              \
    do {                                                                       \
        NEW_B(TYPE, VAR, __VA_ARGS__);                                         \
        VAR->setPos(X, Y);                                                     \
    } while (false)
#define NEW_I(VAR, X, Y, SIG, ...)                                             \
    do {                                                                       \
        NEW(InstructionView, VAR, X, Y, __VA_ARGS__);                          \
        connect(                                                               \
            machine->core(), &machine::Core::SIG, VAR,                         \
            &coreview::InstructionView::instruction_update);                   \
    } while (false)
#define NEW_V(X, Y, SIG, ...)                                                  \
    do {                                                                       \
        NEW(Value, val, X, Y, __VA_ARGS__);                                    \
        connect(                                                               \
            machine->core(), &machine::Core::SIG, val,                         \
            &coreview::Value::value_update);                                   \
    } while (false)
#define NEW_MULTI(VAR, X, Y, SIG, ...)                                         \
    do {                                                                       \
        NEW(MultiText, VAR, X, Y, __VA_ARGS__);                                \
        connect(                                                               \
            machine->core(), &machine::Core::SIG, VAR,                         \
            &coreview::MultiText::multitext_update);                           \
    } while (false)

CoreViewScene::CoreViewScene(machine::Machine *machine, const QString &background_name)
    : QGraphicsScene() {
    setSceneRect(0, 0, SC_WIDTH, SC_HEIGHT);
    background = new QSvgWidget();
    background->load(QString(":/core/%1.svg").arg(background_name));
    background->setFixedSize(SC_WIDTH, SC_HEIGHT);
    background->setAttribute(Qt::WA_TranslucentBackground, true);
    this->addWidget(background);

    NEW(ProgramMemory, mem_program, 90, 240, machine);
    NEW(DataMemory, mem_data, 580, 258, machine);
    NEW(Registers, regs, 230, 240);
    NEW(LogicBlock, peripherals, 610, 350, "");
    peripherals->setSize(45, 16);
    NEW(LogicBlock, terminal, 610, 400, "");
    terminal->setSize(35, 16);
    NEW(ProgramCounter, ft.pc, 2, 280, machine);
    NEW_MULTI(
        mm.multi_excause, 602, 447, memory_excause_value, excause_map, true);
    new_label("Exception", 595, 437);

    coreview::Value *val;
    // Fetch stage values
    NEW_V(25, 440, fetch_branch_value, false, 1);
    NEW_V(360, 93, fetch_jump_reg_value, false, 1);
    // Decode stage values
    NEW_V(200, 200, decode_instruction_value); // Instruction
    NEW_V(360, 250, decode_reg1_value);        // Register output 1
    NEW_V(360, 270, decode_reg2_value);        // Register output 2
    NEW_V(335, 413, decode_immediate_value);   // Sign extended immediate value
    // NEW_V(370, 99, decode_regd31_value, false, 1);
    NEW_V(370, 113, decode_memtoreg_value, false, 1);
    NEW_V(360, 120, decode_memwrite_value, false, 1);
    NEW_V(370, 127, decode_memread_value, false, 1);
    // NEW_V(360, 140, decode_regdest_value, false, 1);
    NEW_V(370, 148, decode_alusrc_value, false, 1);
    // Execute stage
    NEW_V(450, 230, execute_reg1_value, true); // Register 1
    NEW_V(450, 310, execute_reg2_value, true); // Register 2
    NEW_V(527, 280, execute_alu_value, true);  // Alu output
    NEW_V(480, 413, execute_immediate_value);  // Immediate value
    NEW_V(470, 113, execute_memtoreg_value, false, 1);
    NEW_V(460, 120, execute_memwrite_value, false, 1);
    NEW_V(470, 127, execute_memread_value, false, 1);
    NEW_V(470, 127, execute_memread_value, false, 1);
    NEW_V(485, 345, execute_regdest_value, false, 1);
    NEW_V(475, 280, execute_alusrc_value, false, 1);
    // Memory stage
    NEW_V(560, 260, memory_alu_value, true); // Alu output
    NEW_V(560, 345, memory_rt_value, true);  // rt
    NEW_V(650, 290, memory_mem_value, true); // Memory output
    NEW_V(570, 113, memory_memtoreg_value, false, 1);
    NEW_V(630, 220, memory_memwrite_value, false, 1);
    NEW_V(620, 220, memory_memread_value, false, 1);
    // Write back stage
    NEW_V(710, 330, writeback_value, true); // Write back value

    NEW_V(205, 250, decode_rs_num_value, false, 2, 0, 10, ' ');
    NEW_V(205, 270, decode_rt_num_value, false, 2, 0, 10, ' ');

    NEW_V(320, 390, decode_rd_num_value, false, 2, 0, 10, ' ');
    NEW_V(320, 500, writeback_regw_num_value, false, 2, 0, 10, ' ');

    NEW_V(500, SC_HEIGHT - 12, cycle_c_value, false, 10, 0, 10, ' ', false);
    NEW_V(630, SC_HEIGHT - 12, stall_c_value, false, 10, 0, 10, ' ', false);

    setBackgroundBrush(QBrush(Qt::white));

    connect(
        regs, &coreview::Registers::open_registers, this,
        &CoreViewScene::request_registers);
    connect(
        mem_program, &coreview::Memory::open_mem, this,
        &CoreViewScene::request_program_memory);
    connect(
        mem_data, &coreview::Memory::open_mem, this,
        &CoreViewScene::request_data_memory);
    connect(
        ft.pc, &coreview::ProgramCounter::open_program, this,
        &CoreViewScene::request_program_memory);
    connect(
        ft.pc, &coreview::ProgramCounter::jump_to_pc, this,
        &CoreViewScene::request_jump_to_program_counter);
    connect(
        mem_program, &coreview::Memory::open_cache, this,
        &CoreViewScene::request_cache_program);
    connect(
        mem_data, &coreview::Memory::open_cache, this,
        &CoreViewScene::request_cache_data);
    connect(
        peripherals, &coreview::LogicBlock::open_block, this,
        &CoreViewScene::request_peripherals);
    connect(
        terminal, &coreview::LogicBlock::open_block, this,
        &CoreViewScene::request_terminal);
}

// We add all items to scene and they are removed in QGraphicsScene
// destructor so we don't have to care about them here
CoreViewScene::~CoreViewScene() = default;

QGraphicsSimpleTextItem *
CoreViewScene::new_label(const QString &str, qreal x, qreal y) {
    auto *i = new QGraphicsSimpleTextItem(str);
    QFont f;
    f.setPointSize(FontSize::SIZE5);
    i->setFont(f);
    addItem(i);
    i->setPos(x, y);
    return i;
}

CoreViewSceneSimple::CoreViewSceneSimple(machine::Machine *machine)
    : CoreViewScene(machine, "simple") {
    NEW_I(inst_prim, 230, 60, instruction_executed, QColor(255, 173, 230));

    coreview::Value *val;
    // Label for write back stage
    NEW_V(280, 200, writeback_regw_value, false, 1);
}

CoreViewScenePipelined::CoreViewScenePipelined(machine::Machine *machine)
    : CoreViewScene(
        machine,
        (machine->config().hazard_unit() == machine::MachineConfig::HU_STALL_FORWARD)
            ? "forwarding"
            : "pipeline") {
    NEW_I(inst_fetch, 79, 2, instruction_fetched, QColor(255, 173, 173));
    NEW_I(inst_dec, 275, 2, instruction_decoded, QColor(255, 212, 173));
    NEW_I(inst_exec, 464, 2, instruction_executed, QColor(193, 255, 173));
    NEW_I(inst_mem, 598, 2, instruction_memory, QColor(173, 255, 229));
    NEW_I(inst_wrb, 660, 18, instruction_writeback, QColor(255, 173, 230));

    if (machine->config().hazard_unit() != machine::MachineConfig::HU_NONE) {
        NEW(LogicBlock, hazard_unit, SC_WIDTH / 2, SC_HEIGHT - 15,
            "Hazard Unit");
        hazard_unit->setSize(SC_WIDTH - 100, 12);
        static QMap<uint32_t, QString> stall_map
            = { { 0, "NORMAL" }, { 1, "STALL" }, { 2, "FORWARD" } };
        NEW_MULTI(
            hu.multi_stall, 480, 447, execute_stall_forward_value, stall_map);
        NEW_MULTI(hu.multi_stall, 310, 340, branch_forward_value, stall_map);
        NEW_MULTI(hu.multi_stall, 250, SC_HEIGHT - 18, hu_stall_value, stall_map);
    }

    coreview::Value *val;
    // Label for write back stage
    NEW_V(460, 45, writeback_regw_value, false, 1);
    NEW_V(360, 105, decode_regw_value, false, 1);
    NEW_V(460, 105, execute_regw_value, false, 1);
    NEW_V(560, 105, memory_regw_value, false, 1);

    NEW_V(450, 390, execute_rd_num_value, false, 2, 0, 10, ' ');
    //    NEW_V(610, 390, memory_rd_num_value, false, 2, 0, 10, ' ');

    if (machine->config().hazard_unit()
        == machine::MachineConfig::HU_STALL_FORWARD) {
        NEW_V(434, 227, execute_reg1_ff_value, false, 1); // Register 1 forward
                                                          // to ALU
        NEW_V(434, 280, execute_reg2_ff_value, false, 1); // Register 2 forward
                                                          // to ALU

        //        NEW_V(291, 230, forward_m_d_rs_value, false, 1); // Register 1
        //        forward
        // for bxx and jr, jalr
        //        NEW_V(333, 230, forward_m_d_rt_value, false, 1); // Register 2
        //        forward
        // for beq, bne
    }
}
