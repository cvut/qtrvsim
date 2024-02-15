#ifndef ACLINTSSWI_H
#define ACLINTSSWI_H

#include "common/endian.h"
#include "memory/backend/backend_memory.h"

#include <cstdint>

#include <QTime>

namespace machine::aclint {

constexpr Offset CLINT_SSWI_OFFSET    = 0xc000u;
constexpr Offset CLINT_SSWI_SIZE      = 0x4000u;

constexpr Offset ACLINT_SSWI_OFFSET     =   0;
constexpr Offset ACLINT_SSWI_COUNT_MAX  =   1;

// Timer interrupts
// mip.MTIP and mie.MTIE are bit 7
// mip.STIP and mie.STIE are bit 5

// Software interrupts
// mip.MSIP and mie.MSIE are bit 3
// mip.SSIP and mie.SSIE are bit 1

class AclintSswi : public BackendMemory {
    Q_OBJECT
public:
    explicit AclintSswi(Endian simulated_machine_endian);
    ~AclintSswi() override;

signals:
    void write_notification(Offset address, uint32_t value);
    void read_notification(Offset address, uint32_t value) const;
    void signal_interrupt(uint irq_level, bool active) const;

public:
    WriteResult write(
        Offset destination,
        const void *source,
        size_t size,
        WriteOptions options) override;

    ReadResult read(
        void *destination,
        Offset source,
        size_t size,
        ReadOptions options) const override;

    [[nodiscard]] LocationStatus location_status(Offset offset) const override;

private:
    /** endian of internal registers of the periphery use. */
    static constexpr Endian internal_endian = NATIVE_ENDIAN;

    [[nodiscard]] uint32_t read_reg32(Offset source, AccessEffects type) const;
    bool write_reg32(Offset destination, uint32_t value);

    unsigned sswi_count;

    const uint8_t sswi_irq_level;
};

} // namespace machine aclint

#endif // ACLINTSSWI_H
