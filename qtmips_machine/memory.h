#ifndef MEMORY_H
#define MEMORY_H

#include <QObject>
#include <cstdint>
#include <qtmipsexception.h>

namespace machine {

// Virtual class for common memory access
class MemoryAccess : public QObject {
    Q_OBJECT
public:
    // Note: hword and word methods are throwing away lowest bits so unaligned access is ignored without error.
    void write_byte(std::uint32_t offset, std::uint8_t value);
    void write_hword(std::uint32_t offset, std::uint16_t value);
    void write_word(std::uint32_t offset, std::uint32_t value);

    std::uint8_t read_byte(std::uint32_t offset) const;
    std::uint16_t read_hword(std::uint32_t offset) const;
    std::uint32_t read_word(std::uint32_t offset) const;

    enum AccessControl {
        AC_NONE,
        AC_BYTE,
        AC_HALFWORD,
        AC_WORD,
        AC_BYTE_UNSIGNED,
        AC_HALFWORD_UNSIGNED
    };
    void write_ctl(enum AccessControl ctl, std::uint32_t offset, std::uint32_t value);
    std::uint32_t read_ctl(enum AccessControl ctl, std::uint32_t offset) const;

protected:
    virtual void wword(std::uint32_t offset, std::uint32_t value) = 0;
    virtual std::uint32_t rword(std::uint32_t offset) const = 0;

private:
    static int sh_nth(std::uint32_t offset);
};

class MemorySection : public MemoryAccess {
public:
    MemorySection(std::uint32_t length);
    MemorySection(const MemorySection&);
    ~MemorySection();

    void wword(std::uint32_t offset, std::uint32_t value);
    std::uint32_t rword(std::uint32_t offset) const;
    void merge(MemorySection&);

    std::uint32_t length() const;
    const std::uint32_t* data() const;

    bool operator==(const MemorySection&) const;
    bool operator!=(const MemorySection&) const;

private:
    std::uint32_t len;
    std::uint32_t *dt;
};

union MemoryTree {
    union MemoryTree *mt;
    MemorySection *sec;
};

class Memory : public MemoryAccess {
    Q_OBJECT
public:
    Memory();
    Memory(const Memory&);
    ~Memory();
    void reset(); // Reset whole content of memory (removes old tree and creates new one)
    void reset(const Memory&);

    MemorySection *get_section(std::uint32_t address, bool create) const; // returns section containing given address
    void wword(std::uint32_t address, std::uint32_t value);
    std::uint32_t rword(std::uint32_t address) const;

    bool operator==(const Memory&) const;
    bool operator!=(const Memory&) const;

    const union MemoryTree *get_memorytree_root() const;

    // These functions locate next start or end of next allocated tree leaf
    std::uint32_t next_allocated(std::uint32_t) const;
    std::uint32_t prev_allocated(std::uint32_t) const;

private:
    union MemoryTree *mt_root;
    static union MemoryTree *allocate_section_tree();
    static void free_section_tree(union MemoryTree*, size_t depth);
    static bool compare_section_tree(const union MemoryTree*, const union MemoryTree*, size_t depth);
    static bool is_zero_section_tree(const union MemoryTree*, size_t depth);
    static union MemoryTree *copy_section_tree(const union MemoryTree*, size_t depth);
};

}

Q_DECLARE_METATYPE(machine::MemoryAccess::AccessControl)
Q_DECLARE_METATYPE(machine::Memory)

#endif // MEMORY_H
