/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of create_random_int_vector function.
 */
#pragma once

#include <random>
#include <unordered_set>
#include <vector>

namespace hash_tables_test {

/*!
 * \brief Create a vector of random integer.
 *
 * \tparam ValueType Type of values.
 * \param[in] size Size.
 * \return Vector.
 */
template <typename ValueType>
[[nodiscard]] inline auto create_random_int_vector(std::size_t size)
    -> std::vector<ValueType> {
    std::mt19937 engine;  // NOLINT
    std::uniform_int_distribution<ValueType> dist;
    std::unordered_set<ValueType> values;
    values.reserve(size);
    while (values.size() < size) {
        values.insert(dist(engine));
    }
    return std::vector<ValueType>(values.begin(), values.end());
}

}  // namespace hash_tables_test
