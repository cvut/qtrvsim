#ifndef COREVIEW_H
    #define COREVIEW_H

    #include "coreview/instructionview.h"
    #include "coreview/logicblock.h"
    #include "coreview/memory.h"
    #include "coreview/multitext.h"
    #include "coreview/programcounter.h"
    #include "coreview/registers.h"
    #include "coreview/value.h"
    #include "graphicsview.h"

    #include <QGraphicsScene>
    #include <QGraphicsView>
    #include <QSignalMapper>
    #include <machine/machine.h>
    #include <svgscene/components/hyperlinkitem.h>
    #include <svgscene/svggraphicsscene.h>

class CoreViewScene : public svgscene::SvgGraphicsScene {
    Q_OBJECT

public:
    CoreViewScene(machine::Machine *machine, const QString &background_name);
    ~CoreViewScene() override;

signals:
    void request_registers();
    void request_data_memory();
    void request_program_memory();
    void request_jump_to_program_counter(machine::Address addr);
    void request_cache_program();
    void request_cache_data();
    void request_peripherals();
    void request_terminal();

protected:
    void register_hyperlink(svgscene::HyperlinkItem *element) const;

protected:
    coreview::ProgramMemory *mem_program;
    coreview::DataMemory *mem_data;
    coreview::Registers *regs;
    coreview::LogicBlock *peripherals;
    coreview::LogicBlock *terminal;
    struct {
        coreview::ProgramCounter *pc;
    } ft {};
    struct {
        coreview::MultiText *multi_excause;
    } mm {};
    struct {
        coreview::MultiText *multi_stall;
    } hu {};

    QGraphicsSimpleTextItem *new_label(const QString &str, qreal x, qreal y);
};

class CoreViewSceneSimple : public CoreViewScene {
public:
    explicit CoreViewSceneSimple(machine::Machine *machine);

private:
    coreview::InstructionView *inst_prim;
};

class CoreViewScenePipelined : public CoreViewScene {
public:
    explicit CoreViewScenePipelined(machine::Machine *machine);

private:
    coreview::InstructionView *inst_fetch, *inst_dec, *inst_exec, *inst_mem,
        *inst_wrb;
    coreview::LogicBlock *hazard_unit;
};

#else

class CoreViewScene;
class CoreViewSceneSimple;
class CoreViewScenePipelined;

#endif // COREVIEW_H
