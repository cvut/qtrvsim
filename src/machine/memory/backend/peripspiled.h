#ifndef PERIPSPILED_H
#define PERIPSPILED_H

#include "common/endian.h"
#include "machinedefs.h"
#include "memory/backend/backend_memory.h"
#include "memory/memory_utils.h"
#include "simulator_exception.h"

#include <cstdint>

namespace machine {

class PeripSpiLed final : public BackendMemory {
    Q_OBJECT
public:
    explicit PeripSpiLed(Endian simulated_machine_endian);
    ~PeripSpiLed() override;

signals:
    void write_notification(Offset address, uint32_t value) const;
    void read_notification(Offset address, uint32_t value) const;

    void led_line_changed(uint val) const;
    void led_rgb1_changed(uint val) const;
    void led_rgb2_changed(uint val) const;

public slots:
    void red_knob_update(int val);
    void green_knob_update(int val);
    void blue_knob_update(int val);
    void red_knob_push(bool state);
    void green_knob_push(bool state);
    void blue_knob_push(bool state);

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
    [[nodiscard]] uint32_t read_reg(Offset source) const;
    bool write_reg(Offset destination, uint32_t value);
    void knob_update_notify(uint32_t val, uint32_t mask, size_t shift);

    /** endian of internal registers of the periphery use. */
    static constexpr Endian internal_endian = NATIVE_ENDIAN;
    uint32_t spiled_reg_led_line = 0;
    uint32_t spiled_reg_led_rgb1 = 0;
    uint32_t spiled_reg_led_rgb2 = 0;
    uint32_t spiled_reg_led_kbdwr_direct = 0;
    uint32_t spiled_reg_kbdrd_knobs_direct = 0;
    uint32_t spiled_reg_knobs_8bit = 0;
};

} // namespace machine

#endif // PERIPSPILED_H
