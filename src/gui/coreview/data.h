/**
 * This file contains maps with data for core visualization.
 * - Tables of strings.
 * - Maps of string hook used in SVG files onto lenses into the core state
 *   struct. (see `common/type_utils/lens`)
 *
 * @file
 */
#ifndef QTRVSIM_DATA_H
#define QTRVSIM_DATA_H

#include "common/type_utils/lens.h"

#include <machine/core/core_state.h>

using std::pair;
using std::unordered_map;
using std::vector;
class CoreViewScene;
using machine::Address;
using machine::CoreState;
using machine::Instruction;
using machine::RegisterId;
using machine::RegisterValue;

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
static const unordered_map<QString, void (::CoreViewScene::*)()> HYPERLINK_TARGETS {
    { "#registers", &CoreViewScene::request_registers },
    { "#cache_data", &CoreViewScene::request_cache_data },
    { "#cache_program", &CoreViewScene::request_cache_program },
    { "#data_memory", &CoreViewScene::request_data_memory },
    { "#peripherals", &CoreViewScene::request_peripherals },
    { "#program_memory", &CoreViewScene::request_program_memory },
    { "#terminal", &CoreViewScene::request_terminal },
};

using MultiTextData = pair<const unsigned &, const std::vector<QString> &>;
using InstructionData = pair<const machine::Instruction &, const machine::Address &>;

struct {
    machine::Address pc { 0xABCDEF };
    machine::RegisterValue reg { 0xAAFFAAFF };
    machine::RegisterId regid { 12 };
    bool b = true;
    unsigned d { 2 };
    machine::Instruction i { 0X8C022000 };
} dummy;

/**
 * Maps SVG usable value names to lenses (lazy references) to fields, where thy
 * can be
 * retrieved.
 */
const struct {
    const unordered_map<QStringView, Lens<CoreState, bool>> BOOL {
        { QStringLiteral("alu-zero"), LENS(CoreState, pipeline.execute.internal.alu_zero) },
        { QStringLiteral("decode-alu-src"), LENS(CoreState, pipeline.decode.result.alusrc) },
        { QStringLiteral("decode-branch"), LENS(CoreState, pipeline.decode.result.branch) },
        { QStringLiteral("decode-mem-read"), LENS(CoreState, pipeline.decode.result.memread) },
        { QStringLiteral("decode-mem-to-reg"), LENS(CoreState, pipeline.decode.result.memwrite) },
        { QStringLiteral("decode-mem-write"), LENS(CoreState, pipeline.decode.result.memwrite) },
        { QStringLiteral("decode-reg-write"), LENS(CoreState, pipeline.decode.result.regwrite) },
        { QStringLiteral("exec-alu-src"), LENS(CoreState, pipeline.execute.internal.alu_src) },
        { QStringLiteral("exec-branch"), LENS(CoreState, pipeline.execute.internal.branch) },
        { QStringLiteral("exec-mem-read"), LENS(CoreState, pipeline.execute.result.memread) },
        { QStringLiteral("exec-mem-to-reg"), LENS(CoreState, pipeline.execute.result.memread) },
        { QStringLiteral("exec-mem-write"), LENS(CoreState, pipeline.execute.result.memwrite) },
        { QStringLiteral("exec-reg-write"), LENS(CoreState, pipeline.execute.result.regwrite) },
        // TODO: NOT AVAILABLE IN MIPS CORE
        //        { QStringLiteral("exec-branch"),
        //          LENS(CoreState, pipeline.execute.inside.) },
        { QStringLiteral("mem-mem-read"), LENS(CoreState, pipeline.memory.internal.memread) },
        { QStringLiteral("mem-mem-to-reg"), LENS(CoreState, pipeline.memory.result.memtoreg) },
        { QStringLiteral("mem-mem-write"), LENS(CoreState, pipeline.memory.internal.memwrite) },
        { QStringLiteral("mem-reg-write"), LENS(CoreState, pipeline.memory.result.regwrite) },
        { QStringLiteral("mem-to-reg"), LENS(CoreState, pipeline.memory.result.memtoreg) },
        { QStringLiteral("wb-reg-write"), LENS(CoreState, pipeline.writeback.internal.regwrite) },
    };
    const unordered_map<QStringView, Lens<CoreState, RegisterValue>> REG {
        { QStringLiteral("alu-res"), LENS(CoreState, pipeline.execute.result.alu_val) },
        { QStringLiteral("alu-src1"), LENS(CoreState, pipeline.execute.internal.alu_src1) },
        { QStringLiteral("alu-src2"), LENS(CoreState, pipeline.execute.internal.alu_src2) },
        { QStringLiteral("decode-imm"), LENS(CoreState, pipeline.decode.result.immediate_val) },
        { QStringLiteral("exec-imm"), LENS(CoreState, pipeline.execute.internal.immediate) },
        { QStringLiteral("fetched-value"), LENS(CoreState, pipeline.fetch.internal.fetched_value) },
        { QStringLiteral("mem-write-val"),
          LENS(CoreState, pipeline.memory.internal.mem_write_val) },
        { QStringLiteral("mem-read-val"), LENS(CoreState, pipeline.memory.internal.mem_read_val) },
        { QStringLiteral("decode-rs1"), LENS(CoreState, pipeline.decode.result.val_rs) },
        { QStringLiteral("decode-rs2"), LENS(CoreState, pipeline.decode.result.val_rt) },
        { QStringLiteral("exec-rs1"), LENS(CoreState, pipeline.execute.internal.rs) },
        { QStringLiteral("exec-rs2"), LENS(CoreState, pipeline.execute.internal.rt) },
        { QStringLiteral("wb"), LENS(CoreState, pipeline.memory.result.towrite_val) },
    };
    const unordered_map<QStringView, Lens<CoreState, uint8_t>> REG_ID {
        { QStringLiteral("decode-rd"), LENS(CoreState, pipeline.decode.result.num_rd) },
        { QStringLiteral("exec-rd"), LENS(CoreState, pipeline.execute.result.num_rd) },
        { QStringLiteral("mem-rd"), LENS(CoreState, pipeline.memory.result.num_rd) },
        { QStringLiteral("wb-rd"), LENS(CoreState, pipeline.writeback.internal.num_rd) },
        { QStringLiteral("rs1"), LENS(CoreState, pipeline.decode.result.num_rs) },
        { QStringLiteral("rs2"), LENS(CoreState, pipeline.decode.result.num_rt) },
    };
    const unordered_map<QStringView, Lens<CoreState, unsigned>> DEBUG_VAL {
        { QStringLiteral("cycle-count"), LENS(CoreState, cycle_count) },
        { QStringLiteral("stall-count"), LENS(CoreState, stall_count) },
        { QStringLiteral("decode-alu-op"), LENS(CoreState, pipeline.decode.internal.alu_op_num) },
        { QStringLiteral("exec-alu-op"), LENS(CoreState, pipeline.execute.internal.alu_op_num) },
        { QStringLiteral("exec-forward-a"),
          LENS(CoreState, pipeline.execute.internal.forward_from_rs1_num) },
        { QStringLiteral("exec-forward-b"),
          LENS(CoreState, pipeline.execute.internal.forward_from_rs2_num) },
    };
    const unordered_map<QStringView, Lens<CoreState, Address>> PC {
        { QStringLiteral("fetch-pc"), LENS(CoreState, pipeline.fetch.result.inst_addr) },
    };

#define MULTITEXT_LENS(INDEX, TABLE)                                                               \
    [](const CoreState &base) -> std::pair<const unsigned &, const vector<QString> &> {            \
        return { base.INDEX, TABLE };                                                              \
    }

    const unordered_map<QStringView, LensPair<CoreState, unsigned, vector<QString>>> MULTI_TEXT {
        { QStringLiteral("fetch-exception"),
          MULTITEXT_LENS(pipeline.fetch.internal.excause_num, EXCEPTION_NAME_TABLE) },
        { QStringLiteral("decode-exception"),
          MULTITEXT_LENS(pipeline.decode.internal.excause_num, EXCEPTION_NAME_TABLE) },
        { QStringLiteral("execute-exception"),
          MULTITEXT_LENS(pipeline.execute.internal.excause_num, EXCEPTION_NAME_TABLE) },
        { QStringLiteral("memory-exception"),
          MULTITEXT_LENS(pipeline.memory.internal.excause_num, EXCEPTION_NAME_TABLE) },
        { QStringLiteral("hazard"),
          MULTITEXT_LENS(pipeline.execute.internal.stall_status, STALL_TEXT_TABLE) },
    };

    const unordered_map<QStringView, LensPair<CoreState, Instruction, Address>> INSTRUCTION {
        { QStringLiteral("fetch"),
          LENS_PAIR(CoreState, pipeline.fetch.result.inst, pipeline.fetch.result.inst_addr) },
        { QStringLiteral("decode"),
          LENS_PAIR(CoreState, pipeline.decode.result.inst, pipeline.decode.result.inst_addr) },
        { QStringLiteral("exec"),
          LENS_PAIR(CoreState, pipeline.execute.result.inst, pipeline.execute.result.inst_addr) },
        { QStringLiteral("mem"),
          LENS_PAIR(CoreState, pipeline.memory.result.inst, pipeline.memory.result.inst_addr) },
        { QStringLiteral("wb"),
          { LENS_PAIR(
              CoreState,
              pipeline.writeback.internal.inst,
              pipeline.writeback.internal.inst_addr) } },
    };
} VALUE_SOURCE_NAME_MAPS;

#endif // QTRVSIM_DATA_H
