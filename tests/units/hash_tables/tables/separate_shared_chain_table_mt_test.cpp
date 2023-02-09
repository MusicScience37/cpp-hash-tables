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

#include <optional>
#include <ostream>
#include <string>
#include <tuple>
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

    SECTION("at") {
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

    SECTION("get_to") {
        table_type table;
        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        const auto value2 = std::string("bcdef");
        const char key2 = extract_key_type()(value2);
        CHECK_NOTHROW(table.emplace(key1, value1));
        CHECK_NOTHROW(table.emplace(key2, value2));

        const auto& const_table = table;
        std::optional<value_type> res;
        CHECK_NOTHROW(const_table.get_to(res, key1));
        CHECK(res == value1);
        CHECK_NOTHROW(const_table.get_to(res, key2));
        CHECK(res == value2);
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

    SECTION("get_or_create_to") {
        table_type table;
        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        const auto value2 = std::string("bcdef");
        const char key2 = extract_key_type()(value2);
        CHECK_NOTHROW(table.emplace(key1, value1));
        CHECK(table.size() == 1);

        std::optional<value_type> res;
        CHECK_NOTHROW(table.get_or_create_to(res, key1, "af"));
        CHECK(res == value1);
        CHECK(table.size() == 1);
        CHECK_NOTHROW(table.get_or_create_to(res, key2, value2.c_str()));
        CHECK(res == value2);
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

    SECTION("get_or_create_with_factory_to") {
        table_type table;
        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        const auto value2 = std::string("bcdef");
        const char key2 = extract_key_type()(value2);
        CHECK_NOTHROW(table.emplace(key1, value1));
        CHECK(table.size() == 1);

        std::optional<value_type> res;
        CHECK_NOTHROW(table.get_or_create_with_factory_to(
            res, key1, [] { return std::string("af"); }));
        CHECK(res == value1);
        CHECK(table.size() == 1);
        CHECK_NOTHROW(table.get_or_create_with_factory_to(
            res, key2, [&value2] { return std::string(value2); }));
        CHECK(res == value2);
        CHECK(table.size() == 2);
    }

    SECTION("try_get") {
        table_type table;
        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        const auto value2 = std::string("bcdef");
        const char key2 = extract_key_type()(value2);
        CHECK_NOTHROW(table.emplace(key1, value1));

        const auto& const_table = table;
        CHECK(const_table.try_get(key1) == value1);
        CHECK(const_table.try_get(key2) == std::nullopt);
    }

    SECTION("try_get_to") {
        table_type table;
        const auto value1 = std::string("abc");
        const char key1 = extract_key_type()(value1);
        const auto value2 = std::string("bcdef");
        const char key2 = extract_key_type()(value2);
        CHECK_NOTHROW(table.emplace(key1, value1));

        const auto& const_table = table;
        std::optional<value_type> res;
        CHECK(const_table.try_get_to(res, key1));
        CHECK(res == value1);
        CHECK_FALSE(const_table.try_get(key2));
        CHECK(res == value1);
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

    SECTION("load_factor") {
        table_type table;
        CHECK(table.load_factor() == 0.0F);

        CHECK(table.insert("abc"));
        CHECK(table.size() == 1);
        CHECK(table.load_factor() ==
            static_cast<float>(table.size()) /
                static_cast<float>(table.num_buckets()));

        CHECK(table.insert("def"));
        CHECK(table.size() == 2);
        CHECK(table.load_factor() ==
            static_cast<float>(table.size()) /
                static_cast<float>(table.num_buckets()));
    }
}
