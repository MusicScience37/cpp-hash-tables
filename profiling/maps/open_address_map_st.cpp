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
 * \brief Test to create and delete many pairs in maps.
 */
#include "hash_tables/maps/open_address_map_st.h"

#include <cstddef>
#include <string>
#include <vector>

#include <gperftools/profiler.h>

#include "hash_tables/hashes/std_hash.h"
#include "hash_tables_test/create_random_int_vector.h"

auto main() -> int {
    using key_type = int;
    using mapped_type = std::string;
    using map_type =
        hash_tables::maps::open_address_map_st<key_type, mapped_type>;

    constexpr std::size_t num_repetition = 10000;
#ifndef NDEBUG
    constexpr std::size_t size = 1000;
#else
    constexpr std::size_t size = 10000;
#endif
    const auto keys =
        hash_tables_test::create_random_int_vector<key_type>(size);
    std::vector<mapped_type> mapped_values;
    mapped_values.reserve(keys.size());
    for (const auto& key : keys) {
        mapped_values.push_back(std::to_string(key));
    }

    ProfilerStart("hash_tables_prof_maps_open_address_map_st.prof");

    for (std::size_t rep = 0; rep < num_repetition; ++rep) {
        map_type map;
        map.reserve(size);

        for (std::size_t i = 0; i < size; ++i) {
            const auto key = keys.at(i);
            const auto& mapped_value = mapped_values.at(i);
            map.emplace(key, mapped_value);
        }

        for (std::size_t i = 0; i < size; ++i) {
            const auto key = keys.at(i);
            (void)map.at(key);
        }

        for (std::size_t i = 0; i < size; ++i) {
            const auto key = keys.at(i);
            map.erase(key);
        }
    }

    ProfilerStop();

    return 0;
}
