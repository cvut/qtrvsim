#ifndef CORE_H
#define CORE_H

#include "common/memory_ownership.h"
#include "core/core_state.h"
#include "csr/controlstate.h"
#include "instruction.h"
#include "machineconfig.h"
#include "memory/address.h"
#include "memory/frontend_memory.h"
#include "pipeline.h"
#include "predictor.h"
#include "register_value.h"
#include "registers.h"
#include "simulator_exception.h"

#include <QObject>

namespace machine {

using std::array;

class ExceptionHandler;
class StopExceptionHandler;
struct hwBreak;

class Core : public QObject {
    Q_OBJECT
public:
    Core(
        Registers *regs,
        BranchPredictor *predictor,
        FrontendMemory *mem_program,
        FrontendMemory *mem_data,
        CSR::ControlState *control_state,
        Xlen xlen,
        ConfigIsaWord isa_word);

    void step(bool skip_break = false);
    void reset(); // Reset core (only core, memory and registers has to be reset separately).

    unsigned get_cycle_count() const;
    unsigned get_stall_count() const;

    Registers *get_regs() const;
    CSR::ControlState *get_control_state() const;
    BranchPredictor *get_predictor() const;
    FrontendMemory *get_mem_data() const;
    FrontendMemory *get_mem_program() const;
    const CoreState &get_state() const;
    Xlen get_xlen() const;

    void insert_hwbreak(Address address);
    void remove_hwbreak(Address address);
    bool is_hwbreak(Address address) const;
    void register_exception_handler(ExceptionCause excause, ExceptionHandler *exhandler);
    void set_stop_on_exception(enum ExceptionCause excause, bool value);
    bool get_stop_on_exception(enum ExceptionCause excause) const;
    void set_step_over_exception(enum ExceptionCause excause, bool value);
    bool get_step_over_exception(enum ExceptionCause excause) const;

    /**
     * Abstracts XLEN from code flow. XLEN core will obtain XLEN value from register value.
     * The value will be zero extended to u64.
     */
    uint64_t get_xlen_from_reg(RegisterValue reg) const;

protected:
    CoreState state {};

    /**
     * Shortcuts to interstage registers
     * Interstage registers are stored in the core state struct in 2 copies. One (result) is the
     * state after combinatorial logic of each stage has been applied. This is used to visualize
     * the internal state of a stage. The should be modified ONLY by the stage logic functions. The
     * other (final) is the one actually written to HW interstage register. Any operation within
     * core should happen on the final registers.
     *
     * The bellow references provide shortcuts to the final interstage registers.
     */

    /** Reference to pseudo interstage register PC/IF inside core state. */
    PCInterstage &pc_if;
    /** Reference to interstage register IF/ID inside core state. */
    FetchInterstage &if_id;
    /** Reference to interstage register ID/EX inside core state. */
    DecodeInterstage &id_ex;
    /** Reference to interstage register EX/MEM inside core state. */
    ExecuteInterstage &ex_mem;
    /** Reference to interstage register MEM/WB inside core state. */
    MemoryInterstage &mem_wb;

signals:
    void stop_on_exception_reached();
    void step_started();
    void step_done(const CoreState &);

protected:
    virtual void do_step(bool skip_break) = 0;
    virtual void do_reset() = 0;

    bool handle_exception(
        ExceptionCause excause,
        const Instruction &inst,
        Address inst_addr,
        Address next_addr,
        Address jump_branch_pc,
        Address mem_ref_addr);

    const Xlen xlen;
    const InstructionFlags check_inst_flags_val;
    const InstructionFlags check_inst_flags_mask;
    BORROWED Registers *const regs;
    BORROWED CSR::ControlState *const control_state;
    BORROWED BranchPredictor *const predictor;
    BORROWED FrontendMemory *const mem_data, *const mem_program;

    array<bool, EXCAUSE_COUNT> stop_on_exception {};
    array<bool, EXCAUSE_COUNT> step_over_exception {};
    QMap<Address, OWNED hwBreak *> hw_breaks {};
    QMap<ExceptionCause, OWNED ExceptionHandler *> ex_handlers;
    Box<ExceptionHandler> ex_default_handler;

    FetchState fetch(PCInterstage pc, bool skip_break);
    DecodeState decode(const FetchInterstage &);
    static ExecuteState execute(const DecodeInterstage &);
    MemoryState memory(const ExecuteInterstage &);
    WritebackState writeback(const MemoryInterstage &);

    /**
     * This function computes the address, the next executed instruction should be on. The word
     * `computed` is used in contrast with predicted value by the branch predictor.
     */
    Address compute_next_inst_addr(const ExecuteInterstage &exec, bool branch_taken) const;

    enum ExceptionCause memory_special(
        enum AccessControl memctl,
        int mode,
        bool memread,
        bool memwrite,
        RegisterValue &towrite_val,
        RegisterValue rt_value,
        Address mem_addr);
};

class CoreSingle : public Core {
public:
    CoreSingle(
        Registers *regs,
        BranchPredictor *predictor,
        FrontendMemory *mem_program,
        FrontendMemory *mem_data,
        CSR::ControlState *control_state,
        Xlen xlen,
        ConfigIsaWord isa_word);

protected:
    void do_step(bool skip_break) override;
    void do_reset() override;

private:
    Address prev_inst_addr {};
};

class CorePipelined : public Core {
public:
    CorePipelined(
        Registers *regs,
        BranchPredictor *predictor,
        FrontendMemory *mem_program,
        FrontendMemory *mem_data,
        CSR::ControlState *control_state,
        Xlen xlen,
        ConfigIsaWord isa_word,
        // Default value is used to keep same interface as core single.
        // Forward was chosen as the most conservative variant (regarding correctness).
        MachineConfig::HazardUnit hazard_unit = MachineConfig::HazardUnit::HU_STALL_FORWARD);

protected:
    void do_step(bool skip_break) override;
    void do_reset() override;

private:
    MachineConfig::HazardUnit hazard_unit;

    bool handle_data_hazards();
    bool detect_mispredicted_jump() const;

    /** Some special instruction require that all issued instructions are committed before this
     * instruction is fetched and issued as it may rely on side-effects of uncommitted instructions.
     * Typical examples are csr modifying instructions. */
    bool is_stall_requested() const;

    void handle_stall(const FetchInterstage &saved_if_id);
    /**
     * Typically, problem in execution is discovered in memory stage. This function flushed all
     * stages containing instructions, that would not execute in a single cycle CPU and continues
     * execution from given address.
     *
     * @param next_pc   address to continue execution from
     */
    void flush_and_continue_from_address(Address next_pc);
};

class ExceptionHandler : public QObject {
    Q_OBJECT
public:
    virtual bool handle_exception(
        Core *core,
        Registers *regs,
        ExceptionCause excause,
        Address inst_addr,
        Address next_addr,
        Address jump_branch_pc,
        Address mem_ref_addr)
        = 0;
};

class StopExceptionHandler : public ExceptionHandler {
    Q_OBJECT
public:
    bool handle_exception(
        Core *core,
        Registers *regs,
        ExceptionCause excause,
        Address inst_addr,
        Address next_addr,
        Address jump_branch_pc,
        Address mem_ref_addr) override;
};

struct hwBreak {
    explicit hwBreak(Address addr) : addr(addr), flags(0), count(0) {};
    Address addr;
    unsigned int flags;
    unsigned int count;
};

} // namespace machine

#endif // CORE_H
