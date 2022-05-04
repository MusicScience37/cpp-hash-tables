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
 * \brief Test to create and delete many pairs in tables.
 */
#include <cstddef>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <catch2/catch_message.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "hash_tables/extract_key_functions/extract_first_from_pair.h"
#include "hash_tables/tables/open_address_table_st.h"
#include "hash_tables/tables/separate_shared_chain_table_mt.h"
#include "hash_tables_test/create_random_int_vector.h"

using key_type = int;
using value_type = std::pair<int, std::string>;
using extract_key =
    hash_tables::extract_key_functions::extract_first_from_pair<value_type>;

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("create and delete many pairs in tables", "",
    (hash_tables::tables::open_address_table_st<value_type, key_type,
        extract_key>),
    (hash_tables::tables::separate_shared_chain_table_mt<value_type, key_type,
        extract_key>)) {
    SECTION("test") {
        constexpr std::size_t size = 1000;
        const auto keys =
            hash_tables_test::create_random_int_vector<key_type>(size);

        TestType table;

        for (std::size_t i = 0; i < size; ++i) {
            INFO("i = " << i);
            const auto key = keys.at(i);
            INFO("key = " << key);
            CHECK(table.emplace(key, key, std::to_string(key)));
        }
        CHECK(table.size() == size);

        for (std::size_t i = 0; i < size; ++i) {
            INFO("i = " << i);
            const auto key = keys.at(i);
            INFO("key = " << key);
            CHECK(table.at(key).second == std::to_string(key));
        }

        for (std::size_t i = 0; i < size; ++i) {
            INFO("i = " << i);
            const auto key = keys.at(i);
            INFO("key = " << key);
            CHECK(table.erase(key));
        }
        CHECK(table.size() == 0);  // NOLINT
    }
}
