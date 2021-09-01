#ifndef CORE_H
#define CORE_H

#include "cop0state.h"
#include "core/core_state.h"
#include "instruction.h"
#include "machineconfig.h"
#include "memory/address.h"
#include "memory/frontend_memory.h"
#include "pipeline.h"
#include "predictor.h"
#include "register_value.h"
#include "registers.h"
#include "simulator_exception.h"
#include "common/memory_ownership.h"

#include <QObject>

namespace machine {

class Core;

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

class Core : public QObject {
    Q_OBJECT
public:
    Core(
        Registers *regs,
        Predictor *predictor,
        FrontendMemory *mem_program,
        FrontendMemory *mem_data,
        unsigned int min_cache_row_size = 1,
        Cop0State *cop0state = nullptr,
        Xlen xlen = Xlen::_32);

    void step(bool skip_break = false); // Do single step
    void reset();                       // Reset core (only core, memory and registers has to be
                                        // reseted separately)

    unsigned get_cycle_count() const; // Returns number of executed
                                      // get_cycle_count
    unsigned get_stall_count() const; // Returns number of stall get_cycle_count

    Registers *get_regs();
    Cop0State *get_cop0state();
    Predictor *get_predictor();
    FrontendMemory *get_mem_data();
    FrontendMemory *get_mem_program();
    void register_exception_handler(ExceptionCause excause, ExceptionHandler *exhandler);
    void insert_hwbreak(Address address);
    void remove_hwbreak(Address address);
    bool is_hwbreak(Address address);
    void set_stop_on_exception(enum ExceptionCause excause, bool value);
    bool get_stop_on_exception(enum ExceptionCause excause) const;
    void set_step_over_exception(enum ExceptionCause excause, bool value);
    bool get_step_over_exception(enum ExceptionCause excause) const;

    void set_c0_userlocal(uint32_t address);

public:
    CoreState state {};

signals:
    void instruction_fetched(
        const machine::Instruction &inst,
        Address inst_addr,
        ExceptionCause excause,
        bool valid);
    void instruction_decoded(
        const machine::Instruction &inst,
        Address inst_addr,
        ExceptionCause excause,
        bool valid);
    void instruction_executed(
        const machine::Instruction &inst,
        Address inst_addr,
        ExceptionCause excause,
        bool valid);
    void instruction_memory(
        const machine::Instruction &inst,
        Address inst_addr,
        ExceptionCause excause,
        bool valid);
    void instruction_writeback(
        const machine::Instruction &inst,
        Address inst_addr,
        ExceptionCause excause,
        bool valid);
    void instruction_program_counter(
        const machine::Instruction &inst,
        Address inst_addr,
        ExceptionCause excause,
        bool valid);

    void fetch_inst_addr_value(machine::Address);
    void fetch_jump_reg_value(uint32_t);
    void fetch_jump_value(uint32_t);
    void fetch_branch_value(uint32_t);
    void decode_inst_addr_value(machine::Address);
    void decode_instruction_value(uint32_t);
    void decode_reg1_value(uint32_t);
    void decode_reg2_value(uint32_t);
    void decode_immediate_value(uint32_t);
    void decode_regw_value(uint32_t);
    void decode_memtoreg_value(uint32_t);
    void decode_memwrite_value(uint32_t);
    void decode_memread_value(uint32_t);
    void decode_alusrc_value(uint32_t);
    void decode_rs_num_value(uint32_t);
    void decode_rt_num_value(uint32_t);
    void decode_rd_num_value(uint32_t);
    void forward_m_d_rs_value(uint32_t);
    void forward_m_d_rt_value(uint32_t);
    void execute_inst_addr_value(machine::Address);
    void execute_alu_value(uint32_t);
    void execute_reg1_value(uint32_t);
    void execute_reg2_value(uint32_t);
    void execute_reg1_ff_value(uint32_t);
    void execute_reg2_ff_value(uint32_t);
    void execute_immediate_value(uint32_t);
    void execute_regw_value(uint32_t);
    void execute_memtoreg_value(uint32_t);
    void execute_memwrite_value(uint32_t);
    void execute_memread_value(uint32_t);
    void execute_alusrc_value(uint32_t);
    void execute_regdest_value(uint32_t);
    void execute_regw_num_value(uint32_t);
    void execute_stall_forward_value(uint32_t);
    void execute_rs_num_value(uint32_t);
    void execute_rt_num_value(uint32_t);
    void execute_rd_num_value(uint32_t);
    void memory_inst_addr_value(machine::Address);
    void memory_alu_value(uint32_t);
    void memory_rt_value(uint32_t);
    void memory_mem_value(uint32_t);
    void memory_regw_value(uint32_t);
    void memory_memtoreg_value(uint32_t);
    void memory_memwrite_value(uint32_t);
    void memory_memread_value(uint32_t);
    void memory_regw_num_value(uint32_t);
    void memory_excause_value(uint32_t);
    void writeback_inst_addr_value(machine::Address);
    void writeback_value(uint32_t);
    void writeback_memtoreg_value(uint32_t);
    void writeback_regw_value(uint32_t);
    void writeback_regw_num_value(uint32_t);

    void hu_stall_value(uint32_t);
    void branch_forward_value(uint32_t);

    void cycle_c_value(uint32_t);
    void stall_c_value(uint32_t);

    void stop_on_exception_reached();

    void step_done() const;

protected:
    virtual void do_step(bool skip_break = false) = 0;
    virtual void do_reset() = 0;

    bool handle_exception(
        Core *core,
        Registers *regs,
        ExceptionCause excause,
        Instruction inst,
        Address inst_addr,
        Address next_addr,
        Address jump_branch_pc,
        Address mem_ref_addr);

    /**
     * Abstracts XLEN from code flow. XLEN core will obtain XLEN value from register value.
     * The value will be zero extended to u64.
     */
    uint64_t get_xlen_from_reg(RegisterValue reg) const;

    const Xlen xlen;
    BORROWED Registers *const regs;
    BORROWED Cop0State *const cop0state;
    BORROWED Predictor *const predictor;
    BORROWED FrontendMemory *const mem_data, *const mem_program;
    QMap<ExceptionCause, OWNED ExceptionHandler *> ex_handlers;
    Box<ExceptionHandler> ex_default_handler;

    FetchState fetch(bool skip_break = false);
    DecodeState decode(const FetchInterstage &);
    ExecuteState execute(const DecodeInterstage &);
    MemoryState memory(const ExecuteInterstage &);
    WritebackState writeback(const MemoryInterstage &);
    Address handle_pc(const ExecuteInterstage &);
    void flush();

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
        Predictor *predictor,
        FrontendMemory *mem_program,
        FrontendMemory *mem_data,
        unsigned int min_cache_row_size,
        Cop0State *cop0state,
        Xlen xlen);

protected:
    void do_step(bool skip_break = false) override;
    void do_reset() override;

private:
    Address prev_inst_addr {};
};

class CorePipelined : public Core {
public:
    CorePipelined(
        Registers *regs,
        Predictor *predictor,
        FrontendMemory *mem_program,
        FrontendMemory *mem_data,
        enum MachineConfig::HazardUnit hazard_unit,
        unsigned int min_cache_row_size,
        Cop0State *cop0state,
        Xlen xlen);

protected:
    void do_step(bool skip_break = false) override;
    void do_reset() override;

private:
    MachineConfig::HazardUnit hazard_unit;
};

std::tuple<bool, Address> predict(Instruction inst, Address addr);

} // namespace machine

#endif // CORE_H
