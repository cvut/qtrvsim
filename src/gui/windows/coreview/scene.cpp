#include "scene.h"

#include "common/logging.h"
#include "data.h"
#include "machine/core.h"

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

CoreViewScene::CoreViewScene(machine::Machine *machine, const QString &core_svg_scheme_name)
    : SvgGraphicsScene()
    , program_counter_value((VALUE_SOURCE_NAME_MAPS.PC.at(QStringLiteral("fetch-pc")))(
          machine->core()->get_state())) {
    SvgDocument document
        = svgscene::parseFromFileName(this, QString(":/core/%1.svg").arg(core_svg_scheme_name));

    for (auto hyperlink_tree : document.getRoot().findAll<HyperlinkItem>()) {
        this->install_hyperlink(hyperlink_tree.getElement());
    }

    /*
     * TODO:
     *      Components not implemented:
     *      - colored frames on special values
     */

    const machine::CoreState &core_state = machine->core()->get_state();

    // Find all components in the DOM tree and install controllers for them.
    for (auto component : document.getRoot().findAll("data-component")) {
        QStringView component_name = component.getAttrValueOr("data-component");
        if (component_name.isEmpty()) { continue; }
        // This switch is performance optimization.
        // Single char lookup will usually give only one match, outperforming
        // a hashtable, which has to check the key in the end as well hut
        // hashing is more complex than single char lookup.
        switch (component_name.at(0).toLatin1()) {
        case 'b': {
            if (component_name == BoolValue::COMPONENT_NAME) {
                install_value(
                    values.bool_values, VALUE_SOURCE_NAME_MAPS.BOOL, component, core_state);
            }
            break;
        }
        case 'd': {
            if (component_name == DebugValue::COMPONENT_NAME) {
                install_value(
                    values.debug_values, VALUE_SOURCE_NAME_MAPS.DEBUG_VAL, component, core_state);
            } else if (component_name == QStringLiteral("data-cache")) {
                if (machine->config().cache_data().enabled()) {
                    auto texts = component.findAll<SimpleTextItem>();
                    // Diagrams.net dow not allow me, to put there some marks.
                    // :(
                    auto miss = texts.takeLast().getElement();
                    auto hit = texts.takeLast().getElement();
                    data_cache.reset(new Cache(machine->cache_data(), hit, miss));
                } else {
                    component.getElement()->hide();
                }
            }
            break;
        }
        case 'i': {
            if (component_name == InstructionValue::COMPONENT_NAME) {
                install_value(
                    values.instruction_values, VALUE_SOURCE_NAME_MAPS.INSTRUCTION, component,
                    core_state);
            }
            break;
        }
        case 'm': {
            if (component_name == MultiTextValue::COMPONENT_NAME) {
                install_value(
                    values.multi_text_values, VALUE_SOURCE_NAME_MAPS.MULTI_TEXT, component,
                    core_state);
            } else if (component_name == QStringLiteral("mux2")) {
                const QString &source_name = component.getAttrValueOr("data-source");
                // Draw.io does not allow tagging the paths, to I use this style identification hack.
                auto conn_trees = component.findAll<QGraphicsPathItem>("stroke-linecap", "round");
                if (conn_trees.size() != 2) {
                    WARN(
                        "Mux2 does not have 2 connections found %zi (source: \"%s\").",
                        conn_trees.size(), qPrintable(source_name));
                    break;
                }
                std::vector<QGraphicsPathItem *> connections;
                connections.reserve(conn_trees.size());
                std::transform(
                    conn_trees.begin(), conn_trees.end(), std::back_inserter(connections),
                    [](SvgDomTree<QGraphicsPathItem> &e) { return e.getElement(); });
                try {
                    const bool &source = VALUE_SOURCE_NAME_MAPS.BOOL.at(source_name)(core_state);
                    values.mux2_values.emplace_back(std::move(connections), source);
                } catch (std::out_of_range &e) {
                    WARN(
                        "Source for mux2 value not found (source: \"%s\").",
                        qPrintable(source_name));
                }
            } else if (component_name == QStringLiteral("mux3")) {
                const QString &source_name = component.getAttrValueOr("data-source");
                // Draw.io does not allow tagging the paths, to I use this style identification hack.
                auto conn_trees = component.findAll<QGraphicsPathItem>("stroke-linecap", "round");
                if (conn_trees.size() != 3) {
                    WARN(
                        "Mux3 does not have 3 connections found %lld (source: \"%s\").",
                        conn_trees.size(), qPrintable(source_name));
                    break;
                }
                std::vector<QGraphicsPathItem *> connections;
                connections.reserve(conn_trees.size());
                std::transform(
                    conn_trees.begin(), conn_trees.end(), std::back_inserter(connections),
                    [](SvgDomTree<QGraphicsPathItem> &e) { return e.getElement(); });
                try {
                    const unsigned &source
                        = VALUE_SOURCE_NAME_MAPS.DEBUG_VAL.at(source_name)(core_state);
                    values.mux3_values.emplace_back(std::move(connections), source);
                } catch (std::out_of_range &e) {
                    WARN(
                        "Source for mux3 value not found (source: \"%s\").",
                        qPrintable(source_name));
                }
            }
            break;
        }
        case 'p': {
            if (component_name == PCValue::COMPONENT_NAME) {
                install_value(values.pc_values, VALUE_SOURCE_NAME_MAPS.PC, component, core_state);
            } else if (component_name == QStringLiteral("program-cache")) {
                if (machine->config().cache_program().enabled()) {
                    auto texts = component.findAll<SimpleTextItem>();
                    // Diagrams.net does not allow me, to put there some
                    // marks. :(
                    auto miss = texts.takeLast().getElement();
                    auto hit = texts.takeLast().getElement();
                    program_cache.reset(new Cache(machine->cache_program(), hit, miss));
                } else {
                    component.getElement()->hide();
                }
            }
            break;
        }
        case 'r': {
            if (component_name == RegValue::COMPONENT_NAME) {
                install_value(values.reg_values, VALUE_SOURCE_NAME_MAPS.REG, component, core_state);
            } else if (component_name == RegIdValue::COMPONENT_NAME) {
                install_value(
                    values.reg_id_values, VALUE_SOURCE_NAME_MAPS.REG_ID, component, core_state);
            }
            break;
        }
        }
    }

    if (machine->config().hazard_unit() == machine::MachineConfig::HU_NONE) {
        // Hazard unit conditional hide
        for (auto elem_tree : document.getRoot().findAll("data-tags", "hazardunit")) {
            elem_tree.getElement()->hide();
        }
    }

    update_values(); // Set to initial value - most often zero.

    // Update coreview with each core step.
    connect(machine->core(), &machine::Core::step_done, this, &CoreViewScene::update_values);
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
template<typename T_handler, typename T_lens, typename T>
void CoreViewScene::install_value(
    vector<T_handler> &handler_list,
    const unordered_map<QStringView, T_lens> &value_source_name_map,
    SvgDomTree<T> component,
    const CoreState &core_state) {
    SimpleTextItem *text_element = component.template find<SimpleTextItem>().getElement();
    const QString &source_name = component.getAttrValueOr("data-source");
    try {
        handler_list.emplace_back(text_element, value_source_name_map.at(source_name)(core_state));

        DEBUG(
            "Installing value %s with source %s.", qPrintable(T_handler::COMPONENT_NAME),
            qPrintable(source_name));
    } catch (std::out_of_range &) {
        WARN(
            "Source for %s value not found (source: \"%s\").", typeid(T).name(),
            qPrintable(source_name));
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
    update_value_list(values.mux2_values);
    update_value_list(values.mux3_values);
}

void CoreViewScene::request_jump_to_program_counter_wrapper() {
    emit request_jump_to_program_counter(program_counter_value);
}

CoreViewSceneSimple::CoreViewSceneSimple(machine::Machine *machine)
    : CoreViewScene(machine, "simple") {}

CoreViewScenePipelined::CoreViewScenePipelined(machine::Machine *machine)
    : CoreViewScene(
        machine,
        (machine->config().hazard_unit() == machine::MachineConfig::HU_STALL_FORWARD)
            ? "forwarding"
            : "pipeline") {}
