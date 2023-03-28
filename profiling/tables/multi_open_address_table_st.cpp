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
 * \brief Test to create and delete many pairs in tables.
 */
#include "hash_tables/tables/multi_open_address_table_st.h"

#include <cstddef>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <gperftools/profiler.h>

#include "hash_tables/extract_key_functions/extract_first_from_pair.h"
#include "hash_tables_test/create_random_int_vector.h"
#include "hash_tables_test/create_random_string_vector.h"

auto main() -> int {
    using key_type = int;
    using value_type = std::pair<int, std::string>;
    using extract_key =
        hash_tables::extract_key_functions::extract_first_from_pair<value_type>;

    constexpr std::size_t num_repetition = 10000;
#ifndef NDEBUG
    constexpr std::size_t size = 1000;
#else
    constexpr std::size_t size = 10000;
#endif
    const auto keys =
        hash_tables_test::create_random_int_vector<key_type>(size);
    const auto second_values =
        hash_tables_test::create_random_string_vector(size);

    hash_tables::tables::multi_open_address_table_st<value_type, key_type,
        extract_key>
        table;
    table.reserve_approx(size);

    ProfilerStart("hash_tables_prof_tables_multi_open_address_table_st.prof");

    for (std::size_t rep = 0; rep < num_repetition; ++rep) {
        for (std::size_t i = 0; i < size; ++i) {
            const auto key = keys.at(i);
            const auto& second_value = second_values.at(i);
            table.emplace(key, key, second_value);
        }

        for (std::size_t i = 0; i < size; ++i) {
            const auto key = keys.at(i);
            (void)table.at(key);
        }

        for (std::size_t i = 0; i < size; ++i) {
            const auto key = keys.at(i);
            table.erase(key);
        }
    }

    ProfilerStop();

    return 0;
}
