/*
 * Based on:
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

#ifndef QTRVSIM_CVECTOR_H
#define QTRVSIM_CVECTOR_H

#include <array>
#include <iterator>
#include <string>
#include <utility>

/** Constexpr vector with inline storage */
template<typename T, std::size_t N>
class cvector {
    T data[N]; // Uninitialized storage cannot be used in constexpr.
    std::size_t dsize = 0;

public:
    // Container typdefs
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    // Constructors
    constexpr cvector() = default;
    constexpr cvector(size_type count, const T &value) : dsize(count) {
        for (std::size_t i = 0; i < N; ++i)
            data[i] = value;
    }
    constexpr cvector(std::initializer_list<value_type> initializer_list) {
        for (auto item : initializer_list) {
            push_back(item);
        }
    }

    // Iterators
    constexpr iterator begin() noexcept { return data; }
    constexpr const_iterator begin() const noexcept { return data; }
    constexpr const_iterator cbegin() const noexcept { return data; }
    constexpr iterator end() noexcept { return data + dsize; }
    constexpr const_iterator end() const noexcept { return data + dsize; }
    constexpr const_iterator cend() const noexcept { return data + dsize; }

    // Capacity
    [[nodiscard]] constexpr size_type size() const { return dsize; }

    // Element access
    constexpr reference operator[](std::size_t index) { return data[index]; }
    constexpr const_reference operator[](std::size_t index) const { return data[index]; }

    constexpr reference back() { return data[dsize - 1]; }
    constexpr const_reference back() const { return data[dsize - 1]; }

    // Modifiers
    constexpr void push_back(const T &a) { data[dsize++] = a; }
    constexpr void push_back(T &&a) { data[dsize++] = std::move(a); }
    constexpr void pop_back() { --dsize; }

    constexpr void clear() { dsize = 0; }
};
#endif // QTRVSIM_CVECTOR_H
