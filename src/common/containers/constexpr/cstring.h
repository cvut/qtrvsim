/*
 * Based on:
 *
 * Frozen
 * Copyright 2016 QuarksLab
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef QTRVSIM_CSTRING
#define QTRVSIM_CSTRING

#include <functional>

template<typename CharT>
class cbasic_string {
    using chr_t = CharT;

    chr_t const *data_;
    std::size_t size_;

public:
    constexpr cbasic_string() : data_(""), size_(0) {};
    template<std::size_t N>
    constexpr cbasic_string(chr_t const (&data)[N]) // NOLINT(google-explicit-constructor)
        : data_(data)
        , size_(N - 1) {}
    constexpr cbasic_string(chr_t const *data, std::size_t size) : data_(data), size_(size) {}

    constexpr cbasic_string(const cbasic_string &) noexcept = default;
    constexpr cbasic_string &operator=(const cbasic_string &) noexcept = default;

    constexpr std::size_t size() const { return size_; }

    constexpr chr_t operator[](std::size_t i) const { return data_[i]; }

    constexpr bool operator==(cbasic_string other) const {
        if (size_ != other.size_) return false;
        for (std::size_t i = 0; i < size_; ++i)
            if (data_[i] != other.data_[i]) return false;
        return true;
    }

    constexpr bool operator<(const cbasic_string &other) const {
        unsigned i = 0;
        while (i < size() && i < other.size()) {
            if ((*this)[i] < other[i]) { return true; }
            if ((*this)[i] > other[i]) { return false; }
            ++i;
        }
        return size() < other.size();
    }

    constexpr const chr_t *data() const { return data_; }
    constexpr const chr_t *begin() const { return data(); }
    constexpr const chr_t *end() const { return data() + size(); }
};

using cstring = cbasic_string<char>;
using wcstring = cbasic_string<wchar_t>;
using u16cstring = cbasic_string<char16_t>;
using u32cstring = cbasic_string<char32_t>;

namespace string_literals {

constexpr cstring operator"" _s(const char *data, std::size_t size) {
    return { data, size };
}

constexpr wcstring operator"" _s(const wchar_t *data, std::size_t size) {
    return { data, size };
}

constexpr u16cstring operator"" _s(const char16_t *data, std::size_t size) {
    return { data, size };
}

constexpr u32cstring operator"" _s(const char32_t *data, std::size_t size) {
    return { data, size };
}

} // namespace string_literals

#endif
