#include "memory/backend/aclintmtimer.h"

#include "common/endian.h"

#include <QTimerEvent>
#include <common/logging.h>

LOG_CATEGORY("machine.memory.aclintmtimer");

using ae = machine::AccessEffects; // For enum values, type is obvious from
                                   // context.

namespace machine::aclint {

AclintMtimer::AclintMtimer(Endian simulated_machine_endian)
    : BackendMemory(simulated_machine_endian)
    , mtimer_irq_level(7) {
    mtimecmp_count = 1;

    for (auto &value : mtimecmp_value) {
        value = 0;
    }

    mtime_start_offset = QTime::currentTime();
    qt_timer_id = -1;
}

AclintMtimer::~AclintMtimer() {
    if (qt_timer_id >= 0) killTimer(qt_timer_id);
    qt_timer_id = -1;
}

uint64_t AclintMtimer::mtime_fetch_current() const {
    QTime current_time = QTime::currentTime();

    mtime_last_current_fetch = mtime_start_offset.msecsTo(current_time) * (uint64_t)10000;

    return mtime_last_current_fetch;
}

bool AclintMtimer::update_mtimer_irq() {
    bool active;

    active = mtimecmp_value[0] < mtime_last_current_fetch + mtime_user_offset;

    if (active != mtimer_irq_active) {
        mtimer_irq_active = active;
        emit signal_interrupt(mtimer_irq_level, active);
    }

    if (active) {
        if (qt_timer_id >= 0) killTimer(qt_timer_id);
        qt_timer_id = -1;
    }
    return active;
}

void AclintMtimer::timerEvent(QTimerEvent *event) {
    if (event->timerId() == qt_timer_id) {
        if (qt_timer_id >= 0) killTimer(qt_timer_id);
        qt_timer_id = -1;

        mtime_fetch_current();
        if (!update_mtimer_irq()) { arm_mtimer_event(); }
    } else {
        BackendMemory::timerEvent(event);
    }
}

void AclintMtimer::arm_mtimer_event() {
    if (qt_timer_id >= 0) killTimer(qt_timer_id);
    qt_timer_id = -1;

    uint64_t ticks_to_wait = mtimecmp_value[0] - (mtime_last_current_fetch + mtime_user_offset);
    qt_timer_id = startTimer(ticks_to_wait / 10000);
}

WriteResult
AclintMtimer::write(Offset destination, const void *source, size_t size, WriteOptions options) {
    UNUSED(options)
    return write_by_u64(
        destination, source, size,
        [&](Offset src) {
            return byteswap_if(
                read_reg64(src, options.type), internal_endian != simulated_machine_endian);
        },
        [&](Offset src, uint64_t value) {
            return write_reg64(
                src, byteswap_if(value, internal_endian != simulated_machine_endian));
        });
}

ReadResult
AclintMtimer::read(void *destination, Offset source, size_t size, ReadOptions options) const {
    return read_by_u64(destination, source, size, [&](Offset src) {
        return byteswap_if(
            read_reg64(src, options.type), internal_endian != simulated_machine_endian);
    });
}

uint64_t AclintMtimer::read_reg64(Offset source, AccessEffects type) const {
    Q_UNUSED(type)
    Q_ASSERT((source & 7U) == 0); // uint64_t aligned

    uint64_t value = 0;

    if (source == ACLINT_MTIME_OFFSET) {
        if (type == AccessEffects::REGULAR) mtime_fetch_current();
        value = mtime_last_current_fetch + mtime_user_offset;
    } else if (
        (source >= ACLINT_MTIMECMP_OFFSET)
        && (source < ACLINT_MTIMECMP_OFFSET + 8 * mtimecmp_count)) {
        value = mtimecmp_value[source >> 3];
    }

    emit read_notification(source, value);

    return value;
}

bool AclintMtimer::write_reg64(Offset destination, uint64_t value) {
    Q_ASSERT((destination & 7U) == 0); // uint64_t aligned

    bool changed = false;

    if (destination == ACLINT_MTIME_OFFSET) {
        mtime_fetch_current();
        mtime_user_offset = value - mtime_last_current_fetch;
        if (!update_mtimer_irq()) arm_mtimer_event();
        changed = true;
    } else if (
        (destination >= ACLINT_MTIMECMP_OFFSET)
        && (destination < ACLINT_MTIMECMP_OFFSET + 8 * mtimecmp_count)) {
        changed = value != mtimecmp_value[destination >> 3];
        mtimecmp_value[destination >> 3] = value;
        if (!update_mtimer_irq()) arm_mtimer_event();
    } else {
        WARN("ACLINT MTIMER - read out of range (at 0x%zu).\n", destination);
    }

    emit write_notification(destination, value);

    return changed;
}

LocationStatus AclintMtimer::location_status(Offset offset) const {
    if ((offset >= ACLINT_MTIMECMP_OFFSET)
        && (offset < ACLINT_MTIMECMP_OFFSET + 8 * mtimecmp_count))
        return LOCSTAT_NONE;
    if ((offset & ~7U) == ACLINT_MTIME_OFFSET)
        return LOCSTAT_NONE; // LOCSTAT_NONE / LOCSTAT_READ_ONLY

    return LOCSTAT_ILLEGAL;
}
} // namespace machine::aclint
