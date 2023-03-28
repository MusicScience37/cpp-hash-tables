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
 * \brief Test of multi_open_address_table_st class.
 */
#include "hash_tables/tables/multi_open_address_table_st.h"

#include <string>
#include <unordered_set>

#include <catch2/catch_message.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "hash_tables/hashes/std_hash.h"
#include "hash_tables_test/extract_key_functions/extract_first_element.h"
#include "hash_tables_test/hashes/fixed_hash.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("hash_tables::tables::multi_open_address_table_st", "",
    (std::tuple<hash_tables::hashes::std_hash<char>>),
    (std::tuple<hash_tables_test::hashes::fixed_hash<char>>)) {
    using hash_tables::tables::multi_open_address_table_st;

    using key_type = char;
    using value_type = std::string;
    using extract_key_type =
        hash_tables_test::extract_key_functions::extract_first_element<
            value_type>;
    using hash_type = std::tuple_element_t<0, TestType>;
    using table_type = multi_open_address_table_st<value_type, key_type,
        extract_key_type, hash_type>;

    SECTION("default constructor") {
        table_type table;
        CHECK(table.size() == 0);  // NOLINT
        CHECK(table.empty());
    }

    SECTION("copy constructor") {
        table_type orig;
        const auto value = std::string("abc");
        orig.insert(value);

        table_type copy{orig};  // NOLINT
        CHECK(copy.at(extract_key_type()(value)) == value);
    }

    SECTION("move constructor") {
        table_type orig;
        const auto value = std::string("abc");
        orig.insert(value);

        table_type copy{std::move(orig)};  // NOLINT
        CHECK(copy.at(extract_key_type()(value)) == value);
    }

    SECTION("copy assignment operator") {
        table_type orig;
        const auto value = std::string("abc");
        orig.insert(value);

        table_type copy;
        copy = orig;  // NOLINT
        CHECK(copy.at(extract_key_type()(value)) == value);
    }

    SECTION("move assignment operator") {
        table_type orig;
        const auto value = std::string("abc");
        orig.insert(value);

        table_type copy;
        copy = std::move(orig);  // NOLINT
        CHECK(copy.at(extract_key_type()(value)) == value);
    }

    SECTION("insert (const reference)") {
        SECTION("successful") {
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
        SECTION("successful") {
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
        SECTION("successful") {
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

    SECTION("emplace_or_assign") {
        SECTION("successful") {
            table_type table;

            const auto value = std::string("abc");
            const char key = extract_key_type()(value);
            CHECK(table.emplace_or_assign(key, value.c_str()));
            CHECK(table.size() == 1);
            CHECK(table.at(key) == value);
        }

        SECTION("multiple") {
            table_type table;

            const auto value1 = std::string("abc");
            const char key1 = extract_key_type()(value1);
            CHECK(table.emplace_or_assign(key1, value1.c_str()));
            CHECK(table.size() == 1);
            CHECK(table.at(key1) == value1);

            const auto value2 = std::string("bcd");
            const char key2 = extract_key_type()(value2);
            CHECK(table.emplace_or_assign(key2, value2.c_str()));
            CHECK(table.size() == 2);
            CHECK(table.at(key2) == value2);
        }

        SECTION("duplicate") {
            table_type table;

            const auto value1 = std::string("abc");
            const char key1 = extract_key_type()(value1);
            CHECK(table.emplace_or_assign(key1, value1));
            CHECK(table.size() == 1);
            CHECK(table.at(key1) == value1);

            const auto value2 = std::string("ab");
            CHECK_FALSE(table.emplace_or_assign(key1, value2));
            CHECK(table.size() == 1);
            CHECK(table.at(key1) == value2);
        }
    }

    SECTION("assign") {
        SECTION("successful") {
            table_type table;

            const auto value1 = std::string("abc");
            const char key1 = extract_key_type()(value1);
            CHECK(table.emplace(key1, value1));
            CHECK(table.size() == 1);
            CHECK(table.at(key1) == value1);

            const auto value2 = std::string("ab");
            CHECK(table.assign(key1, value2));
            CHECK(table.size() == 1);
            CHECK(table.at(key1) == value2);
        }

        SECTION("non-existing key") {
            table_type table;

            const auto value = std::string("abc");
            const char key = extract_key_type()(value);
            CHECK_FALSE(table.assign(key, value.c_str()));
            CHECK(table.size() == 0);  // NOLINT
            CHECK_THROWS(table.at(key));
        }
    }

    // cSpell:ignore bcdef

    SECTION("at (non const)") {
        table_type table;
        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        const auto value2 = std::string("bcdef");
        const char key2 = extract_key_type()(value2);
        CHECK_NOTHROW(table.emplace(key1, value1));
        CHECK_NOTHROW(table.emplace(key2, value2));

        CHECK(table.at(key1) == value1);
        CHECK(table.at(key2) == value2);
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

    SECTION("get_or_create") {
        table_type table;
        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        const auto value2 = std::string("bcdef");
        const char key2 = extract_key_type()(value2);
        CHECK_NOTHROW(table.emplace(key1, value1));
        CHECK(table.size() == 1);

        CHECK(table.get_or_create(key1, "af") == value1);
        CHECK(table.size() == 1);
        CHECK(table.get_or_create(key2, value2.c_str()) == value2);
        CHECK(table.size() == 2);
    }

    SECTION("get_or_create_with_factory") {
        table_type table;
        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        const auto value2 = std::string("bcdef");
        const char key2 = extract_key_type()(value2);
        CHECK_NOTHROW(table.emplace(key1, value1));
        CHECK(table.size() == 1);

        CHECK(table.get_or_create_with_factory(
                  key1, [] { return std::string("af"); }) == value1);
        CHECK(table.size() == 1);
        CHECK(table.get_or_create_with_factory(
                  key2, [&value2] { return std::string(value2); }) == value2);
        CHECK(table.size() == 2);
    }

    SECTION("try_get (non const)") {
        table_type table;
        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        const auto value2 = std::string("bcdef");
        const char key2 = extract_key_type()(value2);
        CHECK_NOTHROW(table.emplace(key1, value1));

        std::string* res1 = table.try_get(key1);
        REQUIRE(static_cast<const void*>(res1) != nullptr);
        CHECK(*res1 == value1);
        std::string* res2 = table.try_get(key2);
        CHECK(static_cast<const void*>(res2) == nullptr);
    }

    SECTION("try_get (const)") {
        table_type table;
        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        const auto value2 = std::string("bcdef");
        const char key2 = extract_key_type()(value2);
        CHECK_NOTHROW(table.emplace(key1, value1));

        const auto& const_table = table;
        const std::string* res1 = const_table.try_get(key1);
        REQUIRE(static_cast<const void*>(res1) != nullptr);
        CHECK(*res1 == value1);
        const std::string* res2 = const_table.try_get(key2);
        CHECK(static_cast<const void*>(res2) == nullptr);
    }

    SECTION("has") {
        table_type table;
        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        const auto value2 = std::string("bcdef");
        const char key2 = extract_key_type()(value2);
        CHECK_NOTHROW(table.emplace(key1, value1));

        CHECK(table.has(key1));
        CHECK_FALSE(table.has(key2));
    }

    SECTION("for_all (non const)") {
        table_type table;

        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        CHECK(table.insert(value1));
        const auto value2 = std::string("bcd");
        const char key2 = extract_key_type()(value2);
        CHECK(table.insert(value2));

        std::unordered_set<std::string> args;
        table.for_all([&args](std::string& val) {
            INFO(val);
            const auto res = args.insert(val);
            CHECK(res.second);
        });
        CHECK(args == std::unordered_set<std::string>{value1, value2});
    }

    SECTION("for_all (const)") {
        table_type table;

        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        CHECK(table.insert(value1));
        const auto value2 = std::string("bcd");
        const char key2 = extract_key_type()(value2);
        CHECK(table.insert(value2));

        const auto& const_table = table;
        std::unordered_set<std::string> args;
        const_table.for_all([&args](const std::string& val) {
            INFO(val);
            const auto res = args.insert(val);
            CHECK(res.second);
        });
        CHECK(args == std::unordered_set<std::string>{value1, value2});
    }

    SECTION("clear") {
        table_type table;

        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        CHECK(table.insert(value1));
        const auto value2 = std::string("bcd");
        const char key2 = extract_key_type()(value2);
        CHECK(table.insert(value2));

        CHECK(table.has(key1));
        CHECK(table.has(key2));
        CHECK(table.size() == 2);

        CHECK_NOTHROW(table.clear());

        CHECK_FALSE(table.has(key1));
        CHECK_FALSE(table.has(key2));
        CHECK(table.size() == 0);  // NOLINT
    }

    SECTION("erase") {
        SECTION("successful") {
            table_type table;

            const auto value1 = std::string("abc");
            const char key1 = extract_key_type()(value1);
            CHECK(table.insert(value1));
            const auto value2 = std::string("bcd");
            const char key2 = extract_key_type()(value2);
            CHECK(table.insert(value2));

            CHECK(table.has(key1));
            CHECK(table.has(key2));
            CHECK(table.size() == 2);

            CHECK(table.erase(key1));

            CHECK_FALSE(table.has(key1));
            CHECK(table.has(key2));
            CHECK(table.size() == 1);
        }

        SECTION("key not found") {
            table_type table;

            const auto value1 = std::string("abc");
            const char key1 = extract_key_type()(value1);
            const auto value2 = std::string("bcd");
            const char key2 = extract_key_type()(value2);
            CHECK(table.insert(value2));

            CHECK_FALSE(table.has(key1));
            CHECK(table.has(key2));
            CHECK(table.size() == 1);

            CHECK_FALSE(table.erase(key1));

            CHECK_FALSE(table.has(key1));
            CHECK(table.has(key2));
            CHECK(table.size() == 1);
        }
    }

    SECTION("erase_if") {
        table_type table;

        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        CHECK(table.insert(value1));
        const auto value2 = std::string("bcd");
        const char key2 = extract_key_type()(value2);
        CHECK(table.insert(value2));

        CHECK(table.has(key1));
        CHECK(table.has(key2));
        CHECK(table.size() == 2);

        CHECK(table.erase_if([&value1](const std::string& val) {
            return val == value1;
        }) == 1);

        CHECK_FALSE(table.has(key1));
        CHECK(table.has(key2));
        CHECK(table.size() == 1);
    }

    SECTION("max_size") {
        table_type table;

        CHECK(table.max_size() > 0U);
    }

    SECTION("reserve") {
        table_type table;

        const auto value = std::string("abc");
        const char key = extract_key_type()(value);
        CHECK(table.insert(value));
        CHECK(table.at(key) == value);

        CHECK(table.size() == 1);
        CHECK(table.num_nodes() ==
            table_type::default_num_tables *
                table_type::default_num_internal_nodes);

        SECTION("to larger size") {
            constexpr std::size_t size = 128;
            CHECK_NOTHROW(table.reserve(size));
            CHECK(table.size() == 1);
            CHECK(table.num_nodes() > size);
            CHECK(table.at(key) == value);
        }

        SECTION("to smaller size") {
            constexpr std::size_t size = 1;
            CHECK_NOTHROW(table.reserve(size));
            CHECK(table.size() == 1);
            CHECK(table.num_nodes() ==
                table_type::default_num_tables *
                    table_type::default_num_internal_nodes);
            CHECK(table.at(key) == value);
        }
    }

    SECTION("reserve_approx") {
        table_type table;

        constexpr std::size_t size = 128;
        CHECK_NOTHROW(table.reserve_approx(size));
    }
}
