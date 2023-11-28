#ifndef MEMORY_UTILS_H
#define MEMORY_UTILS_H

#include "common/endian.h"
#include "utils.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>

namespace machine {

/**
 * Determines what effects should memory access cause.
 */
enum class AccessEffects {
    REGULAR, //> All (memory, simulation counters, simulation flags, allocation
             // on read miss (write allocation is necessary)). For accessed
             // requested by simulated program.
    INTERNAL,       //> Only memory. Internal access performed for visualization,
                    // control and debugging.
    EXTERNAL_ASYNC, //> Used for DMA.
};

/**
 * Additional options for read operation between memory layers
 *
 *  The purpose for this struct is to make the API easily
 *   extensible.
 */
struct ReadOptions {
    AccessEffects type;
};

/**
 * Additional options for write operation between memory layers
 *
 *  The purpose for this struct is to make the API easily
 *   extensible.
 */
struct WriteOptions {
    AccessEffects type;
};

struct ReadResult {
    /**
     * Number of bytes successfully read.
     *
     * May be lower than requested size in case partial success
     *  like page fault.
     */
    size_t n_bytes = 0;

    inline ReadResult operator+(const ReadResult &other) const {
        return {
            this->n_bytes + other.n_bytes,
        };
    }

    inline void operator+=(const ReadResult &other) { this->n_bytes += other.n_bytes; }
};

struct WriteResult {
    /**
     * Number of bytes successfully read.
     *
     * May be lower than requested size in case partial success
     *  like page fault.
     */
    size_t n_bytes = 0;

    /**
     * Indicates whether write caused change in memory.
     * Used to reduce UI redraws.
     */
    bool changed = false;

    inline WriteResult operator+(const WriteResult &other) const {
        return {
            this->n_bytes + other.n_bytes,
            this->changed || other.changed,
        };
    }

    inline void operator+=(const WriteResult &other) {
        this->n_bytes += other.n_bytes;
        this->changed |= other.changed;
    }
};

/**
 * Perform n-byte read into periphery that only supports u32 access.
 *
 * When converting n-byte memory access into aligned series of discrete
 *  accesses each by u32.
 *
 * Example:
 * Periphery supports write by uint32_t. Access of size 4 targets in the middle
 *  of a uint32_t register. Then this function will return 2, which means that
 *  last 2 bytes of the retrieved data will be used (written to register).
 *
 * @tparam STORAGE_TYPE     a type periphery supports for access
 * @param ptr               pointer-like value used for access
 * @return                  size to be used from aligned access
 */
template<typename STORAGE_TYPE>
inline void partial_access_parameters(
        size_t &data_offset,
        size_t &partial_size,
        uintptr_t ptr,
        size_t size) {
    data_offset = ptr % sizeof(STORAGE_TYPE);
    partial_size = sizeof(STORAGE_TYPE);
    partial_size -= data_offset;
    if (partial_size > size)
        partial_size = size;
}

/**
 * Perform n-byte read into periphery that only supports u32 access.
 *
 * @tparam FUNC             function :: size_t -> uint32_t
 * @param src               data offset in periphery
 * @param dst               pointer to write to
 * @param size              bytes to read
 * @param data_getter       function object which return u32 data for given
 */
template<typename FUNC>
inline ReadResult read_by_u32(void *dst, size_t src, size_t size, FUNC data_getter) {
    size_t current_src = src;
    byte *current_dst = static_cast<byte *>(dst);
    size_t remaining_size = size;

    do {
        // For simplicity, this is duplicated in write_by_u32.
        size_t data_offset = current_src % sizeof(uint32_t);
        size_t partial_size = std::min(sizeof(uint32_t) - data_offset, remaining_size);

        uint32_t data = data_getter(current_src & ~3u);

        memcpy(current_dst, (byte *)&data + data_offset, partial_size);

        remaining_size -= partial_size;
        current_src += partial_size;
        current_dst += partial_size;
    } while (remaining_size > 0);

    return { .n_bytes = size };
}

template<typename FUNC>
inline ReadResult read_by_u16(void *dst, size_t src, size_t size, FUNC data_getter) {
    size_t current_src = src;
    byte *current_dst = static_cast<byte *>(dst);
    size_t remaining_size = size;

    do {
        // For simplicity, this is duplicated in write_by_u16.
        size_t data_offset = current_src % sizeof(uint16_t);
        size_t partial_size = std::min(sizeof(uint16_t) - data_offset, remaining_size);

        uint16_t data = data_getter(current_src & ~1u);

        memcpy(current_dst, (byte *)&data + data_offset, partial_size);

        remaining_size -= partial_size;
        current_src += partial_size;
        current_dst += partial_size;
    } while (remaining_size > 0);

    return { .n_bytes = size };
}

template<typename FUNC>
inline ReadResult read_by_u64(void *dst, size_t src, size_t size, FUNC data_getter) {
    size_t current_src = src;
    byte *current_dst = static_cast<byte *>(dst);
    size_t remaining_size = size;

    do {
        // For simplicity, this is duplicated in write_by_u64.
        size_t data_offset = current_src % sizeof(uint64_t);
        size_t partial_size = std::min(sizeof(uint64_t) - data_offset, remaining_size);

        uint64_t data = data_getter(current_src & ~7u);

        memcpy(current_dst, (byte *)&data + data_offset, partial_size);

        remaining_size -= partial_size;
        current_src += partial_size;
        current_dst += partial_size;
    } while (remaining_size > 0);

    return { .n_bytes = size };
}

/**
 * Perform n-byte write into periphery that only supports u32 access.
 *
 * @see read_by_u32
 *
 * @tparam FUNC1            function :: size_t -> uint32_t
 * @tparam FUNC2            function :: size_t, uint32_t -> bool
 * @param src               data source
 * @param dst               offset in periphery
 * @param size              n bytes
 * @param data_getter       function object which return u32 data for given
 *                           offset
 * @param data_setter       function object which writes an u32 to givem offset
 * @return                  true if write caused a change
 */

template<typename FUNC1, typename FUNC2>
inline WriteResult
write_by_u32(size_t dst, const void *src, size_t size, FUNC1 data_getter, FUNC2 data_setter) {
    const byte *current_src = static_cast<const byte *>(src);
    size_t current_dst = dst;
    size_t remaining_size = size;
    bool changed = false;

    do {
        // For simplicity, this is duplicated in read_by_u32.
        size_t data_offset = current_dst % sizeof(uint32_t);
        size_t partial_size = std::min(sizeof(uint32_t) - data_offset, remaining_size);
        uint32_t data = 0;

        if (partial_size < sizeof(data))
            data = data_getter(current_dst & ~3u);

        memcpy((byte *)&data + data_offset, current_src, partial_size);

        changed |= data_setter(current_dst & ~3u, data);

        remaining_size -= partial_size;
        current_src += partial_size;
        current_dst += partial_size;
    } while (remaining_size > 0);

    return { .n_bytes = size, .changed = changed };
}

template<typename FUNC1, typename FUNC2>
inline WriteResult
write_by_u16(size_t dst, const void *src, size_t size, FUNC1 data_getter, FUNC2 data_setter) {
    const byte *current_src = static_cast<const byte *>(src);
    size_t current_dst = dst;
    size_t remaining_size = size;
    bool changed = false;

    do {
        // For simplicity, this is duplicated in read_by_u16.
        size_t data_offset = current_dst % sizeof(uint16_t);
        size_t partial_size = std::min(sizeof(uint16_t) - data_offset, remaining_size);
        uint16_t data = 0;

        if (partial_size < sizeof(data))
            data = data_getter(current_dst & ~1u);

        memcpy((byte *)&data + data_offset, current_src, partial_size);

        changed |= data_setter(current_dst & ~1u, data);

        remaining_size -= partial_size;
        current_src += partial_size;
        current_dst += partial_size;
    } while (remaining_size > 0);

    return { .n_bytes = size, .changed = changed };
}

template<typename FUNC1, typename FUNC2>
inline WriteResult
write_by_u64(size_t dst, const void *src, size_t size, FUNC1 data_getter, FUNC2 data_setter) {
    const byte *current_src = static_cast<const byte *>(src);
    size_t current_dst = dst;
    size_t remaining_size = size;
    bool changed = false;

    do {
        // For simplicity, this is duplicated in read_by_u64.
        size_t data_offset = current_dst % sizeof(uint64_t);
        size_t partial_size = std::min(sizeof(uint64_t) - data_offset, remaining_size);
        uint64_t data = 0;

        if (partial_size < sizeof(data))
            data = data_getter(current_dst & ~7u);

        memcpy((byte *)&data + data_offset, current_src, partial_size);

        changed |= data_setter(current_dst & ~7u, data);

        remaining_size -= partial_size;
        current_src += partial_size;
        current_dst += partial_size;
    } while (remaining_size > 0);

    return { .n_bytes = size, .changed = changed };
}

/**
 * In case that underlying memory representation is fragmented, multiple
 * invocations of the same code might be needed. This is a common case with the
 * n-byte memory access API and therefore this function has been introduce to
 * minimize code duplication.
 *
 * @tparam FUNC         function with same signature as read or write
 * @tparam SRC_TYPE     corresponding type in read or write
 * @tparam DST_TYPE     corresponding type in read or write
 * @tparam OPTIONS_TYPE ReadOptions or WriteOptions
 * @tparam RESULT_TYPE  ReadResult or WriteResult
 * @param src           same arg as in read/write
 * @param dst           same arg as in read/write
 * @param size          same arg as in read/write
 * @param options       same arg as in read/write
 * @param function      lambda to perform individual accesses
 * @return number of bytes obtained, == size if fully successful
 */
template<typename RESULT_TYPE, typename FUNC, typename SRC_TYPE, typename DST_TYPE, typename OPTIONS_TYPE>
inline RESULT_TYPE repeat_access_until_completed(
    DST_TYPE dst,
    SRC_TYPE src,
    size_t size,
    OPTIONS_TYPE options,
    FUNC function) {
    size_t remaining_size = size;
    auto current_src = (uint64_t)(src);
    auto current_dst = (uint64_t)(dst);
    RESULT_TYPE total_result {};

    // do-while is preferred, because this loop is most likely to be executed only once. Empty
    // access is not common and does not need to be optimized.
    do {
        RESULT_TYPE result
            = function((DST_TYPE)(current_dst), (SRC_TYPE)(current_src), remaining_size, options);
        if (result.n_bytes == 0) break;
        total_result += result;
        current_src += result.n_bytes;
        current_dst += result.n_bytes;
        remaining_size -= result.n_bytes;
    } while (remaining_size > 0);

    return total_result;
}

/**
 * Helper function for memories, that do not support function like read_u32.
 * It is used in tests.
 * This is a generic version followed by named instantiations.
 */
template<typename T, typename MEM_T, typename ADDR_T>
T memory_read(MEM_T *mem, ADDR_T address) {
    T buffer;
    mem->read(&buffer, address, sizeof(T), {});
    return byteswap_if(buffer, mem->simulated_machine_endian != NATIVE_ENDIAN);
}

template<typename MEM_T, typename ADDR_T>
uint8_t memory_read_u8(MEM_T *mem, ADDR_T address) {
    return memory_read<uint8_t>(mem, address);
}

template<typename MEM_T, typename ADDR_T>
uint16_t memory_read_u16(MEM_T *mem, ADDR_T address) {
    return memory_read<uint16_t>(mem, address);
}

template<typename MEM_T, typename ADDR_T>
uint32_t memory_read_u32(MEM_T *mem, ADDR_T address) {
    return memory_read<uint32_t>(mem, address);
}

template<typename MEM_T, typename ADDR_T>
uint64_t memory_read_u64(MEM_T *mem, ADDR_T address) {
    return memory_read<uint64_t>(mem, address);
}

/**
 * Helper function for memories, that do not support function like read_u32.
 * It is used in tests.
 * This is a generic version followed by named instantiations.
 */
template<typename T, typename MEM_T, typename ADDR_T>
void memory_write(MEM_T *mem, ADDR_T address, T value) {
    const T swapped_value = byteswap_if(value, mem->simulated_machine_endian != NATIVE_ENDIAN);
    mem->write(address, &swapped_value, sizeof(T), {});
}

template<typename MEM_T, typename ADDR_T>
void memory_write_u8(MEM_T *mem, ADDR_T address, uint8_t value) {
    memory_write(mem, address, value);
}

template<typename MEM_T, typename ADDR_T>
void memory_write_u16(MEM_T *mem, ADDR_T address, uint16_t value) {
    memory_write(mem, address, value);
}

template<typename MEM_T, typename ADDR_T>
void memory_write_u32(MEM_T *mem, ADDR_T address, uint32_t value) {
    memory_write(mem, address, value);
}

template<typename MEM_T, typename ADDR_T>
void memory_write_u64(MEM_T *mem, ADDR_T address, uint64_t value) {
    memory_write(mem, address, value);
}

} // namespace machine

#endif // MEMORY_UTILS_H
