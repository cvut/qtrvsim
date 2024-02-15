#ifndef PERIPHERAL_H
#define PERIPHERAL_H

#include "common/endian.h"
#include "machinedefs.h"
#include "memory/backend/backend_memory.h"
#include "memory/backend/memory.h"
#include "memory/memory_utils.h"
#include "simulator_exception.h"

#include <QMap>
#include <QObject>
#include <cstdint>

namespace machine {

/**
 * NOTE: This peripheral is constant, it does not case about endian. Therefore
 * it does not have internal_endian field.
 */
class SimplePeripheral final : public BackendMemory {
    Q_OBJECT
public:
    explicit SimplePeripheral(Endian simulated_machine_endian);
    ~SimplePeripheral() override;

signals:
    void write_notification(Offset address, size_t size) const;
    void read_notification(Offset address, size_t size) const;

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
};

} // namespace machine

#endif // PERIPHERAL_H
