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

    virtual std::uint8_t read_byte(std::uint32_t offset) = 0;
    std::uint16_t read_hword(std::uint32_t offset);
    std::uint32_t read_word(std::uint32_t offset);

signals:
    // TODO trigger
    void byte_change(std::uint32_t address, std::uint32_t value);
};

class MemorySection : public MemoryAccess {
public:
    MemorySection(std::uint32_t length);
    ~MemorySection();
    void write_byte(std::uint32_t offset, std::uint8_t value);
    std::uint8_t read_byte(std::uint32_t offset);
private:
    std::uint32_t length;
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
    ~Memory();
    MemorySection *get_section(std::uint32_t address, bool create); // returns section containing given address
    void write_byte(std::uint32_t address, std::uint8_t value);
    std::uint8_t read_byte(std::uint32_t address);
private:
    union MemoryTree *mt_root;
    static union MemoryTree *allocate_section_tree();
};

#endif // MEMORY_H
