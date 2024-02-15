#ifndef MACHINE_MEMORY_H
#define MACHINE_MEMORY_H

#include "common/endian.h"
#include "machinedefs.h"
#include "memory/address.h"
#include "memory/backend/backend_memory.h"
#include "memory/memory_utils.h"
#include "simulator_exception.h"
#include "utils.h"

#include <QObject>
#include <cstdint>

namespace machine {

/**
 * NOTE: Internal endian of memory must be the same as endian of the whole
 * simulated machine. Therefore it does not have internal_endian field.
 */
class MemorySection final : public BackendMemory {
public:
    explicit MemorySection(size_t length_bytes, Endian simulated_machine_endian);
    MemorySection(const MemorySection &other);
    ~MemorySection() override = default;

    WriteResult write(
        Offset destination,
        const void *source,
        size_t total_size,
        WriteOptions options) override;

    ReadResult read(
        void *destination,
        Offset source,
        size_t size,
        ReadOptions options) const override;

    [[nodiscard]] LocationStatus location_status(Offset offset) const override;

    [[nodiscard]] size_t length() const;
    [[nodiscard]] const byte *data() const;

    bool operator==(const MemorySection &) const;
    bool operator!=(const MemorySection &) const;

private:
    std::vector<byte> dt;
};

//////////////////////////////////////////////////////////////////////////////
/// Some optimisation options
// How big memory sections will be in bits (2^8=256 bytes)
constexpr size_t MEMORY_SECTION_BITS = 8;
// How big one row of lookup tree will be in bits (2^4=16)
constexpr size_t MEMORY_TREE_BITS = 4;
//////////////////////////////////////////////////////////////////////////////
// Size of one section
constexpr size_t MEMORY_SECTION_SIZE = (1u << MEMORY_SECTION_BITS);
// Size of one memory row
constexpr size_t MEMORY_TREE_ROW_SIZE = (1u << MEMORY_TREE_BITS);
// Depth of tree
constexpr size_t MEMORY_TREE_DEPTH
    = ((32 - MEMORY_SECTION_BITS) / MEMORY_TREE_BITS);

union MemoryTree {
    union MemoryTree *subtree;
    MemorySection *sec;
};

/**
 * NOTE: Internal endian of memory must be the same as endian of the whole
 * simulated machine. Therefore it does not have internal_endian field.
 */
class Memory final : public BackendMemory {
    Q_OBJECT
public:
    // This is dummy constructor for qt internal uses only.
    Memory();
    explicit Memory(Endian simulated_machine_endian);
    Memory(const Memory &);
    ~Memory() override;
    void reset(); // Reset whole content of memory (removes old tree and creates
                  // new one)
    void reset(const Memory &);

    // returns section containing given address
    [[nodiscard]] MemorySection *get_section(size_t offset, bool create) const;

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

    bool operator==(const Memory &) const;
    bool operator!=(const Memory &) const;

    [[nodiscard]] const union MemoryTree *get_memory_tree_root() const;

private:
    union MemoryTree *mt_root;
    uint32_t change_counter = 0;
    static union MemoryTree *allocate_section_tree();
    static void free_section_tree(union MemoryTree *, size_t depth);
    static bool compare_section_tree(
        const union MemoryTree *,
        const union MemoryTree *,
        size_t depth);
    static union MemoryTree *
    copy_section_tree(const union MemoryTree *, size_t depth);
    [[nodiscard]] uint32_t get_change_counter() const;
};
} // namespace machine

Q_DECLARE_METATYPE(machine::Memory);

#endif // MEMORY_H
