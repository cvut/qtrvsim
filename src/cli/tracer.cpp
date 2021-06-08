#include "tracer.h"

#include <cinttypes>

using namespace machine;

Tracer::Tracer(Machine *machine) : core_state(machine->core()->state) {
    connect(machine->core(), &Core::step_done, this, &Tracer::step_output);
}

template<typename StageStruct>
constexpr const char *exception_mark(StageStruct stage) {
    return (stage.excause != EXCAUSE_NONE) ? "!" : "";
}

void Tracer::step_output() {
    const auto &if_id = core_state.pipeline.fetch.final;
    const auto &id_ex = core_state.pipeline.decode.final;
    const auto &ex_mem = core_state.pipeline.execute.final;
    const auto &mem_wb = core_state.pipeline.memory.final;
    const auto &wb = core_state.pipeline.writeback.internal;
    if (trace_fetch) {
        auto exception_mark = (if_id.excause != EXCAUSE_NONE ? "!" : "");
        printf("Fetch: %s%s\n", exception_mark, qPrintable(wb.inst.to_str(if_id.inst_addr)));
    }
    if (trace_decode) {
        auto exception_mark = (id_ex.excause != EXCAUSE_NONE ? "!" : "");
        printf("Decode: %s%s\n", exception_mark, qPrintable(wb.inst.to_str(id_ex.inst_addr)));
    }
    if (trace_execute) {
        auto exception_mark = (ex_mem.excause != EXCAUSE_NONE ? "!" : "");
        printf("Execute: %s%s\n", exception_mark, qPrintable(wb.inst.to_str(ex_mem.inst_addr)));
    }
    if (trace_memory) {
        auto exception_mark = (mem_wb.excause != EXCAUSE_NONE ? "!" : "");
        printf("Memory: %s%s\n", exception_mark, qPrintable(wb.inst.to_str(mem_wb.inst_addr)));
    }
    if (trace_writeback) { printf("Writeback: %s\n", qPrintable(wb.inst.to_str(wb.inst_addr))); }
    if (trace_pc) { printf("PC: %" PRIx64 "\n", if_id.inst_addr.get_raw()); }
    if (trace_regs_gp && wb.regwrite && regs_to_trace.at(wb.num_rd)) {
        printf("GP %d: %" PRIx64 "\n", wb.num_rd, wb.value.as_u64());
    }
}