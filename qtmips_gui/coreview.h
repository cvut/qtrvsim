// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

#ifndef COREVIEW_H
    #define COREVIEW_H

    #include "coreview/adder.h"
    #include "coreview/alu.h"
    #include "coreview/and.h"
    #include "coreview/connection.h"
    #include "coreview/constant.h"
    #include "coreview/instructionview.h"
    #include "coreview/junction.h"
    #include "coreview/latch.h"
    #include "coreview/logicblock.h"
    #include "coreview/memory.h"
    #include "coreview/minimux.h"
    #include "coreview/multiplexer.h"
    #include "coreview/multitext.h"
    #include "coreview/programcounter.h"
    #include "coreview/registers.h"
    #include "coreview/value.h"
    #include "graphicsview.h"
    #include "qtmips_machine/qtmipsmachine.h"

    #include <QGraphicsScene>
    #include <QGraphicsView>

class CoreViewScene : public QGraphicsScene {
    Q_OBJECT
public:
    CoreViewScene(machine::QtMipsMachine *machine);
    ~CoreViewScene() override;

signals:
    void request_registers();
    void request_data_memory();
    void request_program_memory();
    void request_jump_to_program_counter(uint32_t addr);
    void request_cache_program();
    void request_cache_data();
    void request_peripherals();
    void request_terminal();

protected:
    coreview::ProgramMemory *mem_program;
    coreview::DataMemory *mem_data;
    coreview::Registers *regs;
    coreview::Alu *alu;
    coreview::LogicBlock *peripherals;
    coreview::LogicBlock *terminal;
    struct {
        coreview::ProgramCounter *pc;
        coreview::Latch *latch;
        coreview::Adder *adder;
        coreview::Constant *adder_4;
        coreview::Junction *junc_pc, *junc_pc_4;
        coreview::Multiplexer *multiplex;
    } ft {};
    struct {
        coreview::LogicBlock *ctl_block, *sign_ext, *shift2, *cmp;
        coreview::Adder *add;
        coreview::Junction *j_sign_ext;
        coreview::And *and_branch;
        coreview::Junction *j_inst_up, *j_inst_down;
        coreview::Junction *j_jalpctor31, *j_jump_reg;
        coreview::Bus *instr_bus;
    } dc {};
    struct {
        coreview::Junction *j_mux;
        coreview::Junction *j_rs_num;
        coreview::Multiplexer *mux_imm, *mux_regdest;
    } ex {};
    struct {
        coreview::Junction *j_addr;
        coreview::MultiText *multi_excause;
    } mm {};
    struct {
        coreview::Multiplexer *mem_or_reg;
        coreview::Junction *j_reg_write_val;
    } wb {};
    struct {
        coreview::Multiplexer *mux_alu_reg_a;
        coreview::Multiplexer *mux_alu_reg_b;
        coreview::Junction *j_alu_out;
        coreview::MultiText *multi_stall;
        coreview::MiniMux *mux_branch_reg_a;
        coreview::MiniMux *mux_branch_reg_b;
    } hu {};

    coreview::Connection *
    new_connection(const coreview::Connector *, const coreview::Connector *);
    coreview::Bus *new_bus(
        const coreview::Connector *,
        const coreview::Connector *,
        unsigned width = 4);
    coreview::Signal *
    new_signal(const coreview::Connector *, const coreview::Connector *);
    QGraphicsSimpleTextItem *new_label(const QString &str, qreal x, qreal y);
    coreview::Connection *pc2pc_latch {};
    coreview::Connection *pc_latch2pc_joint {}, *pc_joint2pc_adder {},
        *pc_joint2mem {};
    coreview::Connection *pc_multiplexer2pc {};
};

class CoreViewSceneSimple : public CoreViewScene {
public:
    CoreViewSceneSimple(machine::QtMipsMachine *machine);

private:
    coreview::InstructionView *inst_prim, *inst_fetch;
    coreview::Latch *latch_if_id;
};

class CoreViewScenePipelined : public CoreViewScene {
public:
    CoreViewScenePipelined(machine::QtMipsMachine *machine);

private:
    coreview::Latch *latch_if_id, *latch_id_ex, *latch_ex_mem, *latch_mem_wb;
    coreview::InstructionView *inst_fetch, *inst_dec, *inst_exec, *inst_mem,
        *inst_wrb;
    coreview::LogicBlock *hazard_unit;
};

#else

class CoreViewScene;
class CoreViewSceneSimple;
class CoreViewScenePipelined;

#endif // COREVIEW_H
