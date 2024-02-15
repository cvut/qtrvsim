#ifndef FRONTEND_MEMORY_H
#define FRONTEND_MEMORY_H

#include "common/endian.h"
#include "machinedefs.h"
#include "memory/address.h"
#include "memory/memory_utils.h"
#include "register_value.h"
#include "simulator_exception.h"

#include <QObject>
#include <cstdint>

// Shortcut for enum class values, type is obvious from context.
using ae = machine::AccessEffects;

namespace machine {

/**
 * # What is frontend memory
 *
 * The core is the entry point to the whole simulated machine. Therefore the
 * reworked concepts can be best understood from its point of view on the memory
 * subsystem. Every core configuration has three mandatory memory components.
 * Two of them  are the entry points for memory operations for program and data,
 * respectively. The entry point starts a chain of memory components of
 * arbitrary length, each backed by the next one. A component that receives a
 * request either resolves it or invokes its lower component. For the core, the
 * chain is invisible, and it only interfaces with the top-level component. The
 * chain is terminated by a memory data bus. Data bus for data memory is the
 * third component I have mentioned.  All of the above components are instances
 * of the frontend memory. Frontend memory has three characteristics. Except for
 * the memory bus, each frontend component must have a frontend component
 * backing it. The memory bus is backed by zero or more backed devices
 * (explained later). All addresses used in the frontend are full memory
 * address. By the word "full", I mean the numerical value observable by a C
 * program (up to address translation), in contrast to a local relative offset.
 * The Address datatype is used to store it and pass as an argument.
 * Finally, frontend memory instances are likely those found on the CPU chip
 * itself (caches, TLB...).
 */
class FrontendMemory : public QObject {
    Q_OBJECT
public:
    /**
     * It is necessary to know the endian of the whole simulation in the
     * entrypoint on the memory subsystem, which might be any instance of
     * frontend memory.
     *
     * @param simulated_endian  endian of the simulated CPU/memory subsystem
     */
    explicit FrontendMemory(Endian simulated_endian);

    bool
    write_u8(Address address, uint8_t value, AccessEffects type = ae::REGULAR);
    bool
    write_u16(Address address, uint16_t value, AccessEffects type = ae::REGULAR);
    bool
    write_u32(Address address, uint32_t value, AccessEffects type = ae::REGULAR);
    bool
    write_u64(Address address, uint64_t value, AccessEffects type = ae::REGULAR);

    [[nodiscard]] uint8_t read_u8(Address address, AccessEffects type = ae::REGULAR) const;
    [[nodiscard]] uint16_t read_u16(Address address, AccessEffects type = ae::REGULAR) const;
    [[nodiscard]] uint32_t read_u32(Address address, AccessEffects type = ae::REGULAR) const;
    [[nodiscard]] uint64_t read_u64(Address address, AccessEffects type = ae::REGULAR) const;

    /**
     * Store with size specified by the CPU control unit.
     *
     * This is for CPU core only and the AccessEffects type is implicitly
     * REGULAR.
     * @param control_signal    CPU control unit signal
     */
    void write_ctl(
        AccessControl control_signal,
        Address destination,
        RegisterValue value);

    /**
     * Read with size specified by the CPU control unit.
     *
     * This is for CPU core only and the AccessEffects type is implicitly
     * ae::REGULAR.
     * @param control_signal    CPU control unit signal
     */
    [[nodiscard]] RegisterValue read_ctl(enum AccessControl ctl, Address source) const;

    virtual void sync();
    [[nodiscard]] virtual LocationStatus location_status(Address address) const;
    [[nodiscard]] virtual uint32_t get_change_counter() const = 0;

    /**
     * Write byte sequence to memory
     *
     * @param source        pointer to array of bytes to be copied
     * @param destination   emulated address of destination to write to
     * @param size         number of bytes to be written
     * @return              true when memory before and after write differs
     */
    virtual WriteResult write(
        Address destination,
        const void *source,
        size_t size,
        WriteOptions options)
        = 0;

    /**
     * Read sequence of bytes from memory
     *
     * @param source        emulated address of data to be read
     * @param destination   pointer to destination buffer
     * @param size          number of bytes to be read
     * @param options       additional option like debug mode, see type
     *                      definition
     */
    virtual ReadResult read(
        void *destination,
        Address source,
        size_t size,
        ReadOptions options) const = 0;

    /**
     * Endian of the simulated CPU/memory system.
     *
     * Used when frontend_memory subclass instance is used as entrypoint to the
     * memory system (e.g. `read_XX` and `write_XX` functions.).
     * @see `convert_endian` in `memory/endian.h`.
     */
    const Endian simulated_machine_endian;

signals:
    /**
     * Signal used to propagate a change up through the hierarchy.
     */
    void external_change_notify(
        const FrontendMemory *issuing_memory,
        Address start_addr,
        Address last_addr,
        AccessEffects type) const;

private:
    /**
     * Read any type from memory
     *
     * This function was introduced to make stupid errors
     *  like mismatched type and and its size impossible.
     * It provides nicer interface than `read`.
     *  by eliminating the need for buffer.
     *
     * @tparam T            type value should be read as
     * @param address       emulated address to read from
     * @param type          read by visualization etc (type ae::INTERNAL) should
     *                      not cause certain effects (counter increments...)
     * @return              requested data with type T
     */
    template<typename T>
    T read_generic(Address address, AccessEffects type) const;

    /**
     * Write to any type from memory
     *
     * This function was introduced to make stupid errors like mismatched
     * type and and its size impossible. It provides nicer interface than
     * `write` by eliminating the need for buffer.
     *
     * @tparam T            type of value to be written
     * @param address       emulated address to write to
     * @param value         value of type T to be written
     * @param type          read by visualization etc (type ae::INTERNAL).
     * should not cause certain effects (counter increments...)
     * @return              true when memory before and after write differs
     */
    template<typename T>
    bool write_generic(Address address, T value, AccessEffects type);
};

} // namespace machine

#endif // FRONTEND_MEMORY_H
