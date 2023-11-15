#include "memory/backend/peripspiled.h"

#include "common/endian.h"

using namespace machine;

constexpr size_t SPILED_REG_LED_LINE_o = 0x004;
constexpr size_t SPILED_REG_LED_RGB1_o = 0x010;
constexpr size_t SPILED_REG_LED_RGB2_o = 0x014;
constexpr size_t SPILED_REG_LED_KBDWR_DIRECT_o = 0x018;

constexpr size_t SPILED_REG_KBDRD_KNOBS_DIRECT_o = 0x020;
constexpr size_t SPILED_REG_KNOBS_8BIT_o = 0x024;

PeripSpiLed::PeripSpiLed(Endian simulated_machine_endian)
    : BackendMemory(simulated_machine_endian) {}

PeripSpiLed::~PeripSpiLed() = default;

WriteResult PeripSpiLed::write(
    Offset destination,
    const void *source,
    size_t size,
    WriteOptions options) {
    UNUSED(options)
    return write_by_u32(
        destination, source, size,
        [&](Offset src) {
            return byteswap_if(
                read_reg(src), internal_endian != simulated_machine_endian);
        },
        [&](Offset src, uint32_t value) {
            return write_reg(
                src, byteswap_if(
                         value, internal_endian != simulated_machine_endian));
        });
}

ReadResult PeripSpiLed::read(
    void *destination,
    Offset source,
    size_t size,
    ReadOptions options) const {
    UNUSED(options)
    return read_by_u32(destination, source, size, [&](Offset src) {
        return byteswap_if(
            read_reg(src), internal_endian != simulated_machine_endian);
    });
}

uint32_t PeripSpiLed::read_reg(Offset source) const {
    Q_ASSERT((source & 3U) == 0); // uint32_t aligned
    uint32_t value = [&]() {
        switch (source) {
        case SPILED_REG_LED_LINE_o: return spiled_reg_led_line;
        case SPILED_REG_LED_RGB1_o: return spiled_reg_led_rgb1;
        case SPILED_REG_LED_RGB2_o: return spiled_reg_led_rgb2;
        case SPILED_REG_LED_KBDWR_DIRECT_o: return spiled_reg_led_kbdwr_direct;
        case SPILED_REG_KBDRD_KNOBS_DIRECT_o:
            return spiled_reg_kbdrd_knobs_direct;
        case SPILED_REG_KNOBS_8BIT_o: return spiled_reg_knobs_8bit;
        default:
            // Todo show this to user as this is failure of supplied program
            printf("[WARNING] PeripSpiLed: read to non-readable location.\n");
            return 0u;
        }
    }();

    emit read_notification(source, value);

    return value;
}

bool PeripSpiLed::write_reg(Offset destination, uint32_t value) {
    Q_ASSERT((destination & 3U) == 0); // uint32_t aligned

    bool changed = [&]() {
        switch (destination) {
        case SPILED_REG_LED_LINE_o: {
            if (spiled_reg_led_line != value) {
                spiled_reg_led_line = value;
                emit led_line_changed(spiled_reg_led_line);
                return true;
            }
            return false;
        }
        case SPILED_REG_LED_RGB1_o:
            if (spiled_reg_led_rgb1 != value) {
                spiled_reg_led_rgb1 = value;
                emit led_rgb1_changed(spiled_reg_led_rgb1);
                return true;
            }
            return false;
        case SPILED_REG_LED_RGB2_o:
            if (spiled_reg_led_rgb2 != value) {
                spiled_reg_led_rgb2 = value;
                emit led_rgb2_changed(spiled_reg_led_rgb2);
                return true;
            }
            return false;
        default:
            // Todo show this to user as this is failure of supplied program
            printf("[WARNING] PeripSpiLed: write to non-writable location.\n");
            return false;
        }
    }();

    emit write_notification(destination, value);

    return changed;
}

void PeripSpiLed::knob_update_notify(uint32_t val, uint32_t mask, size_t shift) {
    mask <<= shift;
    val <<= shift;

    if (!((spiled_reg_knobs_8bit ^ val) & mask)) {
        return;
    }

    spiled_reg_knobs_8bit &= ~mask;
    spiled_reg_knobs_8bit |= val;

    emit external_backend_change_notify(
        this, SPILED_REG_KNOBS_8BIT_o, SPILED_REG_KNOBS_8BIT_o + 3, ae::EXTERNAL_ASYNC);
}

void PeripSpiLed::red_knob_update(int val) {
    knob_update_notify(val, 0xff, 16);
}

void PeripSpiLed::green_knob_update(int val) {
    knob_update_notify(val, 0xff, 8);
}

void PeripSpiLed::blue_knob_update(int val) {
    knob_update_notify(val, 0xff, 0);
}

void PeripSpiLed::red_knob_push(bool state) {
    knob_update_notify(state ? 1 : 0, 1, 26);
}

void PeripSpiLed::green_knob_push(bool state) {
    knob_update_notify(state ? 1 : 0, 1, 25);
}

void PeripSpiLed::blue_knob_push(bool state) {
    knob_update_notify(state ? 1 : 0, 1, 24);
}
LocationStatus PeripSpiLed::location_status(Offset offset) const {
    switch (offset & ~3U) {
    case SPILED_REG_LED_LINE_o: FALLTROUGH
    case SPILED_REG_LED_RGB1_o: FALLTROUGH
    case SPILED_REG_LED_RGB2_o: {
        return LOCSTAT_NONE;
    }
    case SPILED_REG_LED_KBDWR_DIRECT_o: FALLTROUGH
    case SPILED_REG_KBDRD_KNOBS_DIRECT_o: FALLTROUGH
    case SPILED_REG_KNOBS_8BIT_o: {
        return LOCSTAT_READ_ONLY;
    }
    default: {
        return LOCSTAT_ILLEGAL;
    }
    }
}
