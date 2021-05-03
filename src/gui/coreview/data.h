#ifndef QTRVSIM_DATA_H
#define QTRVSIM_DATA_H

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

/**
 * Maps SVG usable value names to references to fields, where thy can be
 * retrieved.
 */
static const struct {
    const unordered_map<QStringView, const bool &> BOOL {};
    const unordered_map<QStringView, const machine::RegisterValue &> REG {};
    const unordered_map<QStringView, const unsigned &> DEBUG {};
    const unordered_map<QStringView, const machine::Address &> PC {};
    const unordered_map<QStringView, MultiTextData> MULTI_TEXT {};
    const unordered_map<QStringView, InstructionData> INSTRUCTION {};
} VALUE_SOURCE_NAME_MAPS;

#endif // QTRVSIM_DATA_H
