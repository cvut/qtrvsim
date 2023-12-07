#ifndef QTRVSIM_CORE_STATE_H
#define QTRVSIM_CORE_STATE_H

#include "machinedefs.h"
#include "pipeline.h"
#include "common/memory_ownership.h"
#include "memory/address_range.h"

#include <QMap>
#include <cstdint>
#include <machineconfig.h>
using std::uint32_t;

namespace machine {

struct CoreState {
    Pipeline pipeline = {};
    AddressRange LoadReservedRange;
    uint32_t stall_count = 0;
    uint32_t cycle_count = 0;
};

} // namespace machine
#endif // QTRVSIM_CORE_STATE_H
