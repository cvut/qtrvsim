#include "memory.h"

using namespace machine;

// Note about endianness: Current memory implementation is expected to be a big endian.
// But we can be running on little endian so we should do conversion from bytes to word according that.

void MemoryAccess::write_hword(std::uint32_t offset, std::uint16_t value) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    this->write_byte(offset++, (std::uint8_t)(value >> 8));
    this->write_byte(offset, (std::uint8_t)value);
#else
    this->write_byte(offset++, (std::uint8_t)value);
    this->write_byte(offset, (std::uint8_t)(value >> 8));
#endif
}

void MemoryAccess::write_word(std::uint32_t offset, std::uint32_t value) {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    this->write_byte(offset++, (std::uint8_t)(value >> 24));
    this->write_byte(offset++, (std::uint8_t)(value >> 16));
    this->write_byte(offset++, (std::uint8_t)(value >> 8));
    this->write_byte(offset, (std::uint8_t)value);
#else
    this->write_byte(offset++, (std::uint8_t)value);
    this->write_byte(offset++, (std::uint8_t)(value >> 8));
    this->write_byte(offset++, (std::uint8_t)(value >> 16));
    this->write_byte(offset, (std::uint8_t)(value >> 24));
#endif
}

std::uint16_t MemoryAccess::read_hword(std::uint32_t offset) {
    std::uint16_t dt = 0;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    dt |= (this->read_byte(offset++) << 8);
    dt |= this->read_byte(offset);
#else
    dt |= this->read_byte(offset++);
    dt |= (this->read_byte(offset) << 8);
#endif
    return dt;
}

std::uint32_t MemoryAccess::read_word(std::uint32_t offset) {
    std::uint32_t dt = 0;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    dt |= (this->read_byte(offset++) << 24);
    dt |= (this->read_byte(offset++) << 16);
    dt |= (this->read_byte(offset++) << 8);
    dt |= this->read_byte(offset);
#else
    dt |= this->read_byte(offset++);
    dt |= (this->read_byte(offset++) << 8);
    dt |= (this->read_byte(offset++) << 16);
    dt |= (this->read_byte(offset) << 24);
#endif
    return dt;
}

void MemoryAccess::write_ctl(enum MemoryAccess::AccessControl ctl, std::uint32_t offset, std::uint32_t value) {
    switch (ctl) {
    case AC_NONE:
        break;
    case AC_BYTE:
    case AC_BYTE_UNSIGNED:
        this->write_byte(offset, value);
        break;
    case AC_HALFWORD:
    case AC_HALFWORD_UNSIGNED:
        this->write_hword(offset, value);
        break;
    case AC_WORD:
        this->write_word(offset, value);
        break;
    default:
        throw QTMIPS_EXCEPTION(UnknownMemoryControl, "Trying to write to memory with unknown ctl", QString::number(ctl));
    }
}

std::uint32_t MemoryAccess::read_ctl(enum MemoryAccess::AccessControl ctl, std::uint32_t offset) {
    switch (ctl) {
    case AC_NONE:
        return 0;
    case AC_BYTE:
        {
        std::uint8_t b = this->read_byte(offset);
        return ((b & 0x80) << 24) | (b & 0x7F); // Sign extend
        }
    case AC_HALFWORD:
        {
        std::uint16_t h = this->read_hword(offset);
        return ((h & 0x8000) << 16) | (h & 0x7FFF); // Sign extend
        }
    case AC_WORD:
        return this->read_word(offset);
    case AC_BYTE_UNSIGNED:
        return this->read_byte(offset);
    case AC_HALFWORD_UNSIGNED:
        return this->read_hword(offset);
    default:
        throw QTMIPS_EXCEPTION(UnknownMemoryControl, "Trying to read from memory with unknown ctl", QString::number(ctl));
    }
}

MemorySection::MemorySection(std::uint32_t length) {
    this->len = length;
    this->dt = new std::uint8_t[length];
    memset(this->dt, 0, sizeof *this->dt * length);
}

MemorySection::MemorySection(const MemorySection &ms) : MemorySection(ms.length()) {
    memcpy(this->dt, ms.data(), sizeof *this->dt * this->len);
}

MemorySection::~MemorySection() {
    delete this->dt;
}

void MemorySection::write_byte(std::uint32_t offset, std::uint8_t value) {
    if (offset >= this->len)
        throw QTMIPS_EXCEPTION(OutOfMemoryAccess, "Trying to write outside of the memory section", QString("Accessing using offset: ") + QString(offset));
    this->dt[offset] = value;
}

std::uint8_t MemorySection::read_byte(std::uint32_t offset) const {
    if (offset >= this->len)
        throw QTMIPS_EXCEPTION(OutOfMemoryAccess, "Trying to read outside of the memory section", QString("Accessing using offset: ") + QString(offset));
    return this->dt[offset];
}

std::uint32_t MemorySection::length() const {
    return len;
}

const std::uint8_t* MemorySection::data() const {
    return this->dt;
}

bool MemorySection::operator==(const MemorySection &ms) const {
    return ! memcmp(this->dt, ms.data(), sizeof *this->dt * this->len);
}

bool MemorySection::operator!=(const MemorySection &ms) const {
    return ! this->operator ==(ms);
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

namespace machine {
union MemoryTree {
    union MemoryTree *mt;
    MemorySection *sec;
};
}

Memory::Memory() {
    this->mt_root = allocate_section_tree();
}

Memory::Memory(const Memory &m) {
    this->mt_root = copy_section_tree(m.get_memorytree_root(), 0);
}

Memory::~Memory() {
    free_section_tree(this->mt_root, 0);
}

// Create address mask with section length
#define ADDRESS_MASK(LEN) ((1 << LEN) - 1)

// Get index in tree node from address, length of row and tree depth
// ADDR is expected to be and address with lowest bites removed (MEMORY_SECTION_BITS)
#define ADDRESS_TREE_INDEX(DEPTH, ADDR) ((ADDR >> (DEPTH * MEMORY_TREE_ROW)) & ADDRESS_MASK(MEMORY_TREE_ROW))


MemorySection *Memory::get_section(std::uint32_t address, bool create) const {
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

std::uint8_t Memory::read_byte(std::uint32_t address) const {
    MemorySection *section = this->get_section(address, false);
    if (section == nullptr)
        return 0;
    else
        return section->read_byte(SECTION_ADDRESS(address));
}

bool Memory::operator==(const Memory&m) const {
    return compare_section_tree(this->mt_root, m.get_memorytree_root(), 0);
}

bool Memory::operator!=(const Memory&m) const {
    return ! this->operator ==(m);
}

const union MemoryTree *Memory::get_memorytree_root() const {
    return this->mt_root;
}

union MemoryTree *Memory::allocate_section_tree() {
    union MemoryTree *mt = new union MemoryTree[MEMORY_TREE_LEN];
    for (size_t i = 0; i < MEMORY_TREE_LEN; i++)
        // Note that this also nulls sec pointer as those are both pointers and so they have same size
        mt[i].mt = nullptr;
    return mt;
}

void Memory::free_section_tree(union MemoryTree *mt, size_t depth) {
    if (depth < (MEMORY_TREE_H - 1))  { // Following level is memory tree
        for (int i = 0; i < MEMORY_TREE_LEN; i++) {
            if (mt[i].mt != nullptr)
                free_section_tree(mt[i].mt, depth + 1);
        }
    } else { // Following level is memory section
        for (int i = 0; i < MEMORY_TREE_LEN; i++) {
            if (mt[i].sec != nullptr)
                delete mt[i].sec;
        }
    }
}

bool Memory::compare_section_tree(const union MemoryTree *mt1, const union MemoryTree *mt2, size_t depth) {
    if (depth < (MEMORY_TREE_H - 1))  { // Following level is memory tree
        for (int i = 0; i < MEMORY_TREE_LEN; i++) {
            if (
                ((mt1[i].mt == nullptr || mt2[i].mt == nullptr) && 	mt1[i].mt != mt2[i].mt)
                    ||
                (mt1[i].mt != nullptr && mt2[i].mt != nullptr && !compare_section_tree(mt1[i].mt, mt2[i].mt, depth + 1))
               ) {
                return false;
            }
        }
    } else { // Following level is memory section
        for (int i = 0; i < MEMORY_TREE_LEN; i++) {
            if (
                ((mt1[i].sec == nullptr || mt2[i].sec == nullptr) && mt1[i].sec != mt2[i].sec)
                    ||
                (mt1[i].sec != nullptr && mt2[i].sec != nullptr && *mt1[i].sec != *mt2[i].sec)
               ) {
                return false;
            }
        }
    }
    return true;
}

union MemoryTree *Memory::copy_section_tree(const union MemoryTree *mt, size_t depth) {
    union MemoryTree *nmt = allocate_section_tree();
    if (depth < (MEMORY_TREE_H - 1))  { // Following level is memory tree
        for (int i = 0; i < MEMORY_TREE_LEN; i++) {
            if (mt[i].mt != nullptr) {
                nmt[i].mt = copy_section_tree(mt[i].mt, depth + 1);
            }
        }
    } else { // Following level is memory section
        for (int i = 0; i < MEMORY_TREE_LEN; i++) {
            if (mt[i].sec != nullptr)
                nmt[i].sec = new MemorySection(*mt[i].sec);
        }
    }
    return nmt;
}
