#include "coreview.h"

#include "common/logging.h"
#include "fontsize.h"

#include <QMetaMethod>
#include <QSignalMapper>
#include <QXmlStreamReader>
#include <svgscene/components/groupitem.h>
#include <svgscene/components/hyperlinkitem.h>
#include <svgscene/components/simpletextitem.h>
#include <svgscene/svghandler.h>
#include <unordered_map>

using svgscene::HyperlinkItem;
using svgscene::SimpleTextItem;

LOG_CATEGORY("gui.coreview");

//////////////////////////////////////////////////////////////////////////////
/// Size of visible view area
constexpr size_t SC_WIDTH = 720;
constexpr size_t SC_HEIGHT = 540;
//////////////////////////////////////////////////////////////////////////////

/**
 * Link targets available for use in the SVG.
 *
 * EXAMPLE:
 * ```svg
 *  <a xlink:href="#registers">
 *    <text>Registers</text>
 *  </a>
 * ```
 */
const std::unordered_map<QString, void (::CoreViewScene::*)()> HYPERLINK_TARGETS {
    { "#registers", &CoreViewScene::request_registers },
    { "#cache_data", &CoreViewScene::request_cache_data },
    { "#cache_program", &CoreViewScene::request_cache_program },
    { "#data_memory", &CoreViewScene::request_data_memory },
    { "#peripherals", &CoreViewScene::request_peripherals },
    { "#program_memory", &CoreViewScene::request_program_memory },
    { "#terminal", &CoreViewScene::request_terminal },
};

static const std::vector<QString> EXCEPTION_NAME_TABLE
    = { { "NONE" },      // machine::EXCAUSE_NONE
        { "INT" },       // machine::EXCAUSE_INT
        { "ADDRL" },     // machine::EXCAUSE_ADDRL
        { "ADDRS" },     // machine::EXCAUSE_ADDRS
        { "IBUS" },      // machine::EXCAUSE_IBUS
        { "DBUS" },      // machine::EXCAUSE_DBUS
        { "SYSCALL" },   // machine::EXCAUSE_SYSCALL
        { "BREAK" },     // machine::EXCAUSE_BREAK
        { "OVERFLOW" },  // machine::EXCAUSE_OVERFLOW
        { "TRAP" },      // machine::EXCAUSE_TRAP
        { "HWBREAK" } }; // machine::EXCAUSE_HWBREAK

static const std::vector<QString> STALL_TEXT_TABLE = { { "NORMAL" }, { "STALL" }, { "FORWARD" } };

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
    : SvgGraphicsScene() {
    svgscene::SvgHandler h(this);
    QFile f(QString(":/core/%1.svg").arg(background_name));
    f.open(QIODevice::ReadOnly);
    QXmlStreamReader xml(&f);
    h.load(&xml);
    svgscene::SvgDocument document = h.getDocument();

    for (auto hyperlink_tree : document.getRoot().findAll<HyperlinkItem>()) {
        this->register_hyperlink(hyperlink_tree.getElement());
    }
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

auto w = &CoreViewScene::request_cache_data;

void CoreViewScene::register_hyperlink(svgscene::HyperlinkItem *element) const {
    try {
        connect(
            element, &svgscene::HyperlinkItem::triggered, this,
            HYPERLINK_TARGETS.at(element->getTargetName()));
        LOG("Registered hyperlink to target %s", qPrintable(element->getTargetName()));
    } catch (std::out_of_range &) {
        WARN(
            "Registering hyperlink without valid target (href: \"%s\").",
            qPrintable(element->getTargetName()));
    }
}

CoreViewSceneSimple::CoreViewSceneSimple(machine::Machine *machine)
    : CoreViewScene(machine, "simple") {
    //    NEW_I(inst_prim, 230, 60, instruction_executed, QColor(255, 173,
    //    230));
    //
    //    coreview::Value *val;
    //    // Label for write back stage
    //    NEW_V(280, 200, writeback_regw_value, false, 1);
}

CoreViewScenePipelined::CoreViewScenePipelined(machine::Machine *machine)
    : CoreViewScene(
        machine,
        (machine->config().hazard_unit() == machine::MachineConfig::HU_STALL_FORWARD)
            ? "forwarding"
            : "pipeline") {
    //    NEW_I(inst_fetch, 79, 2, instruction_fetched, QColor(255, 173, 173));
    //    NEW_I(inst_dec, 275, 2, instruction_decoded, QColor(255, 212, 173));
    //    NEW_I(inst_exec, 464, 2, instruction_executed, QColor(193, 255, 173));
    //    NEW_I(inst_mem, 598, 2, instruction_memory, QColor(173, 255, 229));
    //    NEW_I(inst_wrb, 660, 18, instruction_writeback, QColor(255, 173,
    //    230));
    //
    //    if (machine->config().hazard_unit() !=
    //    machine::MachineConfig::HU_NONE) {
    //        NEW(LogicBlock, hazard_unit, SC_WIDTH / 2, SC_HEIGHT - 15,
    //            "Hazard Unit");
    //        hazard_unit->setSize(SC_WIDTH - 100, 12);
    //        NEW_MULTI(
    //            hu.multi_stall, 480, 447, execute_stall_forward_value,
    //            STALL_TEXT_TABLE);
    //        NEW_MULTI(
    //            hu.multi_stall, 310, 340, branch_forward_value,
    //            STALL_TEXT_TABLE);
    //        NEW_MULTI(
    //            hu.multi_stall, 250, SC_HEIGHT - 18, hu_stall_value,
    //            STALL_TEXT_TABLE);
    //    }

    //    coreview::Value *val;
    //    // Label for write back stage
    //    NEW_V(460, 45, writeback_regw_value, false, 1);
    //    NEW_V(360, 105, decode_regw_value, false, 1);
    //    NEW_V(460, 105, execute_regw_value, false, 1);
    //    NEW_V(560, 105, memory_regw_value, false, 1);
    //
    //    NEW_V(450, 390, execute_rd_num_value, false, 2, 0, 10, ' ');
    //    NEW_V(610, 390, memory_rd_num_value, false, 2, 0, 10, ' ');
    //
    //    if (machine->config().hazard_unit()
    //        == machine::MachineConfig::HU_STALL_FORWARD) {
    //        NEW_V(434, 227, execute_reg1_ff_value, false, 1); // Register 1
    //        forward
    //                                                          // to ALU
    //        NEW_V(434, 280, execute_reg2_ff_value, false, 1); // Register 2
    //        forward
    //                                                          // to ALU
    //
    //        //        NEW_V(291, 230, forward_m_d_rs_value, false, 1); //
    //        Register 1
    //        //        forward
    //        // for bxx and jr, jalr
    //        //        NEW_V(333, 230, forward_m_d_rt_value, false, 1); //
    //        Register 2
    //        //        forward
    //        // for beq, bne
    //    }
}
