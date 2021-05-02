#ifndef CORE_H
#define CORE_H

#include "cop0state.h"
#include "instruction.h"
#include "machineconfig.h"
#include "memory/address.h"
#include "memory/frontend_memory.h"
#include "register_value.h"
#include "registers.h"
#include "simulator_exception.h"

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
        bool in_delay_slot,
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
        bool in_delay_slot,
        Address mem_ref_addr) override;
};

class Core : public QObject {
    Q_OBJECT
public:
    Core(
        Registers *regs,
        FrontendMemory *mem_program,
        FrontendMemory *mem_data,
        unsigned int min_cache_row_size = 1,
        Cop0State *cop0state = nullptr);
    ~Core() override;

    void step(bool skip_break = false); // Do single step
    void reset(); // Reset core (only core, memory and registers has to be
                  // reseted separately)

    unsigned get_cycle_count() const; // Returns number of executed
                                      // get_cycle_count
    unsigned get_stall_count() const; // Returns number of stall get_cycle_count

    Registers *get_regs();
    Cop0State *get_cop0state();
    FrontendMemory *get_mem_data();
    FrontendMemory *get_mem_program();
    void register_exception_handler(
        ExceptionCause excause,
        ExceptionHandler *exhandler);
    void insert_hwbreak(Address address);
    void remove_hwbreak(Address address);
    bool is_hwbreak(Address address);
    void set_stop_on_exception(enum ExceptionCause excause, bool value);
    bool get_stop_on_exception(enum ExceptionCause excause) const;
    void set_step_over_exception(enum ExceptionCause excause, bool value);
    bool get_step_over_exception(enum ExceptionCause excause) const;

    void set_c0_userlocal(uint32_t address);

    enum ForwardFrom {
        FORWARD_NONE = 0b00,
        FORWARD_FROM_W = 0b01,
        FORWARD_FROM_M = 0b10,
    };

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
    void decode_regdest_value(uint32_t);
    void decode_rs_num_value(uint32_t);
    void decode_rt_num_value(uint32_t);
    void decode_rd_num_value(uint32_t);
    void decode_regd31_value(uint32_t);
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

protected:
    virtual void do_step(bool skip_break = false) = 0;
    virtual void do_reset() = 0;

    bool handle_exception(
        Core *core,
        Registers *regs,
        ExceptionCause excause,
        Address inst_addr,
        Address next_addr,
        Address jump_branch_pc,
        bool in_delay_slot,
        Address mem_ref_addr);

    Registers *regs;
    Cop0State *cop0state;
    FrontendMemory *mem_data, *mem_program;
    QMap<ExceptionCause, ExceptionHandler *> ex_handlers;
    ExceptionHandler *ex_default_handler;

    struct dtFetch {
        Instruction inst;  // Loaded instruction
        Address inst_addr; // Address of instruction
        enum ExceptionCause excause;
        bool in_delay_slot;
        bool is_valid;
    };
    struct dtDecode {
        Instruction inst;
        bool memread;              // If memory should be read
        bool memwrite;             // If memory should write input
        bool alusrc;               // If second value to alu is immediate value (rt used
                                   // otherwise)
        bool regd;                 // If rd is used (otherwise rt is used for write target)
        bool regd31;               // Use R31 as destionation for JAL
        bool regwrite;             // If output should be written back to register (which
                                   // one depends on regd)
        bool alu_req_rs;           // requires rs value for ALU
        bool alu_req_rt;           // requires rt value for ALU or SW
        bool bjr_req_rs;           // requires rs for beq, bne, blez, bgtz, jr nad jalr
        bool bjr_req_rt;           // requires rt for beq, bne
        bool branch;               // branch instruction
        bool jump;                 // jump
        bool bj_not;               // negate branch condition
        bool bgt_blez;             // BGTZ/BLEZ instead of BGEZ/BLTZ
        bool nb_skip_ds;           // Skip delay slot if branch is not taken
        bool forward_m_d_rs;       // forwarding required for beq, bne, blez, bgtz, jr
                                   // nad jalr
        bool forward_m_d_rt;       // forwarding required for beq, bne
        enum AluOp aluop;          // Decoded ALU operation
        enum AccessControl memctl; // Decoded memory access type
        uint8_t num_rs;            // Number of the register s
        uint8_t num_rt;            // Number of the register t
        uint8_t num_rd;            // Number of the register d
        RegisterValue val_rs;      // Value from register rs
        RegisterValue val_rt;      // Value from register rt
        uint32_t immediate_val;    // zero or sign-extended immediate value
        uint8_t rwrite;            // Writeback register (multiplexed between rt and
                                   // rd according to regd)
        ForwardFrom ff_rs;
        ForwardFrom ff_rt;
        Address inst_addr; // Address of instruction
        enum ExceptionCause excause;
        bool in_delay_slot;
        bool stall;
        bool stop_if;
        bool is_valid;
    };
    struct dtExecute {
        Instruction inst;
        bool memread;
        bool memwrite;
        bool regwrite;
        enum AccessControl memctl;
        RegisterValue val_rt;
        uint8_t rwrite;
        // Writeback register (multiplexed between rt and rd according to regd)
        RegisterValue alu_val; // Result of ALU execution
        Address inst_addr;     // Address of instruction
        enum ExceptionCause excause;
        bool in_delay_slot;
        bool stop_if;
        bool is_valid;
    };
    struct dtMemory {
        Instruction inst;
        bool memtoreg;
        bool regwrite;
        uint8_t rwrite;
        RegisterValue towrite_val;
        Address mem_addr;  // Address used to access memory
        Address inst_addr; // Address of instruction
        enum ExceptionCause excause;
        bool in_delay_slot;
        bool stop_if;
        bool is_valid;
    };

    struct dtFetch fetch(bool skip_break = false);
    struct dtDecode decode(const struct dtFetch &);
    struct dtExecute execute(const struct dtDecode &);
    struct dtMemory memory(const struct dtExecute &);
    void writeback(const struct dtMemory &);
    bool handle_pc(const struct dtDecode &);

    enum ExceptionCause memory_special(
        enum AccessControl memctl,
        int mode,
        bool memread,
        bool memwrite,
        RegisterValue &towrite_val,
        RegisterValue rt_value,
        Address mem_addr);

    // Initialize structures to NOPE instruction
    static void dtFetchInit(struct dtFetch &dt);
    static void dtDecodeInit(struct dtDecode &dt);
    static void dtExecuteInit(struct dtExecute &dt);
    static void dtMemoryInit(struct dtMemory &dt);

protected:
    unsigned int stall_c;

private:
    struct hwBreak {
        hwBreak(Address addr);
        Address addr;
        unsigned int flags;
        unsigned int count;
    };
    unsigned int cycle_c;
    unsigned int min_cache_row_size;
    uint32_t hwr_userlocal;
    QMap<Address, hwBreak *> hw_breaks;
    bool stop_on_exception[EXCAUSE_COUNT] {};
    bool step_over_exception[EXCAUSE_COUNT] {};
};

class CoreSingle : public Core {
public:
    CoreSingle(
        Registers *regs,
        FrontendMemory *mem_program,
        FrontendMemory *mem_data,
        bool jmp_delay_slot,
        unsigned int min_cache_row_size = 1,
        Cop0State *cop0state = nullptr);
    ~CoreSingle() override;

protected:
    void do_step(bool skip_break = false) override;
    void do_reset() override;

private:
    struct Core::dtFetch *dt_f;
    Address prev_inst_addr {};
};

class CorePipelined : public Core {
public:
    CorePipelined(
        Registers *regs,
        FrontendMemory *mem_program,
        FrontendMemory *mem_data,
        enum MachineConfig::HazardUnit hazard_unit
        = MachineConfig::HU_STALL_FORWARD,
        unsigned int min_cache_row_size = 1,
        Cop0State *cop0state = nullptr);

protected:
    void do_step(bool skip_break = false) override;
    void do_reset() override;

private:
    struct Core::dtFetch dt_f;
    struct Core::dtDecode dt_d;
    struct Core::dtExecute dt_e;
    struct Core::dtMemory dt_m;

    enum MachineConfig::HazardUnit hazard_unit;
};

} // namespace machine

#endif // CORE_H
