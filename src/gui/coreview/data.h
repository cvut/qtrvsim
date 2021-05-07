#ifndef QTRVSIM_DATA_H
#define QTRVSIM_DATA_H

#include "common/polyfills/qstring_hash.h"

using std::pair;
using std::unordered_map;
using std::vector;
class CoreViewScene;

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
 * Maps SVG usable value names to references to fields, where thy can be
 * retrieved.
 */
static const struct {
    const unordered_map<QStringView, const bool &> BOOL {
        { QStringLiteral("alu-zero"), dummy.b },
        { QStringLiteral("decode-alu-src"), dummy.b },
        { QStringLiteral("decode-branch"), dummy.b },
        { QStringLiteral("decode-mem-read"), dummy.b },
        { QStringLiteral("decode-mem-read"), dummy.b },
        { QStringLiteral("decode-mem-to-reg"), dummy.b },
        { QStringLiteral("decode-mem-write"), dummy.b },
        { QStringLiteral("decode-reg-write"), dummy.b },
        { QStringLiteral("exec-alu-src"), dummy.b },
        { QStringLiteral("exec-branch"), dummy.b },
        { QStringLiteral("exec-mem-read"), dummy.b },
        { QStringLiteral("exec-mem-to-reg"), dummy.b },
        { QStringLiteral("exec-mem-write"), dummy.b },
        { QStringLiteral("exec-reg-write"), dummy.b },
        { QStringLiteral("fetch-branch"), dummy.b },
        { QStringLiteral("mem-mem-read"), dummy.b },
        { QStringLiteral("mem-mem-to-reg"), dummy.b },
        { QStringLiteral("mem-mem-write"), dummy.b },
        { QStringLiteral("mem-reg-write"), dummy.b },
        { QStringLiteral("mem-to-reg"), dummy.b },
        { QStringLiteral("wb-reg-write"), dummy.b },
    };
    const unordered_map<QStringView, const machine::RegisterValue &> REG {
        { QStringLiteral("alu-res"), dummy.reg },  { QStringLiteral("alu-src1"), dummy.reg },
        { QStringLiteral("alu-src2"), dummy.reg }, { QStringLiteral("decode-imm"), dummy.reg },
        { QStringLiteral("exec-imm"), dummy.reg }, { QStringLiteral("fetched-value"), dummy.reg },
        { QStringLiteral("mem-in"), dummy.reg },   { QStringLiteral("mem-out"), dummy.reg },
        { QStringLiteral("rs1"), dummy.reg },      { QStringLiteral("rs2"), dummy.reg },
        { QStringLiteral("wb"), dummy.reg },
    };
    const unordered_map<QStringView, const machine::RegisterId &> REG_ID {
        { QStringLiteral("decode-rd"), dummy.regid }, { QStringLiteral("exec-rd"), dummy.regid },
        { QStringLiteral("mem-rd"), dummy.regid },    { QStringLiteral("rs1"), dummy.regid },
        { QStringLiteral("rs2"), dummy.regid },       { QStringLiteral("wb-rd"), dummy.regid },

    };
    const unordered_map<QStringView, const unsigned &> DEBUG {
        { QStringLiteral("cycle-count"), dummy.d },
        { QStringLiteral("decode-alu-op"), dummy.d },
        { QStringLiteral("exec-alu-op"), dummy.d },
        { QStringLiteral("exec-forward-a"), dummy.d },
        { QStringLiteral("exec-forward-b"), dummy.d },
        { QStringLiteral("stall-count"), dummy.d },
    };
    const unordered_map<QStringView, const machine::Address &> PC {
        { QStringLiteral("fetch-pc"), dummy.pc },
    };
    const unordered_map<QStringView, MultiTextData> MULTI_TEXT {
        { QStringLiteral("hazard"), { dummy.d, STALL_TEXT_TABLE } },
        { QStringLiteral("exception"), { dummy.d, EXCEPTION_NAME_TABLE } },
    };
    const unordered_map<QStringView, InstructionData> INSTRUCTION {
        { QStringLiteral("fetch"), { dummy.i, dummy.pc } },
        { QStringLiteral("decode"), { dummy.i, dummy.pc } },
        { QStringLiteral("exec"), { dummy.i, dummy.pc } },
        { QStringLiteral("mem"), { dummy.i, dummy.pc } },
        { QStringLiteral("wb"), { dummy.i, dummy.pc } },
    };
} VALUE_SOURCE_NAME_MAPS;

#endif // QTRVSIM_DATA_H
