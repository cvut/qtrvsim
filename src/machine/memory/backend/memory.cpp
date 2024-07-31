#include "memory/backend/memory.h"

#include "common/endian.h"
#include "simulator_exception.h"

#include <memory>

namespace machine {

MemorySection::MemorySection(
    size_t length_bytes,
    Endian simulated_machine_endian)
    : BackendMemory(simulated_machine_endian)
    , dt(length_bytes, 0) {}

MemorySection::MemorySection(const MemorySection &other)
    : BackendMemory(other.simulated_machine_endian)
    , dt(other.dt) {}

WriteResult MemorySection::write(Offset dst_offset, const void *source, size_t size, WriteOptions options) {
    UNUSED(options)

    auto destination = static_cast<size_t>(dst_offset);

    if (destination >= length()) {
        throw SIMULATOR_EXCEPTION(
            OutOfMemoryAccess, "Trying to write outside of the memory section",
            QString("Accessing using offset: ") + QString::number(destination));
    }

    // Size the can be read from this section
    const size_t available_size = std::min(destination + size, length()) - destination;

    // TODO, make swap conditional for big endian machines
    bool changed = memcmp(source, &dt[destination], available_size) != 0;
    if (changed) {
        memcpy(&dt[destination], source, available_size);
    }

    return { .n_bytes = available_size, .changed = changed };
}

ReadResult MemorySection::read(void *destination, Offset src_offset, size_t size, ReadOptions options) const {
    UNUSED(options)

    auto source = static_cast<size_t>(src_offset);

    size = std::min(source + size, length()) - source;

    if (source >= length()) {
        throw SIMULATOR_EXCEPTION(
            OutOfMemoryAccess, "Trying to read outside of the memory section",
            QString("Accessing using offset: ") + QString::number(source));
    }

    memcpy(destination, &dt[source], size);

    return { .n_bytes = size };
}

LocationStatus MemorySection::location_status(Offset offset) const {
    UNUSED(offset)
    return LOCSTAT_NONE;
}

size_t MemorySection::length() const {
    return this->dt.size();
}

const byte *MemorySection::data() const {
    return this->dt.data();
}

bool MemorySection::operator==(const MemorySection &other) const {
    return this->dt == other.dt;
}

bool MemorySection::operator!=(const MemorySection &ms) const {
    return !this->operator==(ms);
}

// Settings sanity checks
static_assert(
    MEMORY_SECTION_SIZE != 0,
    "Nonzero memory section size is required.");
static_assert(
    MEMORY_TREE_ROW_SIZE != 0,
    "Nonzero memory tree row size is required.");
static_assert(
    ((32 - MEMORY_SECTION_BITS) % MEMORY_TREE_BITS) == 0,
    "Number of bits in tree row has to be exact division of available number "
    "of bits.");

/**
 * Generate mask to get memory section index from address.
 *
 * Memory section of section_size 2^`section_size` separably addressable units
 * each of section_size 2^`unit_size` bytes.
 *
 * Example:
 *  `MemorySection` of 256x1B index is received as
 *  ```address & generate_mask(8, 0)```
 */
constexpr uint64_t generate_mask(size_t section_size, size_t unit_size) {
    return ((1ULL << section_size) - 1) << unit_size;
}

/**
 * Get index in row for given offset and row number i
 */
constexpr uint64_t tree_row_bit_offset(size_t i, size_t xlen) {
    return xlen - MEMORY_TREE_BITS - i * MEMORY_TREE_BITS;
}

/*
 * Select branch index from memory tree.
 */
constexpr uint64_t get_tree_row(uint64_t offset, size_t i, size_t xlen) {
    return (offset & generate_mask(MEMORY_TREE_BITS, tree_row_bit_offset(i, xlen)))
           >> tree_row_bit_offset(i, xlen);
}

Memory::Memory() : BackendMemory(BIG) {
    // This is dummy constructor for qt internal uses only.
    this->mt_root = nullptr;
}

Memory::Memory(Endian simulated_machine_endian)
    : BackendMemory(simulated_machine_endian) {
    this->mt_root = allocate_section_tree();
    this->xlen = 32;
}

Memory::Memory(Endian simulated_machine_endian, size_t xlen)
    : BackendMemory(simulated_machine_endian) {
    this->mt_root = allocate_section_tree();
    this->xlen = xlen;
}

Memory::Memory(const Memory &other)
    : BackendMemory(other.simulated_machine_endian) {
    this->xlen = other.xlen;
    this->mt_root = copy_section_tree(other.get_memory_tree_root(), 0, ((this->xlen - MEMORY_SECTION_BITS) / MEMORY_TREE_BITS));
}

Memory::~Memory() {
    free_section_tree(this->mt_root, 0, ((this->xlen - MEMORY_SECTION_BITS) / MEMORY_TREE_BITS));
    delete[] this->mt_root;
}

void Memory::reset() {
    free_section_tree(this->mt_root, 0, ((this->xlen - MEMORY_SECTION_BITS) / MEMORY_TREE_BITS));
    delete[] this->mt_root;
    this->mt_root = allocate_section_tree();
}

void Memory::reset(const Memory &m) {
    free_section_tree(this->mt_root, 0, ((this->xlen - MEMORY_SECTION_BITS) / MEMORY_TREE_BITS));
    if(this->xlen > m.xlen) {
        union MemoryTree *cmt = allocate_section_tree();
        const union MemoryTree *mt = m.get_memory_tree_root();
        this->xlen = m.xlen;
        for(size_t i = 0; i < MEMORY_TREE_ROW_SIZE; i++) {
            if (mt[i].subtree != nullptr) {
                size_t index = i >> 3 ? 15 : 0;
                union MemoryTree *temp = cmt;
                for (size_t j = 0; j < 8; j++, temp = temp[index].subtree) {
                    if (temp[index].subtree == nullptr) {
                        temp[index].subtree = allocate_section_tree();
                    }
                }
                temp[i].subtree = copy_section_tree(mt[i].subtree, 1, ((this->xlen - MEMORY_SECTION_BITS) / MEMORY_TREE_BITS));
            }
        }
        delete[] this->mt_root;
        this->mt_root = cmt;
        this->xlen = 64;
    }
    else if (this->xlen < m.xlen) {
        delete[] this->mt_root;
        this->mt_root = allocate_section_tree();
    }
    else
        this->mt_root = copy_section_tree(m.get_memory_tree_root(), 0, ((this->xlen - MEMORY_SECTION_BITS) / MEMORY_TREE_BITS));

}

MemorySection *Memory::get_section(size_t offset, bool create) const {
    union MemoryTree *w = this->mt_root;
    size_t row_num;
    // Walk memory tree branch from root to leaf and create new nodes when
    // needed and requested (`create` flag).
    size_t memory_tree_depth = ((this->xlen - MEMORY_SECTION_BITS) / MEMORY_TREE_BITS);
    for (size_t i = 0; i < (memory_tree_depth - 1); i++) {
        row_num = get_tree_row(offset, i, this->xlen);
        if (w[row_num].subtree == nullptr) {
            // We don't have this tree so allocate it.
            if (!create) {
                // If we shouldn't be creating it than just return// null.
                return nullptr;
            }
            w[row_num].subtree = allocate_section_tree();
        }
        w = w[row_num].subtree;
    }
    row_num = get_tree_row(offset, memory_tree_depth - 1, this->xlen);
    if (w[row_num].sec == nullptr) {
        if (!create) {
            return nullptr;
        }
        w[row_num].sec
            = new MemorySection(MEMORY_SECTION_SIZE, simulated_machine_endian);
    }
    return w[row_num].sec;
}

size_t get_section_offset_mask(size_t addr) {
    return addr & generate_mask(MEMORY_SECTION_BITS, 0);
}

WriteResult Memory::write(
    Offset destination,
    const void *source,
    size_t size,
    WriteOptions options) {
    return repeat_access_until_completed<WriteResult>(
        destination, source, size, options,
        [this](
            Offset _destination, const void *_source, size_t _size,
            WriteOptions) {
            MemorySection *section = this->get_section(_destination, true);
            return section->write(
                get_section_offset_mask(_destination), _source, _size, {});
        });
}

ReadResult Memory::read(
    void *destination,
    Offset source,
    size_t size,
    ReadOptions options) const {
    return repeat_access_until_completed<ReadResult>(
        destination, source, size, options,
        [this](
            void *_destination, Offset _source, size_t _size,
            ReadOptions _options) -> ReadResult {
            MemorySection *section = this->get_section(_source, false);
            if (section == nullptr) {
                memset(_destination, 0, _size);
                // TODO Warning read of uninitialized memory
                return { .n_bytes = _size };
            } else {
                return section->read(
                    _destination, get_section_offset_mask(_source), _size,
                    _options);
            }
        });
}

uint32_t Memory::get_change_counter() const {
    return change_counter;
}

bool Memory::operator==(const Memory &m) const {
    return compare_section_tree(this->mt_root, m.get_memory_tree_root(), 0 , ((this->xlen - MEMORY_SECTION_BITS) / MEMORY_TREE_BITS));
}

bool Memory::operator!=(const Memory &m) const {
    return !this->operator==(m);
}

const union machine::MemoryTree *Memory::get_memory_tree_root() const {
    return this->mt_root;
}

union machine::MemoryTree *Memory::allocate_section_tree() {
    auto *mt = new union MemoryTree[MEMORY_TREE_ROW_SIZE];
    memset(mt, 0, sizeof *mt * MEMORY_TREE_ROW_SIZE);
    return mt;
}

void Memory::free_section_tree(union MemoryTree *mt, size_t depth, size_t memory_tree_depth) {
    if (depth < (memory_tree_depth - 1)) { // Following level is memory tree
        for (size_t i = 0; i < MEMORY_TREE_ROW_SIZE; i++) {
            if (mt[i].subtree != nullptr) {
                free_section_tree(mt[i].subtree, depth + 1, memory_tree_depth);
                delete[] mt[i].subtree;
            }
        }
    } else { // Following level is memory section
        for (size_t i = 0; i < MEMORY_TREE_ROW_SIZE; i++) {
            delete mt[i].sec;
        }
    }
}

bool Memory::compare_section_tree(
    const union MemoryTree *mt1,
    const union MemoryTree *mt2,
    size_t depth,
    size_t memory_tree_depth) { 
    if (depth < (memory_tree_depth - 1)) { // Following level is memory tree
        for (size_t i = 0; i < MEMORY_TREE_ROW_SIZE; i++) {
            if (((mt1[i].subtree == nullptr || mt2[i].subtree == nullptr)
                 && mt1[i].subtree != mt2[i].subtree)
                || (mt1[i].subtree != nullptr && mt2[i].subtree != nullptr
                    && !compare_section_tree(
                        mt1[i].subtree, mt2[i].subtree, depth + 1, memory_tree_depth))) {
                return false;
            }
        }
    } else { // Following level is memory section
        for (size_t i = 0; i < MEMORY_TREE_ROW_SIZE; i++) {
            if (((mt1[i].sec == nullptr || mt2[i].sec == nullptr)
                 && mt1[i].sec != mt2[i].sec)
                || (mt1[i].sec != nullptr && mt2[i].sec != nullptr
                    && *mt1[i].sec != *mt2[i].sec)) {
                return false;
            }
        }
    }
    return true;
}

union machine::MemoryTree *
Memory::copy_section_tree(const union MemoryTree *mt, size_t depth, size_t memory_tree_depth) {
    union MemoryTree *nmt = allocate_section_tree();
    if (depth < (memory_tree_depth - 1)) { // Following level is memory tree
        for (size_t i = 0; i < MEMORY_TREE_ROW_SIZE; i++) {
            if (mt[i].subtree != nullptr) {
                nmt[i].subtree = copy_section_tree(mt[i].subtree, depth + 1, memory_tree_depth);
            }
        }
    } else { // Following level is memory section
        for (size_t i = 0; i < MEMORY_TREE_ROW_SIZE; i++) {
            if (mt[i].sec != nullptr) {
                nmt[i].sec = new MemorySection(*mt[i].sec);
            }
        }
    }
    return nmt;
}
LocationStatus Memory::location_status(Offset offset) const {
    UNUSED(offset)
    // Lazy allocation of memory is only internal implementation detail.
    return LOCSTAT_NONE;
}

} // namespace machine
