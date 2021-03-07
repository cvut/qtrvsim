#ifndef LCDDISPLAY_H
#define LCDDISPLAY_H

#include "machinedefs.h"
#include "memory/backend/backend_memory.h"
#include "simulator_exception.h"

#include <QMap>
#include <QObject>
#include <cstdint>

namespace machine {

class LcdDisplay final : public BackendMemory {
    Q_OBJECT
public:
    explicit LcdDisplay(Endian simulated_machine_endian);
    ~LcdDisplay() override;

signals:
    void write_notification(Offset offset, uint32_t value) const;
    void read_notification(Offset offset, uint32_t value) const;
    void pixel_update(size_t x, size_t y, uint r, uint g, uint b);

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

    LocationStatus location_status(Offset offset) const override;

    /**
     * @return  framebuffer width in pixels
     */
    inline constexpr size_t get_width() const {
        return fb_width;
    }

    /**
     * @return  framebuffer height in pixels
     */
    inline constexpr size_t get_height() const {
        return fb_height;
    }

private:
    /** Endian internal registers of the periphery (framebuffer) use. */
    static constexpr Endian internal_endian = BIG;

    /** Read HW register - allows only 32bit aligned access. */
    uint32_t read_reg(Offset source) const;

    /** Write HW register - allows only 32bit aligned access */
    bool write_reg(Offset destination, uint32_t value);

    size_t get_fb_line_size() const;
    size_t get_fb_size_bytes() const;
    size_t get_address_from_pixel(size_t x, size_t y) const;
    std::tuple<size_t, size_t> get_pixel_from_address(size_t address) const;

    const size_t fb_width;  //> Width in pixels
    const size_t fb_height; //> Height in pixels
    const size_t fb_bits_per_pixel;
    std::vector<byte> fb_data;
};

} // namespace machine

#endif // LCDDISPLAY_H
