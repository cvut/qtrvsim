#ifndef DEBUGINFO_H
#define DEBUGINFO_H

#include <cstdint>
#include <string>
#include <vector>

namespace debuginfo {

using FileId = uint32_t;

struct SourceLocation {
    FileId file_id;
    uint32_t line;
};

class DebugInfo {
public:
    DebugInfo() = default;

    /**
     * Lookup existing id or assign new one.
     *
     * Note: This uses O(n) linear search intentionally to save memory.
     * The internal assembler only ever uses one file, and the DWARF loader
     * maintains its own id->id mapping cache, so this is never hot.
     */
    FileId get_file_id(const std::string &path);

    /**
     * Insert new line mapping. Line 0 means no source line.
     * `finalize` must be called after all lines are added.
     */
    void add_line(uint64_t address, FileId file_index, uint32_t line);

    /** First address after the preceding sequence. */
    void add_end_sequence(uint64_t address);

    /**
     * Find the source location for a given address.
     *
     * Returns the source location for the greatest address that is less than
     * or equal to the query address (i.e., the line that contains the address).
     * Returns nullptr if empty, before all entries, in a gap, or on line 0.
     *
     * Optional hint_index can be provided to speed up sequential access.
     * The hint is a pure performance optimization and does not affect
     * correctness - if the hint doesn't help, a full search is performed.
     * If a result is found, hint_index is updated to the found index.
     */
    [[nodiscard]] const SourceLocation *find(uint64_t address, size_t *hint_index = nullptr) const;

    /**
     * Get the file path for a given file id.
     * Throws std::out_of_range if id is invalid.
     */
    const std::string &get_file_path(FileId id) const;

    /**
     * Sort the address table. Must be called after all lines are added.
     * Duplicate addresses keep the last line row over a sequence end.
     */
    void finalize();

    /** Clear all data. */
    void clear();

private:
    std::vector<std::string> files;
    std::vector<uint64_t> addresses;
    std::vector<SourceLocation> locations;
};

} // namespace debuginfo

#endif // DEBUGINFO_H