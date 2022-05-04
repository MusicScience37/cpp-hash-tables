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
 * \brief Test of separate_shared_chain_table_mt class.
 */
#include "hash_tables/tables/separate_shared_chain_table_mt.h"

#include <string>
#include <unordered_set>

#include <catch2/catch_message.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "hash_tables/hashes/std_hash.h"
#include "hash_tables_test/extract_key_functions/extract_first_element.h"
#include "hash_tables_test/hashes/fixed_hash.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("hash_tables::tables::separate_shared_chain_table_mt", "",
    (std::tuple<hash_tables::hashes::std_hash<char>>),
    (std::tuple<hash_tables_test::hashes::fixed_hash<char>>)) {
    using hash_tables::tables::separate_shared_chain_table_mt;

    using key_type = char;
    using value_type = std::string;
    using extract_key_type =
        hash_tables_test::extract_key_functions::extract_first_element<
            value_type>;
    using hash_type = std::tuple_element_t<0, TestType>;
    using table_type = separate_shared_chain_table_mt<value_type, key_type,
        extract_key_type, hash_type>;

    SECTION("default constructor") {
        table_type table;
        CHECK(table.size() == 0);  // NOLINT
        CHECK(table.empty());
        CHECK(table.num_buckets() == table_type::default_num_buckets);
    }

    SECTION("insert (const reference)") {
        SECTION("successfull") {
            table_type table;

            const auto value = std::string("abc");
            const char key = extract_key_type()(value);
            CHECK(table.insert(value));
            CHECK(table.size() == 1);
            CHECK(table.at(key) == value);
        }

        SECTION("multiple") {
            table_type table;

            const auto value1 = std::string("abc");
            const char key1 = extract_key_type()(value1);
            CHECK(table.insert(value1));
            CHECK(table.size() == 1);
            CHECK(table.at(key1) == value1);

            const auto value2 = std::string("bcd");
            const char key2 = extract_key_type()(value2);
            CHECK(table.insert(value2));
            CHECK(table.size() == 2);
            CHECK(table.at(key2) == value2);
        }

        SECTION("duplicate") {
            table_type table;

            const auto value1 = std::string("abc");
            const char key1 = extract_key_type()(value1);
            CHECK(table.insert(value1));
            CHECK(table.size() == 1);
            CHECK(table.at(key1) == value1);

            const auto value2 = std::string("ab");
            CHECK_FALSE(table.insert(value2));
            CHECK(table.size() == 1);
            CHECK(table.at(key1) == value1);
        }
    }

    SECTION("insert (rvalue reference)") {
        SECTION("successfull") {
            table_type table;

            const auto value = std::string("abc");
            const char key = extract_key_type()(value);
            CHECK(table.insert(std::string(value)));
            CHECK(table.size() == 1);
            CHECK(table.at(key) == value);
        }

        SECTION("multiple") {
            table_type table;

            const auto value1 = std::string("abc");
            const char key1 = extract_key_type()(value1);
            CHECK(table.insert(std::string(value1)));
            CHECK(table.size() == 1);
            CHECK(table.at(key1) == value1);

            const auto value2 = std::string("bcd");
            const char key2 = extract_key_type()(value2);
            CHECK(table.insert(std::string(value2)));
            CHECK(table.size() == 2);
            CHECK(table.at(key2) == value2);
        }

        SECTION("duplicate") {
            table_type table;

            const auto value1 = std::string("abc");
            const char key1 = extract_key_type()(value1);
            CHECK(table.insert(std::string(value1)));
            CHECK(table.size() == 1);
            CHECK(table.at(key1) == value1);

            const auto value2 = std::string("ab");
            CHECK_FALSE(table.insert(std::string(value2)));
            CHECK(table.size() == 1);
            CHECK(table.at(key1) == value1);
        }
    }

    SECTION("emplace") {
        SECTION("successfull") {
            table_type table;

            const auto value = std::string("abc");
            const char key = extract_key_type()(value);
            CHECK(table.emplace(key, value.c_str()));
            CHECK(table.size() == 1);
            CHECK(table.at(key) == value);
        }

        SECTION("multiple") {
            table_type table;

            const auto value1 = std::string("abc");
            const char key1 = extract_key_type()(value1);
            CHECK(table.emplace(key1, value1.c_str()));
            CHECK(table.size() == 1);
            CHECK(table.at(key1) == value1);

            const auto value2 = std::string("bcd");
            const char key2 = extract_key_type()(value2);
            CHECK(table.emplace(key2, value2.c_str()));
            CHECK(table.size() == 2);
            CHECK(table.at(key2) == value2);
        }

        SECTION("duplicate") {
            table_type table;

            const auto value1 = std::string("abc");
            const char key1 = extract_key_type()(value1);
            CHECK(table.emplace(key1, value1));
            CHECK(table.size() == 1);
            CHECK(table.at(key1) == value1);

            const auto value2 = std::string("ab");
            CHECK_FALSE(table.emplace(key1, value2));
            CHECK(table.size() == 1);
            CHECK(table.at(key1) == value1);
        }
    }
}
