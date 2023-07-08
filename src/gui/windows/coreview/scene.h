#ifndef QTRVSIM_SCENE_H
#define QTRVSIM_SCENE_H

#include "./components/cache.h"
#include "./components/value_handlers.h"
#include "common/polyfills/qstring_hash.h"
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

    /** Hyperlink handler which automatically uses current PC value via this object */
    void request_jump_to_program_counter_wrapper();

signals:
    /* Hyperlink handlers propagated to the main window. */
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
     * @param value_source_name_map   maps of load_sections_indexes names used in svg to
     *                                references in the state struct
     * @param component               text element that will be updated
     * @param source_name             name of data source, see
     *                                value_source_name_map
     */
    template<typename T_handler, typename T_lens, typename T>
    void install_value(
        std::vector<T_handler> &handler_list,
        const std::unordered_map<QStringView, T_lens> &value_source_name_map,
        svgscene::SvgDomTree<T> component,
        const machine::CoreState &core_state);

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
        std::vector<Multiplexer<bool>> mux2_values;
        std::vector<Multiplexer<unsigned>> mux3_values;
    } values;

    Box<Cache> program_cache;
    Box<Cache> data_cache;

    /** Reference to current PC value to be used to focus PC in program memory on lick */
    const machine::Address& program_counter_value;
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
