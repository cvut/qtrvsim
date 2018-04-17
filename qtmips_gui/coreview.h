#ifndef COREVIEW_H
#define COREVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include "qtmipsmachine.h"
#include "graphicsview.h"
#include "coreview/connection.h"
#include "coreview/programcounter.h"
#include "coreview/multiplexer.h"
#include "coreview/latch.h"
#include "coreview/alu.h"
#include "coreview/adder.h"
#include "coreview/memory.h"
#include "coreview/registers.h"
#include "coreview/instructionview.h"
#include "coreview/junction.h"
#include "coreview/constant.h"
#include "coreview/logicblock.h"
#include "coreview/and.h"

class CoreViewScene : public QGraphicsScene {
    Q_OBJECT
public:
    CoreViewScene(machine::QtMipsMachine *machine);
    ~CoreViewScene();

signals:
    void request_registers();
    void request_data_memory();
    void request_program_memory();
    void request_jump_to_program_counter(std::uint32_t addr);
    void request_cache_program();
    void request_cache_data();

protected:
    coreview::ProgramMemory *mem_program;
    coreview::DataMemory *mem_data;
    coreview::Registers *regs;
    coreview::Alu *alu;
    struct {
        coreview::ProgramCounter *pc;
        coreview::Latch *latch;
        coreview::Adder *adder;
        coreview::Constant *adder_4;
        coreview::Junction *junc_pc, *junc_pc_4;
        coreview::Multiplexer *multiplex;
    } ft;
    struct {
        coreview::LogicBlock *ctl_block, *sign_ext, *shift2, *cmp;
        coreview::Adder *add;
        coreview::Junction *j_sign_ext;
        coreview::And *and_branch;
        coreview::Junction *j_inst_up, *j_inst_down;
        coreview::Bus *instr_bus;
    } dc;
    struct {
        coreview::Junction *j_mux;
        coreview::Multiplexer *mux_imm, *mux_regdest;
    } ex;
    struct {
        coreview::Junction *j_addr;
    } mm;
    struct {
        coreview::Multiplexer *mem_or_reg;
    } wb;

    coreview::Connection *new_connection(const coreview::Connector*, const coreview::Connector*);
    coreview::Bus *new_bus(const coreview::Connector*, const coreview::Connector*, unsigned width = 4);
    coreview::Signal *new_signal(const coreview::Connector*, const coreview::Connector*);
    coreview::Connection *pc2pc_latch;
    coreview::Connection *pc_latch2pc_joint, *pc_joint2pc_adder, *pc_joint2mem;
    coreview::Connection *pc_multiplexer2pc;
};

class CoreViewSceneSimple : public CoreViewScene {
public:
    CoreViewSceneSimple(machine::QtMipsMachine *machine);

private:
    coreview::InstructionView *instr_prim, *instr_delay;
    coreview::Latch *delay_slot_latch;
};

class CoreViewScenePipelined : public CoreViewScene {
public:
    CoreViewScenePipelined(machine::QtMipsMachine *machine);

private:
    coreview::Latch *latch_if_id, *latch_id_ex, *latch_ex_mem, *latch_mem_wb;
    coreview::InstructionView *inst_fetch, *inst_dec, *inst_exec, *inst_mem, *inst_wrb;
    coreview::LogicBlock *hazard_unit;
};

#else

class CoreViewScene;
class CoreViewSceneSimple;
class CoreViewScenePipelined;

#endif // COREVIEW_H
