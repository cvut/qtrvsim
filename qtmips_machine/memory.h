#ifndef MEMORY_H
#define MEMORY_H

#include <QObject>
#include <vector>
#include <cstdint>
#include "qtmipsexception.h"

// Virtual class for common memory access
class MemoryAccess : public QObject {
    Q_OBJECT
public:
    virtual void write_byte(std::uint32_t offset, std::uint8_t value) = 0;
    void write_hword(std::uint32_t offset, std::uint16_t value);
    void write_word(std::uint32_t offset, std::uint32_t value);

    virtual std::uint8_t read_byte(std::uint32_t offset) const = 0;
    std::uint16_t read_hword(std::uint32_t offset);
    std::uint32_t read_word(std::uint32_t offset);

    enum AccessControl {
        AC_BYTE,
        AC_HALFWORD,
        AC_WORD,
        AC_BYTE_UNSIGNED,
        AC_HALFWORD_UNSIGNED
    };
    void write_ctl(enum AccessControl ctl, std::uint32_t offset, std::uint32_t value);
    std::uint32_t read_ctl(enum AccessControl ctl, std::uint32_t offset);

signals:
    // TODO trigger
    void byte_change(std::uint32_t address, std::uint32_t value);
};

Q_DECLARE_METATYPE(MemoryAccess::AccessControl)

class MemorySection : public MemoryAccess {
public:
    MemorySection(std::uint32_t length);
    MemorySection(const MemorySection&);
    ~MemorySection();

    void write_byte(std::uint32_t offset, std::uint8_t value);
    std::uint8_t read_byte(std::uint32_t offset) const;

    std::uint32_t length() const;
    const std::uint8_t* data() const;

    bool operator==(const MemorySection&) const;
    bool operator!=(const MemorySection&) const;

private:
    std::uint32_t len;
    std::uint8_t *dt;
};

//////////////////////////////////////////////////////////////////////////////
/// Some optimalization options
// How big memory sections will be (2^8=256)
#define MEMORY_SECTION_BITS 8
// How deep memory lookup tree will be
#define MEMORY_TREE_H 4
//////////////////////////////////////////////////////////////////////////////

union MemoryTree;

class Memory : public MemoryAccess {
    Q_OBJECT
public:
    Memory();
    Memory(const Memory&);
    ~Memory();

    MemorySection *get_section(std::uint32_t address, bool create) const; // returns section containing given address
    void write_byte(std::uint32_t address, std::uint8_t value);
    std::uint8_t read_byte(std::uint32_t address) const;

    bool operator==(const Memory&) const;
    bool operator!=(const Memory&) const;

    const union MemoryTree *get_memorytree_root() const;

private:
    union MemoryTree *mt_root;
    static union MemoryTree *allocate_section_tree();
    static void free_section_tree(union MemoryTree*, size_t depth);
    static bool compare_section_tree(const union MemoryTree*, const union MemoryTree*, size_t depth);
    static bool is_zero_section_tree(const union MemoryTree*, size_t depth);
    static union MemoryTree *copy_section_tree(const union MemoryTree*, size_t depth);
};

Q_DECLARE_METATYPE(Memory)

#endif // MEMORY_H
