#include "memory/backend/aclintmswi.h"

#include "common/endian.h"

#include <QTimerEvent>

using ae = machine::AccessEffects; // For enum values, type is obvious from
                                   // context.

 namespace machine::aclint {

AclintMswi::AclintMswi(Endian simulated_machine_endian)
    : BackendMemory(simulated_machine_endian)
    , mswi_irq_level(3)
{
    mswi_count = 1;
    for (bool & i : mswi_value)
        i = false;
}

AclintMswi::~AclintMswi() = default;

bool AclintMswi::update_mswi_irq() {
    bool active;

    active = mswi_value[0];

    if (active != mswi_irq_active) {
        mswi_irq_active = active;
        emit signal_interrupt(mswi_irq_level, active);
    }
    return active;
}

WriteResult AclintMswi::write(
    Offset destination,
    const void *source,
    size_t size,
    WriteOptions options) {
    UNUSED(options)
    return write_by_u32(
        destination, source, size,
        [&](Offset src) {
            return byteswap_if(
                read_reg32(src, options.type), internal_endian != simulated_machine_endian);
        },
        [&](Offset src, uint32_t value) {
            return write_reg32(
                src, byteswap_if(
                    value, internal_endian != simulated_machine_endian));
        });
}

ReadResult AclintMswi::read(
    void *destination,
    Offset source,
    size_t size,
    ReadOptions options) const {
    return read_by_u32(destination, source, size, [&](Offset src) {
        return byteswap_if(
            read_reg32(src, options.type),
            internal_endian != simulated_machine_endian);
    });
}

uint32_t AclintMswi::read_reg32(Offset source, AccessEffects type) const {
    Q_UNUSED(type)
    Q_ASSERT((source & 3U) == 0); // uint32_t aligned

    uint32_t value = 0;

    if ((source >= ACLINT_MSWI_OFFSET) &&
               (source < ACLINT_MSWI_OFFSET + 4 * mswi_count)) {
        value = mswi_value[source >> 2]? 1: 0;
    }

    emit read_notification(source, value);

    return value;
}

bool AclintMswi::write_reg32(Offset destination, uint32_t value) {
    Q_ASSERT((destination & 3U) == 0); // uint32_t aligned

    bool changed = false;

    if ((destination >= ACLINT_MSWI_OFFSET) &&
               (destination < ACLINT_MSWI_OFFSET + 4 * mswi_count)) {
        bool value_bool = value & 1;
        changed = value_bool != mswi_value[destination >> 2];
        mswi_value[destination >> 2] = value_bool;
        update_mswi_irq();
    } else {
        printf("WARNING: ACLINT MSWI - read out of range (at 0x%zu).\n", destination);
    }

    emit write_notification(destination, value);

    return changed;
}
LocationStatus AclintMswi::location_status(Offset offset) const {

    if ((offset >= ACLINT_MSWI_OFFSET) &&
        (offset < ACLINT_MSWI_OFFSET + 4 * mswi_count))
        return LOCSTAT_NONE;

    return LOCSTAT_ILLEGAL;
}

} // namespace machine aclint
