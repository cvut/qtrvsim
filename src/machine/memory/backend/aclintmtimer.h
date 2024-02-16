#ifndef ACLINTMTIMER_H
#define ACLINTMTIMER_H

#include "common/endian.h"
#include "memory/backend/backend_memory.h"

#include <QTime>
#include <cstdint>

namespace machine { namespace aclint {

    constexpr Offset CLINT_MTIMER_OFFSET = 0x4000u;
    constexpr Offset CLINT_MTIMER_SIZE = 0x8000u;

    constexpr Offset ACLINT_MTIME_OFFSET = 0x7ff8u;
    constexpr Offset ACLINT_MTIME_SIZE = 0x8u;
    constexpr Offset ACLINT_MTIMECMP_OFFSET = 0x0000u;
    constexpr Offset ACLINT_MTIMECMP_SIZE = 0x7ff8u;
    constexpr unsigned ACLINT_MTIMECMP_COUNT_MAX = 1;

    // Timer interrupts
    // mip.MTIP and mie.MTIE are bit 7
    // mip.STIP and mie.STIE are bit 5

    // Software interrupts
    // mip.MSIP and mie.MSIE are bit 3
    // mip.SSIP and mie.SSIE are bit 1

    class AclintMtimer : public BackendMemory {
        Q_OBJECT
    public:
        explicit AclintMtimer(Endian simulated_machine_endian);
        ~AclintMtimer() override;

    signals:
        void write_notification(Offset address, uint32_t value);
        void read_notification(Offset address, uint32_t value) const;
        void signal_interrupt(uint irq_level, bool active) const;

    public:
        uint64_t mtime_fetch_current() const;

        WriteResult
        write(Offset destination, const void *source, size_t size, WriteOptions options) override;

        ReadResult
        read(void *destination, Offset source, size_t size, ReadOptions options) const override;

        LocationStatus location_status(Offset offset) const override;

    private:
        void timerEvent(QTimerEvent *event) override;

        /** endian of internal registers of the periphery use. */
        static constexpr Endian internal_endian = NATIVE_ENDIAN;

        uint64_t read_reg64(Offset source, AccessEffects type) const;
        bool write_reg64(Offset destination, uint64_t value);

        bool update_mtimer_irq();
        void arm_mtimer_event();

        unsigned mtimecmp_count;
        uint64_t mtimecmp_value[ACLINT_MTIMECMP_COUNT_MAX] {};

        const uint8_t mtimer_irq_level;
        QTime mtime_start_offset;
        uint64_t mtime_user_offset = 0;
        mutable uint64_t mtime_last_current_fetch = 0;
        mutable bool mtimer_irq_active = false;
        int qt_timer_id = -1;
    };

}} // namespace machine::aclint

#endif // ACLINTMTIMER_H
