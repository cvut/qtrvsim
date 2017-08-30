#include "memory.h"

void MemoryAccess::write_hword(std::uint32_t offset, std::uint16_t value) {
    this->write_byte(offset++, (std::uint8_t)value);
    this->write_byte(offset, (std::uint8_t)(value >> 8));
}

void MemoryAccess::write_word(std::uint32_t offset, std::uint32_t value) {
    this->write_byte(offset++, (std::uint8_t)value);
    this->write_byte(offset++, (std::uint8_t)(value >> 8));
    this->write_byte(offset++, (std::uint8_t)(value >> 16));
    this->write_byte(offset++, (std::uint8_t)(value >> 24));
}

std::uint16_t MemoryAccess::read_hword(std::uint32_t offset) {
    std::uint16_t dt = 0;
    dt |= this->read_byte(offset++);
    dt |= (this->read_byte(offset) << 8);
    return dt;
}

std::uint32_t MemoryAccess::read_word(std::uint32_t offset) {
    std::uint32_t dt = 0;
    dt |= this->read_byte(offset++);
    dt |= (this->read_byte(offset++) << 8);
    dt |= (this->read_byte(offset++) << 16);
    dt |= (this->read_byte(offset) << 24);
    return dt;
}

MemorySection::MemorySection(std::uint32_t length) {
    this->length = length;
    this->dt = new std::uint8_t[length];
}

MemorySection::~MemorySection() {
    delete this->dt;
}

using namespace  std;

void MemorySection::write_byte(std::uint32_t offset, std::uint8_t value) {
    if (offset >= this->length)
        throw QTMIPS_EXCEPTION(OutOfMemoryAccess, "Trying to write outside of the memory section", std::string("Accessing using offset: ") + std::to_string(offset));
    this->dt[offset] = value;
}

std::uint8_t MemorySection::read_byte(std::uint32_t offset) {
    if (offset >= this->length)
        throw QTMIPS_EXCEPTION(OutOfMemoryAccess, "Trying to read outside of the memory section", std::string("Accessing using offset: ") + std::to_string(offset));
    return this->dt[offset];
}

// Number of bites per row on lookup tree
#define MEMORY_TREE_ROW ((32 - MEMORY_SECTION_BITS) / MEMORY_TREE_H)
// Size of row in memory lookup tree
#define MEMORY_TREE_LEN (1 << MEMORY_TREE_ROW)
// Just do some sanity checks
#if (MEMORY_TREE_LEN == 0)
#error Nonzero memory tree row size
#endif
#if (((32 - MEMORY_SECTION_BITS) % MEMORY_TREE_H) != 0)
#error Memory tree is not fully divisible by memory tree height
#endif
#if (MEMORY_TREE_H < 2)
#error Memory tree have to be higher or in limit equal to two
#endif

union MemoryTree {
    union MemoryTree *mt;
    MemorySection *sec;
};

Memory::Memory() {
    this->mt_root = allocate_section_tree();
}

Memory::~Memory() {
    // Free up memory tree
    // TODO
}

union MemoryTree *Memory::allocate_section_tree() {
    union MemoryTree *mt = new union MemoryTree[MEMORY_TREE_LEN];
    for (size_t i = 0; i < MEMORY_TREE_LEN; i++)
        // Note that this also nulls sec pointer as those are both pointers and so they have same size
        mt[i].mt = nullptr;
    return mt;
}

// Create address mask with section length
#define ADDRESS_MASK(LEN) ((1 << LEN) - 1)

// Get index in tree node from address, length of row and tree depth
// ADDR is expected to be and address with lowest bites removed (MEMORY_SECTION_BITS)
#define ADDRESS_TREE_INDEX(DEPTH, ADDR) ((ADDR >> (DEPTH * MEMORY_TREE_ROW)) & ADDRESS_MASK(MEMORY_TREE_ROW))


MemorySection *Memory::get_section(std::uint32_t address, bool create) {
    std::uint32_t addr = address >> MEMORY_SECTION_BITS; // drop all bits for addressing inside of the section
    union MemoryTree *w = this->mt_root;
    size_t ii;
    for (int i = 0; i < (MEMORY_TREE_H - 1); i++) {
        ii = ADDRESS_TREE_INDEX(i, addr);
        if (w[ii].mt == nullptr) { // We don't have this tree so allocate it
            if (!create) // If we shouldn't be creating it than just return null
                return nullptr;
            w[ii].mt = allocate_section_tree();
        }
        w = w[ii].mt;
    }
    // Now expand last level
    ii = ADDRESS_TREE_INDEX((MEMORY_TREE_H - 1), addr);
    if (w[ii].sec == nullptr) {
        if (!create)
            return nullptr;
        w[ii].sec = new MemorySection(1 << MEMORY_SECTION_BITS);
    }
    return w[ii].sec;
}

// Note about this address magic: we want to mask upper bits in address as those were used
// for section lookup. We do it using (2^BITS - 1).
#define SECTION_ADDRESS(ADDR) (ADDR & ADDRESS_MASK(MEMORY_SECTION_BITS))

void Memory::write_byte(std::uint32_t address, std::uint8_t value) {
    MemorySection *section = this->get_section(address, true);
    section->write_byte(SECTION_ADDRESS(address), value);
}

std::uint8_t Memory::read_byte(std::uint32_t address) {
    MemorySection *section = this->get_section(address, true);
    if (section == nullptr)
        return 0;
    else
        return section->read_byte(SECTION_ADDRESS(address));
}
