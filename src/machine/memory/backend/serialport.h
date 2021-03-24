#ifndef SERIALPORT_H
#define SERIALPORT_H

#include "common/endian.h"
#include "memory/backend/backend_memory.h"
#include "memory/backend/peripheral.h"
#include "simulator_exception.h"

#include <cstdint>

namespace machine {

class SerialPort : public BackendMemory {
    Q_OBJECT
public:
    explicit SerialPort(Endian simulated_machine_endian);
    ~SerialPort() override;

signals:
    void tx_byte(unsigned int data);
    void rx_byte_pool(int fd, unsigned int &data, bool &available) const;
    void write_notification(Offset address, uint32_t value);
    void read_notification(Offset address, uint32_t value) const;
    void signal_interrupt(uint irq_level, bool active) const;

public slots:
    void rx_queue_check() const;

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

private:
    uint32_t read_reg(Offset source, AccessEffects type) const;
    bool write_reg(Offset destination, uint32_t value);
    void rx_queue_check_internal() const;
    void pool_rx_byte() const;
    void update_rx_irq() const;
    void update_tx_irq() const;
    uint32_t get_change_counter() const;

    /** endian of internal registers of the periphery use. */
    static constexpr Endian internal_endian = NATIVE_ENDIAN;
    const uint8_t tx_irq_level;
    const uint8_t rx_irq_level;
    mutable uint32_t change_counter = { 0 };
    mutable uint32_t tx_st_reg = { 0 };
    mutable uint32_t rx_st_reg = { 0 };
    mutable uint32_t rx_data_reg = { 0 };
    mutable bool tx_irq_active = false;
    mutable bool rx_irq_active = false;
};

} // namespace machine

#endif // SERIALPORT_H
