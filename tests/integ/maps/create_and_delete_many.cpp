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
#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

#include <catch2/catch_message.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "hash_tables/maps/open_address_map_st.h"
#include "hash_tables/maps/separate_shared_chain_map_mt.h"
#include "hash_tables_test/create_random_int_vector.h"

using key_type = int;
using mapped_type = std::string;

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("create and delete many pairs in maps", "",
    (hash_tables::maps::open_address_map_st<key_type, mapped_type>),
    (hash_tables::maps::separate_shared_chain_map_mt<key_type, mapped_type>)) {
    SECTION("test") {
        constexpr std::size_t size = 1000;
        const auto keys =
            hash_tables_test::create_random_int_vector<key_type>(size);

        TestType map;

        for (std::size_t i = 0; i < size; ++i) {
            INFO("i = " << i);
            const auto key = keys.at(i);
            INFO("key = " << key);
            CHECK(map.emplace(key, std::to_string(key)));
        }
        CHECK(map.size() == size);

        for (std::size_t i = 0; i < size; ++i) {
            INFO("i = " << i);
            const auto key = keys.at(i);
            INFO("key = " << key);
            CHECK(map.at(key) == std::to_string(key));
        }

        for (std::size_t i = 0; i < size; ++i) {
            INFO("i = " << i);
            const auto key = keys.at(i);
            INFO("key = " << key);
            CHECK(map.erase(key));
        }
        CHECK(map.size() == 0);  // NOLINT
    }
}
