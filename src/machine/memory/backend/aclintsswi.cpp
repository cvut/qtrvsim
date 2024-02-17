#include "memory/backend/aclintsswi.h"

#include "common/endian.h"

#include <cstdio>
#include <QTimerEvent>

using ae = machine::AccessEffects; // For enum values, type is obvious from
                                   // context.

namespace machine {  namespace aclint {

AclintSswi::AclintSswi(Endian simulated_machine_endian)
    : BackendMemory(simulated_machine_endian)
    , sswi_irq_level(1)
{
    sswi_count = 1;
}

AclintSswi::~AclintSswi() = default;


WriteResult AclintSswi::write(
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

ReadResult AclintSswi::read(
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

uint32_t AclintSswi::read_reg32(Offset source, AccessEffects type) const {
    Q_UNUSED(type)
    Q_ASSERT((source & 3U) == 0); // uint32_t aligned

    uint32_t value = 0;

    if ((source >= ACLINT_SSWI_OFFSET) &&
               (source < ACLINT_SSWI_OFFSET + 4 * sswi_count)) {
    }

    emit read_notification(source, value);

    return value;
}

bool AclintSswi::write_reg32(Offset destination, uint32_t value) {
    Q_ASSERT((destination & 3U) == 0); // uint32_t aligned

    bool changed = false;

    if ((destination >= ACLINT_SSWI_OFFSET) &&
               (destination < ACLINT_SSWI_OFFSET + 4 * sswi_count)) {
        bool value_bool = value & 1;
        if (value_bool)
            emit signal_interrupt(sswi_irq_level, value_bool);
     } else {
        printf("WARNING: ACLINT SSWI - read out of range (at 0x%zu).\n", destination);
    }

    emit write_notification(destination, value);

    return changed;
}
LocationStatus AclintSswi::location_status(Offset offset) const {

    if ((offset >= ACLINT_SSWI_OFFSET) &&
        (offset < ACLINT_SSWI_OFFSET + 4 * sswi_count))
        return LOCSTAT_NONE;

    return LOCSTAT_ILLEGAL;
}

} } // namespace machine aclint
