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
 * \brief Test of open_address_table_st class.
 */
#include "hash_tables/tables/open_address_table_st.h"

#include <catch2/catch_test_macros.hpp>

#include "hash_tables_test/extract_key_functions/extract_first_element.h"

// NOLINTNEXTLINE
TEST_CASE("hash_tables::tables::open_address_table_st") {
    using hash_tables::tables::open_address_table_st;

    using key_type = char;
    using value_type = std::string;
    using extract_key_type =
        hash_tables_test::extract_key_functions::extract_first_element<
            value_type>;
    using table_type =
        open_address_table_st<value_type, key_type, extract_key_type>;

    SECTION("default constructor") {
        table_type table;
        CHECK(table.size() == 0);  // NOLINT
        CHECK(table.empty());
    }

    SECTION("emplace (const key)") {
        SECTION("successfull") {
            table_type table;

            const auto value = std::string("abc");
            const char key = extract_key_type()(value);
            CHECK(table.emplace(key, value.c_str()));
            CHECK(table.at(key) == value);
        }

        SECTION("duplicate") {
            table_type table;

            const auto value1 = std::string("abc");
            const char key1 = extract_key_type()(value1);
            CHECK(table.emplace(key1, value1));
            CHECK(table.at(key1) == value1);

            const auto value2 = std::string("ab");
            CHECK_FALSE(table.emplace(key1, value2));
            CHECK(table.at(key1) == value1);
        }
    }

    SECTION("at (const)") {
        table_type table;
        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        const auto value2 = std::string("bcdef");
        const char key2 = extract_key_type()(value2);
        CHECK_NOTHROW(table.emplace(key1, value1));
        CHECK_NOTHROW(table.emplace(key2, value2));

        const auto& const_table = table;
        CHECK(const_table.at(key1) == value1);
        CHECK(const_table.at(key2) == value2);
    }
}
