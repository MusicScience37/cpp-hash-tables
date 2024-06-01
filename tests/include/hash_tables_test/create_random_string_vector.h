/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file
 * \brief Definition of create_random_string_vector function.
 */
#pragma once

#include <cstdint>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

namespace hash_tables_test {

/*!
 * \brief Create a vector of random strings.
 *
 * \param[in] size Size.
 * \return Vector.
 */
[[nodiscard]] inline auto create_random_string_vector(
    std::size_t size) -> std::vector<std::string> {
    std::mt19937 engine;  // NOLINT

    constexpr auto len_min = static_cast<std::size_t>(1);
    constexpr auto len_max = static_cast<std::size_t>(16);
    std::uniform_int_distribution<std::size_t> len_dist{len_min, len_max};

    constexpr auto char_min = static_cast<std::uint32_t>(0x20);
    constexpr auto char_max = static_cast<std::uint32_t>(0x7E);
    std::uniform_int_distribution<std::uint32_t> char_dist{char_min, char_max};

    std::unordered_set<std::string> values;
    values.reserve(size);
    while (values.size() < size) {
        const std::size_t len = len_dist(engine);
        std::string val;
        val.reserve(len);
        for (std::size_t i = 0; i < len; ++i) {
            val.push_back(static_cast<char>(
                static_cast<unsigned char>(char_dist(engine))));
        }
        values.insert(std::move(val));
    }

    return {values.begin(), values.end()};
}

}  // namespace hash_tables_test
