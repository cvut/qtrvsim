#include "memory/backend/peripheral.h"

#include "common/endian.h"

using namespace machine;

SimplePeripheral::SimplePeripheral(Endian simulated_machine_endian)
    : BackendMemory(simulated_machine_endian) {};

SimplePeripheral::~SimplePeripheral() = default;

WriteResult SimplePeripheral::write(
    Offset destination,
    const void *source,
    size_t size,
    WriteOptions options) {
    UNUSED(source)
    UNUSED(options)

    // Write to dummy periphery is nop

    emit write_notification(destination, size);

    return { size, false };
}

ReadResult SimplePeripheral::read(
    void *destination,
    Offset source,
    size_t size,
    ReadOptions options) const {
    UNUSED(options)

    memset(destination, 0x12, size); // Random value

    emit read_notification(source, size);

    return { size };
}
LocationStatus SimplePeripheral::location_status(Offset offset) const {
    UNUSED(offset)
    return LOCSTAT_NONE;
}

