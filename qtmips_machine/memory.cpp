#include "memory.h"

using namespace machine;

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define SH_NTH_8(OFFSET) ((3 - ((OFFSET) & 0b11)) * 8)
#define SH_NTH_16(OFFSET) ((1 - ((OFFSET) & 0b10)) * 16)
#else
#define SH_NTH_8(OFFSET) (((OFFSET) & 0b11) * 8)
#define SH_NTH_16(OFFSET) (((OFFSET) & 0b10) * 16)
#endif

void MemoryAccess::write_byte(std::uint32_t offset, std::uint8_t value) {
    int nth = SH_NTH_8(offset);
    std::uint32_t mask = 0xff << nth; // Mask for n-th byte
    wword(offset, (rword(offset) & ~mask) | (((std::uint32_t)value << nth) & mask));
}

void MemoryAccess::write_hword(std::uint32_t offset, std::uint16_t value) {
    int nth = SH_NTH_16(offset);
    std::uint32_t mask = 0xffff << nth; // Mask for n-th half-word
    wword(offset, (rword(offset) & ~mask) | (((std::uint32_t)value << nth) & mask));
}

void MemoryAccess::write_word(std::uint32_t offset, std::uint32_t value) {
    wword(offset, value);
}

std::uint8_t MemoryAccess::read_byte(std::uint32_t offset) const {
    int nth = SH_NTH_8(offset);
    return (std::uint8_t)(rword(offset) >> nth);
}

std::uint16_t MemoryAccess::read_hword(std::uint32_t offset) const {
    int nth = SH_NTH_16(offset);
    return (std::uint16_t)(rword(offset) >> nth);
}

std::uint32_t MemoryAccess::read_word(std::uint32_t offset) const {
    return rword(offset);
}

void MemoryAccess::write_ctl(enum MemoryAccess::AccessControl ctl, std::uint32_t offset, std::uint32_t value) {
    switch (ctl) {
    case AC_NONE:
        break;
    case AC_BYTE:
    case AC_BYTE_UNSIGNED:
        this->write_byte(offset, (std::uint8_t) value);
        break;
    case AC_HALFWORD:
    case AC_HALFWORD_UNSIGNED:
        this->write_hword(offset, (std::uint16_t) value);
        break;
    case AC_WORD:
        this->write_word(offset, value);
        break;
    default:
        throw QTMIPS_EXCEPTION(UnknownMemoryControl, "Trying to write to memory with unknown ctl", QString::number(ctl));
    }
}

std::uint32_t MemoryAccess::read_ctl(enum MemoryAccess::AccessControl ctl, std::uint32_t offset) const {
    switch (ctl) {
    case AC_NONE:
        return 0;
    case AC_BYTE:
        {
        std::uint8_t b = this->read_byte(offset);
        return (((std::uint32_t)b & 0x80) << 24) | ((std::uint32_t)b & 0x7F); // Sign extend
        }
    case AC_HALFWORD:
        {
        std::uint16_t h = this->read_hword(offset);
        return (((std::uint32_t)h & 0x8000) << 16) | ((std::uint32_t)h & 0x7FFF); // Sign extend
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

void MemoryAccess::sync() { }

MemorySection::MemorySection(std::uint32_t length) {
    this->len = length;
    this->dt = new std::uint32_t[length];
    memset(this->dt, 0, sizeof *this->dt * length);
}

MemorySection::MemorySection(const MemorySection &ms) : MemorySection(ms.length()) {
    memcpy(this->dt, ms.data(), sizeof *this->dt * this->len);
}

MemorySection::~MemorySection() {
    delete this->dt;
}

void MemorySection::wword(std::uint32_t offset, std::uint32_t value) {
    offset = offset >> 2;
    if (offset >= this->len)
        throw QTMIPS_EXCEPTION(OutOfMemoryAccess, "Trying to write outside of the memory section", QString("Accessing using offset: ") + QString(offset));
    this->dt[offset] = value;
}

std::uint32_t MemorySection::rword(std::uint32_t offset) const {
    offset = offset >> 2;
    if (offset >= this->len)
        throw QTMIPS_EXCEPTION(OutOfMemoryAccess, "Trying to read outside of the memory section", QString("Accessing using offset: ") + QString(offset));
    return this->dt[offset];
}

std::uint32_t MemorySection::length() const {
    return len;
}

const std::uint32_t* MemorySection::data() const {
    return this->dt;
}

bool MemorySection::operator==(const MemorySection &ms) const {
    return ! memcmp(this->dt, ms.data(), sizeof *this->dt * this->len);
}

bool MemorySection::operator!=(const MemorySection &ms) const {
    return ! this->operator ==(ms);
}

//////////////////////////////////////////////////////////////////////////////
/// Some optimalization options
// How big memory sections will be in bits (2^6=64)
#define MEMORY_SECTION_BITS 6
// How big one row of lookup tree will be in bits (2^4=16)
#define MEMORY_TREE_BITS 4
//////////////////////////////////////////////////////////////////////////////
// Size of one section
#define MEMORY_SECTION_SIZE (1 << MEMORY_SECTION_BITS)
// Size of one memory row
#define MEMORY_TREE_ROW_SIZE (1 << MEMORY_TREE_BITS)
// Depth of tree
#define MEMORY_TREE_DEPTH ((30 - MEMORY_SECTION_BITS) / MEMORY_TREE_BITS)
// Just do some sanity checks
#if (MEMORY_SECTION_SIZE == 0)
#error Nonzero memory section size is required
#endif
#if (MEMORY_TREE_ROW_SIZE == 0)
#error Nonzero memory tree row size is required
#endif
#if (((30 - MEMORY_SECTION_BITS) % MEMORY_TREE_BITS) != 0)
#error Number of bits in tree row has to be exact division of available number of bits
#endif

// Macro to generate mask of given size with given righ offset
#define GENMASK(SIZE, OFF) (((1 << (SIZE)) - 1) << (OFF))
// Get index in row for fiven offset and row number i
#define TREE_ROW_BIT_OFFSET(I) (30 - MEMORY_TREE_BITS - (I)*MEMORY_TREE_BITS)
#define TREE_ROW(OFFSET, I) (((OFFSET) & GENMASK(MEMORY_TREE_BITS, TREE_ROW_BIT_OFFSET(I))) >> TREE_ROW_BIT_OFFSET(I))

Memory::Memory() {
    this->mt_root = allocate_section_tree();
}

Memory::Memory(const Memory &m) {
    this->mt_root = copy_section_tree(m.get_memorytree_root(), 0);
}

Memory::~Memory() {
    free_section_tree(this->mt_root, 0);
}

void Memory::reset() {
    free_section_tree(this->mt_root, 0);
    this->mt_root = allocate_section_tree();
}

void Memory::reset(const Memory &m) {
    free_section_tree(this->mt_root, 0);
    this->mt_root = copy_section_tree(m.get_memorytree_root(), 0);
}

MemorySection *Memory::get_section(std::uint32_t address, bool create) const {
    union MemoryTree *w = this->mt_root;
    size_t row_num;
    for (int i = 0; i < (MEMORY_TREE_DEPTH - 1); i++) {
        row_num = TREE_ROW(address, i);
        if (w[row_num].mt == nullptr) { // We don't have this tree so allocate it
            if (!create) // If we shouldn't be creating it than just return null
                return nullptr;
            w[row_num].mt = allocate_section_tree();
        }
        w = w[row_num].mt;
    }
    row_num = TREE_ROW(address, MEMORY_TREE_DEPTH - 1);
    if (w[row_num].sec == nullptr) {
        if (!create)
            return nullptr;
        w[row_num].sec = new MemorySection(MEMORY_SECTION_SIZE);
    }
    return w[row_num].sec;
}

#define SECTION_OFFSET_MASK(ADDR) (ADDR & GENMASK(MEMORY_SECTION_BITS, 2))

void Memory::wword(std::uint32_t address, std::uint32_t value) {
    MemorySection *section = this->get_section(address, true);
    section->write_word(SECTION_OFFSET_MASK(address), value);
}

std::uint32_t Memory::rword(std::uint32_t address) const {
    MemorySection *section = this->get_section(address, false);
    if (section == nullptr)
        return 0;
    else
        return section->read_word(SECTION_OFFSET_MASK(address));
}

bool Memory::operator==(const Memory&m) const {
    return compare_section_tree(this->mt_root, m.get_memorytree_root(), 0);
}

bool Memory::operator!=(const Memory&m) const {
    return ! this->operator ==(m);
}

const union machine::MemoryTree *Memory::get_memorytree_root() const {
    return this->mt_root;
}

std::uint32_t Memory::next_allocated(std::uint32_t address) const {
    address = address >> 2;
    // TODO
    return address;
}

std::uint32_t Memory::prev_allocated(std::uint32_t address) const {
    address = address >> 2;
    // TODO
    return address;
}

union machine::MemoryTree *Memory::allocate_section_tree() {
    union MemoryTree *mt = new union MemoryTree[MEMORY_TREE_ROW_SIZE];
    memset(mt, 0, sizeof *mt * MEMORY_TREE_ROW_SIZE);
    return mt;
}

void Memory::free_section_tree(union machine::MemoryTree *mt, size_t depth) {
    if (depth < (MEMORY_TREE_DEPTH - 1))  { // Following level is memory tree
        for (int i = 0; i < MEMORY_TREE_ROW_SIZE; i++) {
            if (mt[i].mt != nullptr)
                free_section_tree(mt[i].mt, depth + 1);
        }
    } else { // Following level is memory section
        for (int i = 0; i < MEMORY_TREE_ROW_SIZE; i++) {
            if (mt[i].sec != nullptr)
                delete mt[i].sec;
        }
    }
}

bool Memory::compare_section_tree(const union machine::MemoryTree *mt1, const union machine::MemoryTree *mt2, size_t depth) {
    if (depth < (MEMORY_TREE_DEPTH - 1))  { // Following level is memory tree
        for (int i = 0; i < MEMORY_TREE_ROW_SIZE; i++) {
            if (
                ((mt1[i].mt == nullptr || mt2[i].mt == nullptr) && 	mt1[i].mt != mt2[i].mt)
                    ||
                (mt1[i].mt != nullptr && mt2[i].mt != nullptr && !compare_section_tree(mt1[i].mt, mt2[i].mt, depth + 1))
               ) {
                return false;
            }
        }
    } else { // Following level is memory section
        for (int i = 0; i < MEMORY_TREE_ROW_SIZE; i++) {
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

union machine::MemoryTree *Memory::copy_section_tree(const union machine::MemoryTree *mt, size_t depth) {
    union MemoryTree *nmt = allocate_section_tree();
    if (depth < (MEMORY_TREE_DEPTH - 1))  { // Following level is memory tree
        for (int i = 0; i < MEMORY_TREE_ROW_SIZE; i++) {
            if (mt[i].mt != nullptr) {
                nmt[i].mt = copy_section_tree(mt[i].mt, depth + 1);
            }
        }
    } else { // Following level is memory section
        for (int i = 0; i < MEMORY_TREE_ROW_SIZE; i++) {
            if (mt[i].sec != nullptr)
                nmt[i].sec = new MemorySection(*mt[i].sec);
        }
    }
    return nmt;
}
