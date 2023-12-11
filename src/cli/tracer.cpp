#include "tracer.h"

#include <cinttypes>

using namespace machine;

Tracer::Tracer(Machine *machine) : core_state(machine->core()->get_state()) {
    cycle_limit = 0;

    connect(machine->core(), &Core::step_done, this, &Tracer::step_output);
}

template<typename StageStruct>
void trace_instruction_in_stage(
    const char *stage_name,
    const StageStruct &stage,
    const WritebackInternalState &wb) {
    printf(
        "%s: %s%s\n", stage_name, (stage.excause != EXCAUSE_NONE) ? "!" : "",
        qPrintable(wb.inst.to_str(stage.inst_addr)));
}

void Tracer::step_output() {
    const auto &if_id = core_state.pipeline.fetch.final;
    const auto &id_ex = core_state.pipeline.decode.final;
    const auto &ex_mem = core_state.pipeline.execute.final;
    const auto &mem = core_state.pipeline.memory.internal;
    const auto &mem_wb = core_state.pipeline.memory.final;
    const auto &wb = core_state.pipeline.writeback.internal;
    if (trace_fetch) { trace_instruction_in_stage("Fetch", if_id, wb); }
    if (trace_decode) { trace_instruction_in_stage("Decode", id_ex, wb); }
    if (trace_execute) { trace_instruction_in_stage("Execute", ex_mem, wb); }
    if (trace_memory) { trace_instruction_in_stage("Memory", mem_wb, wb); }
    if (trace_writeback) {
        // All exceptions are resolved in memory, therefore there is no excause field in WB.
        printf("Writeback: %s\n", qPrintable(wb.inst.to_str(wb.inst_addr)));
    }
    if (trace_pc) { printf("PC: %" PRIx64 "\n", if_id.inst_addr.get_raw()); }
    if (trace_regs_gp && wb.regwrite && regs_to_trace.at(wb.num_rd)) {
        printf("GP %zu: %" PRIx64 "\n", size_t(wb.num_rd), wb.value.as_u64());
    }
    if (trace_rdmem && mem_wb.memtoreg) {
        printf("MEM[%" PRIx64 "]:  RD %" PRIx64 "\n", mem_wb.mem_addr.get_raw(),
               mem_wb.towrite_val.as_u64());
    }
    if (trace_wrmem && mem.memwrite) {
        printf("MEM[%" PRIx64 "]:  WR %" PRIx64 "\n", mem_wb.mem_addr.get_raw(),
               mem.mem_write_val.as_u64());
    }
    if ((cycle_limit != 0) && (core_state.cycle_count >= cycle_limit)) {
        emit cycle_limit_reached();
    }
}
