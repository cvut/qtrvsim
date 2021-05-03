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
        { u"alu-zero", dummy.b },         { u"decode-alu-src", dummy.b },
        { u"decode-branch", dummy.b },    { u"decode-mem-read", dummy.b },
        { u"decode-mem-read", dummy.b },  { u"decode-mem-to-reg", dummy.b },
        { u"decode-mem-write", dummy.b }, { u"decode-reg-write", dummy.b },
        { u"exec-alu-src", dummy.b },     { u"exec-branch", dummy.b },
        { u"exec-mem-read", dummy.b },    { u"exec-mem-to-reg", dummy.b },
        { u"exec-mem-write", dummy.b },   { u"exec-reg-write", dummy.b },
        { u"fetch-branch", dummy.b },     { u"mem-mem-read", dummy.b },
        { u"mem-mem-to-reg", dummy.b },   { u"mem-mem-write", dummy.b },
        { u"mem-reg-write", dummy.b },    { u"mem-to-reg", dummy.b },
        { u"wb-reg-write", dummy.b },
    };
    const unordered_map<QStringView, const machine::RegisterValue &> REG {
        { u"alu-res", dummy.reg },    { u"alu-src1", dummy.reg }, { u"alu-src2", dummy.reg },
        { u"decode-imm", dummy.reg }, { u"exec-imm", dummy.reg }, { u"fetched-value", dummy.reg },
        { u"mem-in", dummy.reg },     { u"mem-out", dummy.reg },  { u"rs1", dummy.reg },
        { u"rs2", dummy.reg },        { u"wb", dummy.reg },
    };
    const unordered_map<QStringView, const machine::RegisterId &> REG_ID {
        { u"decode-rd", dummy.regid }, { u"exec-rd", dummy.regid }, { u"mem-rd", dummy.regid },
        { u"rs1", dummy.regid },       { u"rs2", dummy.regid },     { u"wb-rd", dummy.regid },

    };
    const unordered_map<QStringView, const unsigned &> DEBUG {
        { u"cycle-count", dummy.d },    { u"decode-alu-op", dummy.d },  { u"exec-alu-op", dummy.d },
        { u"exec-forward-a", dummy.d }, { u"exec-forward-b", dummy.d }, { u"stall-count", dummy.d },
    };
    const unordered_map<QStringView, const machine::Address &> PC {
        { u"fetch-pc", dummy.pc },
    };
    const unordered_map<QStringView, MultiTextData> MULTI_TEXT {
        { u"hazard", { dummy.d, STALL_TEXT_TABLE } },
        { u"exception", { dummy.d, EXCEPTION_NAME_TABLE } },
    };
    const unordered_map<QStringView, InstructionData> INSTRUCTION {
        { u"fetch", { dummy.i, dummy.pc } }, { u"decode", { dummy.i, dummy.pc } },
        { u"exec", { dummy.i, dummy.pc } },  { u"mem", { dummy.i, dummy.pc } },
        { u"wb", { dummy.i, dummy.pc } },
    };
} VALUE_SOURCE_NAME_MAPS;

#endif // QTRVSIM_DATA_H
