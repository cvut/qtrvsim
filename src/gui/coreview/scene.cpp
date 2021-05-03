#include "scene.h"

#include "common/logging.h"
#include "coreview/data.h"

#include <QMetaMethod>
#include <QSignalMapper>
#include <svgscene/components/groupitem.h>
#include <svgscene/components/hyperlinkitem.h>
#include <svgscene/components/simpletextitem.h>
#include <svgscene/svghandler.h>
#include <unordered_map>

using std::unordered_map;
using std::vector;
using svgscene::HyperlinkItem;
using svgscene::SimpleTextItem;
using svgscene::SvgDocument;
using svgscene::SvgDomTree;

LOG_CATEGORY("gui.coreview");

CoreViewScene::CoreViewScene(machine::Machine *machine, const QString &core_svg_scheme_name)
    : SvgGraphicsScene() {
    Q_UNUSED(machine)
    SvgDocument document
        = svgscene::parseFromFileName(this, QString(":/core/%1.svg").arg(core_svg_scheme_name));

    for (auto hyperlink_tree : document.getRoot().findAll<HyperlinkItem>()) {
        this->install_hyperlink(hyperlink_tree.getElement());
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

    install_values_from_document(document, values.bool_values, VALUE_SOURCE_NAME_MAPS.BOOL);
    install_values_from_document(document, values.reg_values, VALUE_SOURCE_NAME_MAPS.REG);
    install_values_from_document(document, values.debug_values, VALUE_SOURCE_NAME_MAPS.DEBUG);
    install_values_from_document(document, values.pc_values, VALUE_SOURCE_NAME_MAPS.PC);
    install_values_from_document(
        document, values.multi_text_values, VALUE_SOURCE_NAME_MAPS.MULTI_TEXT);
    install_values_from_document(
        document, values.instruction_values, VALUE_SOURCE_NAME_MAPS.INSTRUCTION);

    update_values();
}

// We add all items to scene and they are removed in QGraphicsScene
// destructor so we don't have to care about them here
CoreViewScene::~CoreViewScene() = default;

auto w = &CoreViewScene::request_cache_data;

void CoreViewScene::install_hyperlink(svgscene::HyperlinkItem *element) const {
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
template<typename T_handler, typename T>
void CoreViewScene::install_value(
    vector<T_handler> &handler_list,
    const unordered_map<QStringView, T> &value_source_name_map,
    SimpleTextItem *element,
    const QString &source_name) {
    try {
        handler_list.emplace_back(element, value_source_name_map.at(source_name));

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
    const unordered_map<QStringView, T> &value_source_name_map) {
    for (SvgDomTree<QGraphicsItem> component_tree :
         document.getRoot().findAll<QGraphicsItem>("data-component", T_handler::COMPONENT_NAME)) {
        SimpleTextItem *text_element = component_tree.find<SimpleTextItem>().getElement();
        QString source_name = component_tree.getAttrValueOr("data-source", "");
        install_value<T_handler, T>(handler_list, value_source_name_map, text_element, source_name);
    }
}

template<typename T>
void CoreViewScene::update_value_list(std::vector<T> &value_list) {
    for (auto &value_handler : value_list) {
        DEBUG("Calling full update of %s...", typeid(T).name());
        value_handler.update();
    }
}

void CoreViewScene::update_values() {
    update_value_list(values.bool_values);
    update_value_list(values.debug_values);
    update_value_list(values.reg_values);
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
