#include "debuginfo.h"

#include <algorithm>
#include <cassert>
#include <numeric>

namespace debuginfo {

FileId DebugInfo::get_file_id(const std::string &path) {
    // This method is not called often so we prefer compact memory layout
    // over quick access.
    for (size_t i = 0; i < files.size(); ++i) {
        if (files[i] == path) { return static_cast<FileId>(i); }
    }
    files.push_back(path);
    return static_cast<FileId>(files.size() - 1);
}

void DebugInfo::add_line(uint64_t address, FileId file_index, uint32_t line) {
    addresses.push_back(address);
    locations.push_back({ file_index, line });
}

const SourceLocation *DebugInfo::find(uint64_t address, size_t *hint_index) const {
    if (addresses.empty()) { return nullptr; }

    // Try to use hint for faster sequential access.
    // This is a pure optimization - if hint doesn't help, we fall back to full search.
    if (hint_index && *hint_index < addresses.size()) {
        size_t hint = *hint_index;

        // Check if hint points directly to the answer.
        if (addresses[hint] <= address) {
            // Check if this is the last entry or next entry is beyond our address.
            if (hint + 1 >= addresses.size() || addresses[hint + 1] > address) {
                // hint_index already has the correct value, no update needed
                return &locations[hint];
            }
            // Hint was too low but in the right direction - check hint+1.
            if (hint + 2 >= addresses.size() || addresses[hint + 2] > address) {
                *hint_index = hint + 1;
                return &locations[hint + 1];
            }
        }
        // Hint didn't help, fall through to full search.
    }

    // Find the greatest address that is <= the query address.
    // upper_bound finds first element > address, then we decrement.
    auto it = std::upper_bound(addresses.begin(), addresses.end(), address);

    if (it == addresses.begin()) {
        // Address is before all entries.
        return nullptr;
    }

    --it; // Now points to greatest element <= address.
    size_t index = static_cast<size_t>(std::distance(addresses.begin(), it));

    if (hint_index) { *hint_index = index; }

    return &locations[index];
}

const std::string &DebugInfo::get_file_path(FileId id) const {
    return files.at(id);
}

void DebugInfo::finalize() {
    if (addresses.empty()) { return; }

    // We need to sort addresses and keep locations in sync.
    // Create a permutation index and sort it by address.
    std::vector<size_t> permutation(addresses.size());
    std::iota(permutation.begin(), permutation.end(), size_t(0));

    std::sort(permutation.begin(), permutation.end(), [this](size_t a, size_t b) {
        if (addresses[a] != addresses[b]) { return addresses[a] < addresses[b]; }
        return a > b;
    });

    // Apply permutation to both vectors.
    std::vector<uint64_t> sorted_addresses;
    std::vector<SourceLocation> sorted_locations;
    sorted_addresses.reserve(addresses.size());
    sorted_locations.reserve(locations.size());

    for (size_t i = 0; i < permutation.size(); ++i) {
        size_t idx = permutation[i];
        if (!sorted_addresses.empty() && sorted_addresses.back() == addresses[idx]) { continue; }
        sorted_addresses.push_back(addresses[idx]);
        sorted_locations.push_back(locations[idx]);
    }

    addresses = std::move(sorted_addresses);
    locations = std::move(sorted_locations);
}

void DebugInfo::clear() {
    files.clear();
    addresses.clear();
    locations.clear();
}

} // namespace debuginfo