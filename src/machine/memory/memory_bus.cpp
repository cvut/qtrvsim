#include "memory/memory_bus.h"

#include "common/endian.h"
#include "memory/memory_utils.h"

using namespace machine;

MemoryDataBus::MemoryDataBus(Endian simulated_endian)
    : FrontendMemory(simulated_endian) {};

MemoryDataBus::~MemoryDataBus() {
    ranges_by_addr.clear(); // No stored values are owned.
    auto iter = ranges_by_device.begin();
    while (iter != ranges_by_device.end()) {
        const RangeDesc *range = iter.value();
        iter = ranges_by_device.erase(iter); // Advances the iterator.
        if (range->owns_device) {
            delete range->device;
        }
        delete range;
    }
}

WriteResult MemoryDataBus::write(
    Address destination,
    const void *source,
    size_t size,
    WriteOptions options) {
    return repeat_access_until_completed<WriteResult>(
        destination, source, size, options,
        [this](Address dst, const void *src, size_t s, WriteOptions opt)
            -> WriteResult { return write_single(dst, src, s, opt); });
}

WriteResult MemoryDataBus::write_single(
    Address destination,
    const void *source,
    size_t size,
    WriteOptions options) {
    const RangeDesc *range = find_range(Address(destination));
    if (range == nullptr) {
        // Write to unused address range - no devices it present.
        // This could produce a fault but for simplicity, we will
        // just ignore the write.
        return (WriteResult) { .n_bytes = 0, .changed = false };
    }
    WriteResult result = range->device->write(
        destination - range->start_addr, source, size, options);

    if (result.changed) {
        change_counter++;
    }

    return result;
}

ReadResult MemoryDataBus::read(
    void *destination,
    Address source,
    size_t size,
    ReadOptions options) const {
    return repeat_access_until_completed<ReadResult>(
        destination, source, size, options,
        [this](void *dst, Address src, size_t s, ReadOptions opt)
            -> ReadResult { return read_single(dst, src, s, opt); });
}

ReadResult MemoryDataBus::read_single(
    void *destination,
    Address source,
    size_t size,
    ReadOptions options) const {
    const RangeDesc *p_range = find_range(Address(source));
    if (p_range == nullptr) {
        // Write to unused address range, no devices it present.
        // This could produce a fault but for simplicity, we will
        // consider unused ranges to be constantly zero.
        memset(destination, 0, size);
        return (ReadResult) { .n_bytes = size };
    }

    return p_range->device->read(
        destination, source - p_range->start_addr, size, options);
}

uint32_t MemoryDataBus::get_change_counter() const {
    return change_counter;
}

enum LocationStatus MemoryDataBus::location_status(Address address) const {
    const RangeDesc *range = find_range(address);
    if (range == nullptr) {
        return LOCSTAT_ILLEGAL;
    }
    return range->device->location_status(address - range->start_addr);
}

const MemoryDataBus::RangeDesc *
MemoryDataBus::find_range(Address address) const {
    // lowerBound finds range what has highest key (which is range->last_addr)
    // less then or equal to address.
    // See comment in insert_device_to_range for description, why this works.
    auto iter = ranges_by_addr.lowerBound(address);
    if (iter == ranges_by_addr.end()) {
        return nullptr;
    }

    const RangeDesc *range = iter.value();
    if (address >= range->start_addr && address <= range->last_addr) {
        return range;
    }

    return nullptr;
}

bool MemoryDataBus::insert_device_to_range(
    BackendMemory *device,
    Address start_addr,
    Address last_addr,
    bool move_ownership) {
    auto iter = ranges_by_addr.lowerBound(start_addr);
    if (iter != ranges_by_addr.end()
        && iter.value()->overlaps(start_addr, last_addr)) {
        // Some part of requested range in already taken.
        return false;
    }
    auto *range = new RangeDesc(device, start_addr, last_addr, move_ownership);

    // Why are we using last address as key?
    //
    // QMap can return greatest lower key (lowerBound), so by indexing by last
    // address we can simply search any address within range. If searched
    // address is in given range, it is larger the previous range last address
    // and smaller or equal than the last address of its. This way we find the
    // last address of desired range in QMap red black tree and retrieve the
    // rang. Finally we just make sure, that the found range contains the
    // searched address for case that range is not present.
    ranges_by_addr.insert(last_addr, range);
    ranges_by_device.insert(device, range);
    connect(
        device, &BackendMemory::external_backend_change_notify, this,
        &MemoryDataBus::range_backend_external_change);
    return true;
}

bool MemoryDataBus::remove_device(BackendMemory *device) {
    const RangeDesc *range = ranges_by_device.take(device);
    if (range == nullptr) {
        return false; // Device not present.
    }

    ranges_by_addr.remove(range->last_addr);
    if (range->owns_device) {
        delete range->device;
    }
    delete range;

    return true;
}

void MemoryDataBus::clean_range(Address start_addr, Address last_addr) {
    for (auto iter = ranges_by_addr.lowerBound(start_addr);
         iter != ranges_by_addr.end(); iter++) {
        const RangeDesc *range = iter.value();
        if (range->start_addr <= last_addr) {
            remove_device(range->device);
        } else {
            break;
        }
    }
}

void MemoryDataBus::range_backend_external_change(
    const BackendMemory *device,
    Offset start_offset,
    Offset last_offset,
    AccessEffects type) {
    change_counter++;

    // We only use device here for lookup, so const_cast is safe as find takes
    // it by const reference .
    for (auto i = ranges_by_device.find(const_cast<BackendMemory *>(device));
         i != ranges_by_device.end(); i++) {
        const RangeDesc *range = i.value();
        emit external_change_notify(
            this, range->start_addr + start_offset,
            std::max(range->start_addr + last_offset, range->last_addr), type);
    }
}

MemoryDataBus::RangeDesc::RangeDesc(
    BackendMemory *device,
    Address start_addr,
    Address last_addr,
    bool owns_device)
    : device(device)
    , start_addr(start_addr)
    , last_addr(last_addr)
    , owns_device(owns_device) {}

bool MemoryDataBus::RangeDesc::contains(Address address) const {
    return start_addr <= address && address <= last_addr;
}

bool MemoryDataBus::RangeDesc::overlaps(Address start, Address last) const {
    return contains(start) || contains(last);
}

TrivialBus::TrivialBus(BackendMemory *backend_memory)
    : FrontendMemory(backend_memory->simulated_machine_endian)
    , device(backend_memory) {}

WriteResult TrivialBus::write(
    Address destination,
    const void *source,
    size_t size,
    WriteOptions options) {
    change_counter += 1; // Counter is mandatory by the frontend interface.
    return device->write(destination.get_raw(), source, size, options);
}

ReadResult TrivialBus::read(
    void *destination,
    Address source,
    size_t size,
    ReadOptions options) const {
    return device->read(destination, source.get_raw(), size, options);
}

uint32_t TrivialBus::get_change_counter() const {
    return change_counter;
}
