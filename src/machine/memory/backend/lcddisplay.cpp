#include "lcddisplay.h"

#include "common/endian.h"

#ifdef DEBUG_LCD
    #undef DEBUG_LCD
    #define DEBUG_LCD true
#else
    #define DEBUG_LCD false
#endif

namespace machine {

LcdDisplay::LcdDisplay(Endian simulated_machine_endian)
    : BackendMemory(simulated_machine_endian)
    , fb_width(480)
    , fb_height(320)
    , fb_bits_per_pixel(16)
    , fb_data(get_fb_size_bytes(), 0) {}

LcdDisplay::~LcdDisplay() = default;

WriteResult LcdDisplay::write(
    Offset destination,
    const void *source,
    size_t size,
    WriteOptions options) {
    UNUSED(options)
    return write_by_u16(
        destination, source, size,
        [&](Offset src) {
            return byteswap_if(
                read_raw_pixel(src), internal_endian != simulated_machine_endian);
        },
        [&](Offset src, uint16_t value) {
            return write_raw_pixel(
                src, byteswap_if(
                         value, internal_endian != simulated_machine_endian));
        });
}

ReadResult LcdDisplay::read(
    void *destination,
    Offset source,
    size_t size,
    ReadOptions options) const {
    UNUSED(options)
    return read_by_u16(destination, source, size, [&](Offset src) {
        return byteswap_if(
            read_raw_pixel(src), internal_endian != simulated_machine_endian);
    });
}

uint16_t LcdDisplay::read_raw_pixel(Offset source) const {
    Q_ASSERT((source & 1U) == 0); // uint16_t aligned

    if (source + 1 >= get_fb_size_bytes()) {
        return 0;
    }

    uint16_t value;
    memcpy(&value, &fb_data[source], sizeof(value));

    // TODO Switch to if constexpr as soon as we have cpp17.
    if (DEBUG_LCD) {
        printf(
            "LcdDisplay::read_reg address 0x%08lx data 0x%08lx\n",
            (unsigned long)source, (unsigned long)value);
    }

    emit read_notification(source, value);
    return value;
}

bool LcdDisplay::write_raw_pixel(Offset destination, uint16_t value) {
    Q_ASSERT((destination & 1U) == 0); // uint16_t aligned

    if (destination + 1 >= get_fb_size_bytes()) {
        printf("WARNING: LCD display - read out of range.");
        return false;
    }

    // TODO Switch to if constexpr as soon as we have cpp17.
    if (DEBUG_LCD) {
        printf(
            "LcdDisplay::write_reg address 0x%08lx data 0x%08lx\n",
            (unsigned long)destination, (unsigned long)value);
    }

    if (read_raw_pixel(destination) == value) {
        return false;
    }

    memcpy(&fb_data[destination], &value, sizeof(value));

    size_t x, y;
    std::tie(x, y) = get_pixel_from_address(destination);

    const uint32_t last_addr = destination + 1;
    uint32_t pixel_addr;
    uint16_t pixel_data;
    uint r, g, b;

    while ((pixel_addr = get_address_from_pixel(x, y)) <= last_addr) {
        memcpy(&pixel_data, &fb_data[pixel_addr], sizeof(pixel_data));

        r = ((pixel_data >> 11u) & 0x1fu) << 3u;
        g = ((pixel_data >> 5u) & 0x3fu) << 2u;
        b = ((pixel_data >> 0u) & 0x1fu) << 3u;

        emit pixel_update(x, y, r, g, b);

        if (++x >= fb_width) {
            x = 0;
            y++;
        }
    }

    emit write_notification(destination, value);

    return true;
}

size_t LcdDisplay::get_address_from_pixel(size_t x, size_t y) const {
    size_t address = y * get_fb_line_size();
    if (fb_bits_per_pixel > 12) {
        address += x * divide_and_ceil(fb_bits_per_pixel, 8u);
    } else {
        address += x * fb_bits_per_pixel / 8;
    }

    return address;
}

std::tuple<size_t, size_t>
LcdDisplay::get_pixel_from_address(size_t address) const {
    size_t y = address / get_fb_line_size();
    size_t x = (fb_bits_per_pixel > 12)
                   ? (address - y * get_fb_line_size())
                         / ((fb_bits_per_pixel + 7) >> 3u)
                   : (address - y * get_fb_line_size()) * 8 / fb_bits_per_pixel;
    return std::make_tuple(x, y);
}

size_t LcdDisplay::get_fb_line_size() const {
    return (fb_bits_per_pixel > 12) ? ((fb_bits_per_pixel + 7) >> 3u) * fb_width
                                    : (fb_bits_per_pixel * fb_width + 7) >> 3u;
}

size_t LcdDisplay::get_fb_size_bytes() const {
    return get_fb_line_size() * fb_height;
}
LocationStatus LcdDisplay::location_status(Offset offset) const {
    if ((offset | ~3u) >= get_fb_size_bytes()) {
        return LOCSTAT_ILLEGAL;
    }
    return LOCSTAT_NONE;
}

} // namespace machine
