#include "scene.h"

#include "common/logging.h"
#include "coreview/data.h"
#include "machine/core.h"

#include <svgscene/components/groupitem.h>
#include <svgscene/components/hyperlinkitem.h>
#include <svgscene/components/simpletextitem.h>
#include <svgscene/svghandler.h>
#include <unordered_map>
#include <vector>

using std::unordered_map;
using std::vector;
using svgscene::HyperlinkItem;
using svgscene::SimpleTextItem;
using svgscene::SvgDocument;
using svgscene::SvgDomTree;

LOG_CATEGORY("gui.coreview");

// Template specialization must be declared before usage.
template<>
void CoreViewScene::install_values_from_document<PCValue>(
    const SvgDocument &document,
    vector<PCValue> &handler_list,
    const unordered_map<QStringView, Lens<CoreState, Address>> &value_source_name_map,
    const CoreState &core_state);

CoreViewScene::CoreViewScene(machine::Machine *machine, const QString &core_svg_scheme_name)
    : SvgGraphicsScene() {
    Q_UNUSED(machine)

    // Update coreview each core step.
    connect(machine->core(), &machine::Core::step_done, this, &CoreViewScene::update_values);

    SvgDocument document
        = svgscene::parseFromFileName(this, QString(":/core/%1.svg").arg(core_svg_scheme_name));

    for (auto hyperlink_tree : document.getRoot().findAll<HyperlinkItem>()) {
        this->install_hyperlink(hyperlink_tree.getElement());
    }
    { // Program cache install
        auto program_cache_tree = document.getRoot().find("data-component", "program-cache");
        if (machine->config().cache_program().enabled()) {
            auto texts = program_cache_tree.findAll<SimpleTextItem>();
            // Diagrams.net dow not allow me, to put there some marks. :(
            auto miss = texts.takeLast().getElement();
            auto hit = texts.takeLast().getElement();
            program_cache.reset(new Cache(machine->cache_program(), hit, miss));
        } else {
            program_cache_tree.getElement()->hide();
        }
    }
    { // Data cache install
        auto data_cache_tree = document.getRoot().find("data-component", "data-cache");
        if (machine->config().cache_data().enabled()) {
            auto texts = data_cache_tree.findAll<SimpleTextItem>();
            // Diagrams.net dow not allow me, to put there some marks. :(
            auto miss = texts.takeLast().getElement();
            auto hit = texts.takeLast().getElement();
            data_cache.reset(new Cache(machine->cache_data(), hit, miss));
        } else {
            data_cache_tree.getElement()->hide();
        }
    }
    if (machine->config().hazard_unit() == machine::MachineConfig::HU_NONE) {
        // Hazard unit conditional hide
        for (auto elem_tree : document.getRoot().findAll("data-tags", "hazardunit")) {
            elem_tree.getElement()->hide();
        }
    }

    /*
     * TODO:
     * Components not implemented:
     * - MUX
     * - colored frames on special values
     *
     * Invert the registering process, so that the tree is traversed
     * only once.
     */

    const machine::CoreState &core_state = machine->core()->state;
    install_values_from_document(
        document, values.bool_values, VALUE_SOURCE_NAME_MAPS.BOOL, core_state);
    install_values_from_document(
        document, values.reg_values, VALUE_SOURCE_NAME_MAPS.REG, core_state);
    install_values_from_document(
        document, values.reg_id_values, VALUE_SOURCE_NAME_MAPS.REG_ID, core_state);
    install_values_from_document(
        document, values.debug_values, VALUE_SOURCE_NAME_MAPS.DEBUG, core_state);
    install_values_from_document(document, values.pc_values, VALUE_SOURCE_NAME_MAPS.PC, core_state);
    install_values_from_document(
        document, values.multi_text_values, VALUE_SOURCE_NAME_MAPS.MULTI_TEXT, core_state);
    install_values_from_document(
        document, values.instruction_values, VALUE_SOURCE_NAME_MAPS.INSTRUCTION, core_state);

    update_values();
}

CoreViewScene::~CoreViewScene() = default;

void CoreViewScene::install_hyperlink(svgscene::HyperlinkItem *element) const {
    if (element->getTargetName() == "#") {
        DEBUG("Skipping NOP hyperlink.");
        return;
    }
    try {
        connect(
            element, &svgscene::HyperlinkItem::triggered, this,
            HYPERLINK_TARGETS.at(element->getTargetName()));
        DEBUG("Registered hyperlink to target %s", qPrintable(element->getTargetName()));
    } catch (std::out_of_range &) {
        WARN(
            "Registering hyperlink without valid target (href: \"%s\").",
            qPrintable(element->getTargetName()));
    }
}
template<typename T_handler, typename T>
void CoreViewScene::install_value(
    vector<T_handler> &handler_list,
    const unordered_map<QStringView, T> &value_source_name_map,
    SimpleTextItem *element,
    const QString &source_name,
    const CoreState &core_state) {
    try {
        handler_list.emplace_back(element, value_source_name_map.at(source_name)(core_state));

        DEBUG(
            "Installing value %s with source %s.", T_handler::COMPONENT_NAME,
            qPrintable(source_name));
    } catch (std::out_of_range &) {
        WARN("Source for bool value not found (source: \"%s\").", qPrintable(source_name));
    }
}

template<typename T_handler, typename T>
void CoreViewScene::install_values_from_document(
    const SvgDocument &document,
    vector<T_handler> &handler_list,
    const unordered_map<QStringView, T> &value_source_name_map,
    const CoreState &core_state) {
    for (SvgDomTree<QGraphicsItem> component_tree :
         document.getRoot().findAll("data-component", T_handler::COMPONENT_NAME)) {
        SimpleTextItem *text_element = component_tree.find<SimpleTextItem>().getElement();
        QString source_name = component_tree.getAttrValueOr("data-source", "");
        install_value<T_handler, T>(
            handler_list, value_source_name_map, text_element, source_name, core_state);
    }
}

template<>
void CoreViewScene::install_values_from_document<PCValue>(
    const SvgDocument &document,
    vector<PCValue> &handler_list,
    const unordered_map<QStringView, Lens<CoreState, Address>> &value_source_name_map,
    const CoreState &core_state) {
    for (SvgDomTree<QGraphicsItem> component_tree :
         document.getRoot().findAll("data-component", PCValue::COMPONENT_NAME)) {
        SimpleTextItem *text_element = component_tree.find<SimpleTextItem>().getElement();
        QString source_name = component_tree.getAttrValueOr("data-source", "");
        install_value<PCValue, Lens<CoreState, Address>>(
            handler_list, value_source_name_map, text_element, source_name, core_state);
        try {
            PCValue *handler = &handler_list.back();
            HyperlinkItem *hyperlink = component_tree.find<HyperlinkItem>().getElement();
            connect(hyperlink, &HyperlinkItem::triggered, handler, &PCValue::clicked);
            connect(
                handler, &PCValue::jump_to_pc, this,
                &CoreViewScene::request_jump_to_program_counter);
            connect(
                hyperlink, &HyperlinkItem::triggered, this, &CoreViewScene::request_program_memory);
        } catch (std::out_of_range &e) { DEBUG("PC component without a hyperlink."); }
    }
}

template<typename T>
void CoreViewScene::update_value_list(std::vector<T> &value_list) {
    DEBUG("Calling full update of %s...", typeid(T).name());
    for (auto &value_handler : value_list) {
        value_handler.update();
    }
}

void CoreViewScene::update_values() {
    update_value_list(values.bool_values);
    update_value_list(values.debug_values);
    update_value_list(values.reg_values);
    update_value_list(values.reg_id_values);
    update_value_list(values.pc_values);
    update_value_list(values.multi_text_values);
    update_value_list(values.instruction_values);
}

CoreViewSceneSimple::CoreViewSceneSimple(machine::Machine *machine)
    : CoreViewScene(machine, "simple") {}

CoreViewScenePipelined::CoreViewScenePipelined(machine::Machine *machine)
    : CoreViewScene(
        machine,
        (machine->config().hazard_unit() == machine::MachineConfig::HU_STALL_FORWARD)
            ? "forwarding"
            : "pipeline") {}
