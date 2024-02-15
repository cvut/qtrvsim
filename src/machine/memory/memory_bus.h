#ifndef MEMORY_BUS_H
#define MEMORY_BUS_H

#include "common/endian.h"
#include "machinedefs.h"
#include "memory/backend/backend_memory.h"
#include "memory/frontend_memory.h"
#include "simulator_exception.h"
#include "utils.h"

#include <QMultiMap>
#include <QObject>
#include <cstdint>

namespace machine {

/**
 * Memory bus serves as last level of frontend memory and interconnects it with
 * backend memory devices, that are subscribed to given address range.
 *
 * Simulated core always has exactly one bus. This is necessary to access it
 * (e.g. from syscall) to map new devices. Backend memory device simulation
 * classes are implemented in `memory/backend`. For testing purposes,
 * `TrivialBus` is provided to wrap backend memory device into a minimal
 * frontend interface. Used mapping is always one to one with identity address
 * resolution. TrivialBus does not support external changes.
 * Backend memory devices subscribe to the bus via range descriptions (see
 * `RangeDecs`). Frontend address (`Address` type) is here converted using the
 * range descriptions to relative offset within the given backend memory device.
 * Downstream (frontend -> backend) communication is performed directly and
 * upstream communication is done via "external_change" signals.
 */
class MemoryDataBus : public FrontendMemory {
    Q_OBJECT

public:
    /**
     * @param simulated_endian  endian of the simulated CPU/memory system
     */
    explicit MemoryDataBus(Endian simulated_endian);

    ~MemoryDataBus() override;

    /**
     * Write method that repeats write single (for each affected range) until
     * whole size is written.
     *
     * @see MemoryDataBus::write_single
     */
    WriteResult write(
        Address destination,
        const void *source,
        size_t size,
        WriteOptions options) override;

    /**
     * Read method that repeats write single (for each affected range) until
     * whole size is read.
     *
     * @see MemoryDataBus:read_single
     */
    ReadResult read(
        void *destination,
        Address source,
        size_t size,
        ReadOptions options) const override;

    /**
     * Number of writes and external changes recorded.
     */
    uint32_t get_change_counter() const override;

    /**
     * Connect a backend device to the bus for given address range.
     *
     * @param device            simulated backend memory device object
     * @param start_addr        start of address range, where the device will be
     *                          accessible
     * @param last_addr         end of range, devices occupies
     * @param move_ownership    if true, bus will be responsible for for
     *                          device destruction
     *                          TODO: consider replace with a smartpointer
     * @return                  result of connection, it will fail if range is
     *                          already occupied
     */
    bool insert_device_to_range(
        BackendMemory *device,
        Address start_addr,
        Address last_addr,
        bool move_ownership);

    /**
     * Disconnect a device by a pointer to it.
     * Owned device will be deallocated.
     *
     * @param device    simulated backend memory device object
     * @return          result of disconnection, false if device not present
     */
    bool remove_device(BackendMemory *device);

    /**
     * Disconnect all devices in given range.
     *
     *
     * Owned devices will be deallocated.
     *
     * @param start_addr
     * @param last_addr
     */
    void clean_range(Address start_addr, Address last_addr);

    enum LocationStatus location_status(Address address) const override;

private slots:
    /**
     * Receive external changes in underlying memory devices.
     */
    void range_backend_external_change(
        const BackendMemory *device,
        Offset start_offset,
        Offset last_offset,
        AccessEffects type);

private:
    class RangeDesc; // See declaration bellow;
    QMultiMap<BackendMemory *, OWNED const RangeDesc *> ranges_by_device;
    /*
     * Ranges by address noes not own any value it hold. It can be erased at
     * once.
     */
    QMap<Address, const RangeDesc *> ranges_by_addr;
    mutable uint32_t change_counter = 0;

    /**
     * Helper to write into single range. Used by `write`.
     *
     * Write spanning multiple ranges will succeed partially and return size,
     * that was written.
     * API corresponds to `BackendMemory` interface method `write`.
     */
    WriteResult write_single(
        Address destination,
        const void *source,
        size_t size,
        WriteOptions options);

    /**
     * Helper to read from single range. Used by `read` from Backend memory
     * interface.
     *
     * Read spanning multiple ranges will succeed partially and return
     * size, that was written.
     * API corresponds to `BackendMemory` interface method `read`.
     */
    ReadResult read_single(
        void *destination,
        Address source,
        size_t size,
        ReadOptions options) const;

    /**
     * Get range (or nullptr) for arbitrary address (not just start or last).
     */
    const MemoryDataBus::RangeDesc *find_range(Address address) const;
};

/**
 * Describes a address range occupied by a single backend memory device.
 */
class MemoryDataBus::RangeDesc {
public:
    RangeDesc(
        BackendMemory *device,
        Address start_addr,
        Address last_addr,
        bool owns_device);

    /**
     * Tells, whether given address belongs to this range.
     */
    [[nodiscard]] bool contains(Address address) const;

    /*
     * Tells, whether this range (of the RangeDesc) overlaps with supplied
     * range.
     */
    [[nodiscard]] bool overlaps(Address start, Address last) const;

    BackendMemory *const device; // TODO consider a shared pointer
    const Address start_addr;
    const Address last_addr;
    const bool owns_device;
};

/**
 * Minimal frontend-backend wrapper.
 *
 * Redirects memory requests from frontend to backend one-to-one.
 * Used in test without cache (core only accepts `FrontendMemory`).
 *
 * Do not use in non-test code!
 */
class TrivialBus final : public FrontendMemory {
public:
    explicit TrivialBus(BackendMemory *backend_memory);

    WriteResult write(
        Address destination,
        const void *source,
        size_t size,
        WriteOptions options) override;

    ReadResult read(
        void *destination,
        Address source,
        size_t size,
        ReadOptions options) const override;

    uint32_t get_change_counter() const override;

private:
    BackendMemory *const device;
    mutable uint32_t change_counter = 0;
};

} // namespace machine
#endif // MEMORY_BUS_H
