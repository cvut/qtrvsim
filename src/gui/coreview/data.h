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

static const std::unordered_map<unsigned, QString> EXCEPTION_NAME_TABLE = {
    { machine::EXCAUSE_NONE, QStringLiteral("NONE") },
    { machine::EXCAUSE_INT, QStringLiteral("INT") },
    { machine::EXCAUSE_ADDRL, QStringLiteral("ADDRL") },
    { machine::EXCAUSE_ADDRS, QStringLiteral("ADDRS") },
    { machine::EXCAUSE_IBUS, QStringLiteral("IBUS") },
    { machine::EXCAUSE_DBUS, QStringLiteral("DBUS") },
    { machine::EXCAUSE_SYSCALL, QStringLiteral("SYSCALL") },
    { machine::EXCAUSE_BREAK, QStringLiteral("BREAK") },
    { machine::EXCAUSE_OVERFLOW, QStringLiteral("OVERFLOW") },
    { machine::EXCAUSE_TRAP, QStringLiteral("TRAP") },
    { machine::EXCAUSE_HWBREAK, QStringLiteral("HWBREAK") },
    { machine::EXCAUSE_UNKNOWN, QStringLiteral("UNKNOWN") },
};

static const std::unordered_map<unsigned, QString> STALL_TEXT_TABLE = {
    { 0, QStringLiteral("NORMAL") },
    { 1, QStringLiteral("STALL") },
    { 2, QStringLiteral("FORWARD") },
};

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

using MultiTextData = pair<const unsigned &, const std::unordered_map<unsigned, QString> &>;

/**
 * Maps SVG usable value names to lenses (lazy references) to fields, where thy
 * can be
 * retrieved.
 */
const struct {
    const unordered_map<QStringView, Lens<CoreState, bool>> BOOL {
        { QStringLiteral("decode-RegWrite"), LENS(CoreState, pipeline.decode.result.regwrite) },
        { QStringLiteral("decode-MemToReg"), LENS(CoreState, pipeline.decode.result.memread) },
        { QStringLiteral("decode-MemWrite"), LENS(CoreState, pipeline.decode.result.memwrite) },
        { QStringLiteral("decode-MemRead"), LENS(CoreState, pipeline.decode.result.memread) },
        { QStringLiteral("decode-BranchBxx"), LENS(CoreState, pipeline.decode.result.branch_bxx) },
        { QStringLiteral("decode-BranchJal"), LENS(CoreState, pipeline.decode.result.branch_jal) },
        { QStringLiteral("decode-BranchJalr"),
          LENS(CoreState, pipeline.decode.result.branch_jalr) },
        { QStringLiteral("decode-BranchVal"), LENS(CoreState, pipeline.decode.result.branch_val) },
        { QStringLiteral("decode-AluMul"), LENS(CoreState, pipeline.decode.internal.alu_mul) },
        { QStringLiteral("decode-AluSrc"), LENS(CoreState, pipeline.decode.result.alusrc) },
        { QStringLiteral("decode-AuiPC"), LENS(CoreState, pipeline.decode.result.alu_pc) },
        { QStringLiteral("exec-RegWrite"), LENS(CoreState, pipeline.execute.result.regwrite) },
        { QStringLiteral("exec-MemToReg"), LENS(CoreState, pipeline.execute.result.memread) },
        { QStringLiteral("exec-MemWrite"), LENS(CoreState, pipeline.execute.result.memwrite) },
        { QStringLiteral("exec-MemRead"), LENS(CoreState, pipeline.execute.result.memread) },
        { QStringLiteral("exec-BranchBxx"), LENS(CoreState, pipeline.execute.internal.branch_bxx) },
        { QStringLiteral("exec-BranchJal"), LENS(CoreState, pipeline.execute.result.branch_jal) },
        { QStringLiteral("exec-BranchJalr"), LENS(CoreState, pipeline.execute.result.branch_jalr) },
        { QStringLiteral("exec-BranchVal"), LENS(CoreState, pipeline.execute.result.branch_val) },
        { QStringLiteral("exec-AluMul"), LENS(CoreState, pipeline.execute.internal.alu_mul) },
        { QStringLiteral("exec-AluSrc"), LENS(CoreState, pipeline.execute.internal.alu_src) },
        { QStringLiteral("exec-AuiPC"), LENS(CoreState, pipeline.execute.internal.alu_pc) },
        { QStringLiteral("exec-AluZero"), LENS(CoreState, pipeline.execute.result.alu_zero) },
        { QStringLiteral("mem-RegWrite"), LENS(CoreState, pipeline.memory.result.regwrite) },
        { QStringLiteral("mem-MemToReg"), LENS(CoreState, pipeline.memory.result.memtoreg) },
        { QStringLiteral("mem-MemWrite"), LENS(CoreState, pipeline.memory.internal.memwrite) },
        { QStringLiteral("mem-MemRead"), LENS(CoreState, pipeline.memory.internal.memread) },
        { QStringLiteral("mem-BranchOutcome"),
          LENS(CoreState, pipeline.memory.internal.branch_outcome) },
        { QStringLiteral("mem-BranchJalx"), LENS(CoreState, pipeline.memory.internal.branch_jalx) },
        { QStringLiteral("mem-BranchJalr"), LENS(CoreState, pipeline.memory.internal.branch_jalr) },
        { QStringLiteral("wb-RegWrite"), LENS(CoreState, pipeline.writeback.internal.regwrite) },
        { QStringLiteral("wb-MemToReg"), LENS(CoreState, pipeline.writeback.internal.memtoreg) },
    };
    const unordered_map<QStringView, Lens<CoreState, RegisterValue>> REG {
        { QStringLiteral("alu-res"), LENS(CoreState, pipeline.execute.result.alu_val) },
        { QStringLiteral("alu-src1"), LENS(CoreState, pipeline.execute.internal.alu_src1) },
        { QStringLiteral("alu-src2"), LENS(CoreState, pipeline.execute.internal.alu_src2) },
        { QStringLiteral("decode-imm"), LENS(CoreState, pipeline.decode.result.immediate_val) },
        { QStringLiteral("exec-imm"), LENS(CoreState, pipeline.execute.result.immediate_val) },
        { QStringLiteral("decode-inst-bus"), LENS(CoreState, pipeline.decode.internal.inst_bus) },
        { QStringLiteral("mem-write-val"),
          LENS(CoreState, pipeline.memory.internal.mem_write_val) },
        { QStringLiteral("mem-write-addr"), LENS(CoreState, pipeline.memory.internal.mem_addr) },
        { QStringLiteral("mem-read-val"), LENS(CoreState, pipeline.memory.internal.mem_read_val) },
        { QStringLiteral("decode-rs1"), LENS(CoreState, pipeline.decode.result.val_rs) },
        { QStringLiteral("decode-rs2"), LENS(CoreState, pipeline.decode.result.val_rt) },
        { QStringLiteral("exec-rs1"), LENS(CoreState, pipeline.execute.internal.rs) },
        { QStringLiteral("exec-rs2"), LENS(CoreState, pipeline.execute.internal.rt) },
        { QStringLiteral("wb"), LENS(CoreState, pipeline.writeback.internal.value) },
    };
    const unordered_map<QStringView, Lens<CoreState, machine::RegisterId>> REG_ID {
        { QStringLiteral("decode-rd"), LENS(CoreState, pipeline.decode.result.num_rd) },
        { QStringLiteral("exec-rd"), LENS(CoreState, pipeline.execute.result.num_rd) },
        { QStringLiteral("mem-rd"), LENS(CoreState, pipeline.memory.result.num_rd) },
        { QStringLiteral("wb-rd"), LENS(CoreState, pipeline.writeback.internal.num_rd) },
        { QStringLiteral("rs1"), LENS(CoreState, pipeline.decode.result.num_rs) },
        { QStringLiteral("rs2"), LENS(CoreState, pipeline.decode.result.num_rt) },
    };
    const unordered_map<QStringView, Lens<CoreState, unsigned>> DEBUG_VAL {
        { QStringLiteral("CycleCount"), LENS(CoreState, cycle_count) },
        { QStringLiteral("StallCount"), LENS(CoreState, stall_count) },
        { QStringLiteral("decode-AluControl"),
          LENS(CoreState, pipeline.decode.internal.alu_op_num) },
        { QStringLiteral("exec-AluControl"),
          LENS(CoreState, pipeline.execute.internal.alu_op_num) },
        { QStringLiteral("exec-ForwardA"),
          LENS(CoreState, pipeline.execute.internal.forward_from_rs1_num) },
        { QStringLiteral("exec-ForwardB"),
          LENS(CoreState, pipeline.execute.internal.forward_from_rs2_num) },
    };
    const unordered_map<QStringView, Lens<CoreState, Address>> PC {
        { QStringLiteral("fetch-pc"), LENS(CoreState, pipeline.fetch.result.inst_addr) },
    };

#define MULTITEXT_LENS(INDEX, TABLE)                                                               \
    [](const CoreState &base) -> MultiTextData { return { base.INDEX, TABLE }; }

    const unordered_map<
        QStringView,
        LensPair<CoreState, unsigned, const std::unordered_map<unsigned, QString>>>
        MULTI_TEXT {
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
