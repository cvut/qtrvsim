#ifndef QTRVSIM_SCENE_H
#define QTRVSIM_SCENE_H

#include "./components/cache.h"
#include "./components/value_handlers.h"
#include "graphicsview.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSignalMapper>
#include <machine/machine.h>
#include <svgscene/components/hyperlinkitem.h>
#include <svgscene/components/simpletextitem.h>
#include <svgscene/svggraphicsscene.h>
#include <unordered_map>

class CoreViewScene : public svgscene::SvgGraphicsScene {
    Q_OBJECT

public:
    CoreViewScene(machine::Machine *machine, const QString &core_svg_scheme_name);
    ~CoreViewScene() override;

signals:
    /* Hyperlink handlers propagated to main window. */
    void request_registers();
    void request_data_memory();
    void request_program_memory();
    void request_jump_to_program_counter(machine::Address addr);
    void request_cache_program();
    void request_cache_data();
    void request_peripherals();
    void request_terminal();

public slots:
    /**
     * Update all installed dynamic values.
     *
     * @see install_value
     */
    void update_values();

protected:
    /**
     * Lookup link target and connect element one of `request_` slots.
     * @param element   the clickable element
     */
    void install_hyperlink(svgscene::HyperlinkItem *element) const;

    /**
     * Create update handler for dynamic value in core view.
     *
     * @tparam T_handler              one of classes in
     *                                `coreview/components/numeric_value.h`
     * @tparam T                      type of value to update from
     * @param handler_list            list, where the created handler will be
     *                                stored
     * @param value_source_name_map   maps of map names used in svg to
     *                                references in the state struct
     * @param element                 text element that will be updated
     * @param source_name             name of data source, see
     *                                value_source_name_map
     */
    template<typename T_handler, typename T>
    void install_value(
        std::vector<T_handler> &handler_list,
        const std::unordered_map<QStringView, T> &value_source_name_map,
        svgscene::SimpleTextItem *element,
        const QString &source_name);

    /**
     * Wrapper for `install_value` which searched all value components of
     * the given type in the SVG document.
     *
     * @tparam T_handler            see install_value
     * @tparam T                    see install_value
     * @param document              SVG document
     * @param handler_list          see install_value
     * @param value_source_name_map see install_value
     */
    template<typename T_handler, typename T>
    void install_values_from_document(
        const svgscene::SvgDocument &document,
        std::vector<T_handler> &handler_list,
        const std::unordered_map<QStringView, T> &value_source_name_map);

    /**
     * Update all value components of given type.
     *
     * @tparam T          type of component
     * @param value_list  list of components
     * @see               install_value
     */
    template<typename T>
    void update_value_list(std::vector<T> &value_list);

protected:
    /**
     * Lists of dynamic values from svg that should updated each cycle.
     */
    struct {
        std::vector<BoolValue> bool_values;
        std::vector<RegValue> reg_values;
        std::vector<RegIdValue> reg_id_values;
        std::vector<DebugValue> debug_values;
        std::vector<PCValue> pc_values;
        std::vector<MultiTextValue> multi_text_values;
        std::vector<InstructionValue> instruction_values;
    } values;

    Box<Cache> program_cache;
    Box<Cache> data_cache;
};

class CoreViewSceneSimple : public CoreViewScene {
public:
    explicit CoreViewSceneSimple(machine::Machine *machine);
};

class CoreViewScenePipelined : public CoreViewScene {
public:
    explicit CoreViewScenePipelined(machine::Machine *machine);
};

#endif
