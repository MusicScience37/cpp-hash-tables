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
 * \brief Test of multi_open_address_map_st class.
 */
#include "hash_tables/maps/multi_open_address_map_st.h"

#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "hash_tables/hashes/std_hash.h"
#include "hash_tables/tables/multi_open_address_table_st.h"
#include "hash_tables_test/hashes/fixed_hash.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("hash_tables::maps::multi_open_address_map_st", "",
    (std::tuple<hash_tables::hashes::std_hash<std::string>>),
    (std::tuple<hash_tables_test::hashes::fixed_hash<std::string>>)) {
    using hash_tables::maps::multi_open_address_map_st;

    using key_type = std::string;
    using mapped_type = int;
    using hash_type = std::tuple_element_t<0, TestType>;
    using map_type =
        multi_open_address_map_st<key_type, mapped_type, hash_type>;

    SECTION("default constructor") {
        map_type map;
        CHECK(map.size() == 0);  // NOLINT
        CHECK(map.empty());
    }

    SECTION("constructor with an argument") {
        map_type map{5U};        // NOLINT
        CHECK(map.size() == 0);  // NOLINT
        CHECK(map.empty());
    }

    SECTION("copy constructor") {
        map_type orig;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(orig.emplace(key, mapped));

        const map_type copy{orig};  // NOLINT
        CHECK(copy.size() == 1);
        CHECK(copy.at(key) == mapped);
    }

    SECTION("move constructor") {
        map_type orig;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(orig.emplace(key, mapped));

        const map_type copy{std::move(orig)};  // NOLINT
        CHECK(copy.size() == 1);
        CHECK(copy.at(key) == mapped);
    }

    SECTION("copy assignment operator") {
        map_type orig;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(orig.emplace(key, mapped));

        const map_type copy = orig;  // NOLINT
        CHECK(copy.size() == 1);
        CHECK(copy.at(key) == mapped);
    }

    SECTION("move assignment operator") {
        map_type orig;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(orig.emplace(key, mapped));

        const map_type copy = std::move(orig);  // NOLINT
        CHECK(copy.size() == 1);
        CHECK(copy.at(key) == mapped);
    }

    SECTION("insert (const reference)") {
        map_type map;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        const typename map_type::value_type value = std::make_pair(key, mapped);
        CHECK(map.insert(value));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        constexpr int mapped2 = 12345;
        const typename map_type::value_type value2 =
            std::make_pair(key, mapped2);
        CHECK_FALSE(map.insert(value2));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);
    }

    SECTION("insert (rvalue reference)") {
        map_type map;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(map.insert(std::make_pair(key, mapped)));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        constexpr int mapped2 = 12345;
        CHECK_FALSE(map.insert(std::make_pair(key, mapped2)));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);
    }

    SECTION("emplace (const reference)") {
        map_type map;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(map.emplace(key, mapped));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        constexpr int mapped2 = 12345;
        CHECK_FALSE(map.emplace(key, mapped2));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);
    }

    SECTION("emplace (rvalue reference)") {
        map_type map;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(map.emplace(std::string(key), mapped));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        constexpr int mapped2 = 12345;
        CHECK_FALSE(map.emplace(std::string(key), mapped2));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);
    }

    SECTION("emplace_of_assign (const reference)") {
        map_type map;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(map.emplace_or_assign(key, mapped));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        constexpr int mapped2 = 12345;
        CHECK_FALSE(map.emplace_or_assign(key, mapped2));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped2);
    }

    SECTION("emplace_of_assign (rvalue reference)") {
        map_type map;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(map.emplace_or_assign(std::string(key), mapped));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        constexpr int mapped2 = 12345;
        CHECK_FALSE(map.emplace_or_assign(std::string(key), mapped2));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped2);
    }

    SECTION("emplace_of_assign (const reference)") {
        map_type map;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(map.emplace(key, mapped));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        constexpr int mapped2 = 12345;
        CHECK(map.assign(key, mapped2));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped2);

        const auto key2 = std::to_string(mapped2);
        CHECK_FALSE(map.assign(key2, mapped2));
        CHECK(map.size() == 1);
        CHECK_THROWS(map.at(key2));
    }

    SECTION("get_or_create") {
        map_type map;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(map.emplace(key, mapped));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        constexpr int mapped2 = 12345;
        CHECK(map.get_or_create(key, mapped2) == mapped);
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        const auto key2 = std::to_string(mapped2);
        CHECK(map.get_or_create(key2, mapped2) == mapped2);
        CHECK(map.size() == 2);
        CHECK(map.at(key2) == mapped2);
    }

    SECTION("get_or_create_with_factory") {
        map_type map;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(map.emplace(key, mapped));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        static constexpr int mapped2 = 12345;
        CHECK(map.get_or_create_with_factory(key, [] { return mapped2; }) ==
            mapped);
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        const auto key2 = std::to_string(mapped2);
        CHECK(map.get_or_create_with_factory(key2, [] { return mapped2; }) ==
            mapped2);
        CHECK(map.size() == 2);
        CHECK(map.at(key2) == mapped2);
    }

    SECTION("operator[] (non const)") {
        map_type map;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(map.emplace(key, mapped));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        CHECK(map[key] == mapped);
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        const auto key2 = std::string("abc");
        CHECK(map[key2] == 0);
        CHECK(map.size() == 2);
        CHECK(map.at(key2) == 0);
    }

    SECTION("operator[] (const)") {
        map_type map;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(map.emplace(key, mapped));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        const auto& const_map = map;
        CHECK(const_map[key] == mapped);
        CHECK(const_map.size() == 1);
        CHECK(const_map.at(key) == mapped);

        const auto key2 = std::string("abc");
        CHECK_THROWS(const_map[key2]);
        CHECK(const_map.size() == 1);
        CHECK_THROWS(const_map.at(key2));
    }

    SECTION("try_get (non const)") {
        map_type map;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(map.emplace(key, mapped));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        SECTION("found") {
            mapped_type* res = map.try_get(key);
            REQUIRE(static_cast<void*>(res) != nullptr);
            CHECK(*res == mapped);
        }

        SECTION("not found") {
            const auto key2 = std::string("abc");
            mapped_type* res = map.try_get(key2);
            CHECK(static_cast<void*>(res) == nullptr);
        }
    }

    SECTION("try_get (const)") {
        map_type map;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(map.emplace(key, mapped));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        const auto& const_map = map;

        SECTION("found") {
            const mapped_type* res = const_map.try_get(key);
            REQUIRE(static_cast<const void*>(res) != nullptr);
            CHECK(*res == mapped);
        }

        SECTION("not found") {
            const auto key2 = std::string("abc");
            const mapped_type* res = const_map.try_get(key2);
            CHECK(static_cast<const void*>(res) == nullptr);
        }
    }

    SECTION("has") {
        map_type map;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(map.emplace(key, mapped));
        CHECK(map.size() == 1);
        CHECK(map.at(key) == mapped);

        const auto& const_map = map;
        CHECK(const_map.has(key));
        const auto key2 = std::string("abc");
        CHECK_FALSE(const_map.has(key2));
    }

    SECTION("for_all (non const)") {
        map_type map;

        constexpr int mapped1 = 123;
        const auto key1 = std::to_string(mapped1);
        CHECK(map.emplace(key1, mapped1));
        constexpr int mapped2 = 12345;
        const auto key2 = std::to_string(mapped2);
        CHECK(map.emplace(key2, mapped2));

        std::unordered_set<key_type> keys;
        map.for_all([&keys](const key_type& key, mapped_type& mapped) {
            CHECK(keys.insert(key).second);
            CHECK(key == std::to_string(mapped));
        });
        CHECK(keys == std::unordered_set<key_type>{key1, key2});
    }

    SECTION("for_all (const)") {
        map_type map;

        constexpr int mapped1 = 123;
        const auto key1 = std::to_string(mapped1);
        CHECK(map.emplace(key1, mapped1));
        constexpr int mapped2 = 12345;
        const auto key2 = std::to_string(mapped2);
        CHECK(map.emplace(key2, mapped2));

        const auto& const_map = map;
        std::unordered_set<key_type> keys;
        const_map.for_all(
            [&keys](const key_type& key, const mapped_type& mapped) {
                CHECK(keys.insert(key).second);
                CHECK(key == std::to_string(mapped));
            });
        CHECK(keys == std::unordered_set<key_type>{key1, key2});
    }

    SECTION("clear") {
        map_type map;

        constexpr int mapped1 = 123;
        const auto key1 = std::to_string(mapped1);
        CHECK(map.emplace(key1, mapped1));
        constexpr int mapped2 = 12345;
        const auto key2 = std::to_string(mapped2);
        CHECK(map.emplace(key2, mapped2));

        CHECK(map.size() == 2);
        CHECK_NOTHROW(map.clear());
        CHECK(map.size() == 0);  // NOLINT
        CHECK_THROWS(map.at(key1));
        CHECK_THROWS(map.at(key2));
    }

    SECTION("erase") {
        map_type map;

        constexpr int mapped1 = 123;
        const auto key1 = std::to_string(mapped1);
        CHECK(map.emplace(key1, mapped1));
        constexpr int mapped2 = 12345;
        const auto key2 = std::to_string(mapped2);
        CHECK(map.emplace(key2, mapped2));

        CHECK(map.size() == 2);
        CHECK(map.erase(key2));
        CHECK(map.size() == 1);
        CHECK(map.at(key1) == mapped1);
        CHECK_THROWS(map.at(key2));

        const auto key3 = std::string("abc");
        CHECK_FALSE(map.erase(key3));
    }

    SECTION("erase_if") {
        map_type map;

        constexpr int mapped1 = 123;
        const auto key1 = std::to_string(mapped1);
        CHECK(map.emplace(key1, mapped1));
        constexpr int mapped2 = 12345;
        const auto key2 = std::to_string(mapped2);
        CHECK(map.emplace(key2, mapped2));

        CHECK(map.size() == 2);
        std::unordered_set<key_type> keys;
        CHECK(map.erase_if([&key1, &keys](
                               const key_type& key, const mapped_type& mapped) {
            CHECK(keys.insert(key).second);
            CHECK(key == std::to_string(mapped));
            return key == key1;
        }) == 1);
        CHECK(keys == std::unordered_set<key_type>{key1, key2});
        CHECK(map.size() == 1);
        CHECK_THROWS(map.at(key1));
        CHECK(map.at(key2) == mapped2);
    }

    SECTION("check_all_satisfy") {
        map_type map;

        CHECK(map.emplace("123", 123));
        CHECK(map.emplace("12345", 123));

        CHECK(map.check_all_satisfy(
            [](const std::string& key, const mapped_type& /*mapped*/) {
                return !key.empty();
            }));
        CHECK_FALSE(map.check_all_satisfy(
            [](const std::string& key, const mapped_type& mapped) {
                return std::to_string(mapped) == key;
            }));
        CHECK_FALSE(map.check_all_satisfy(
            [](const std::string& key, const mapped_type& /*mapped*/) {
                return key.empty();
            }));
    }

    SECTION("check_any_satisfy") {
        map_type map;

        CHECK(map.emplace("123", 123));
        CHECK(map.emplace("12345", 123));

        CHECK(map.check_any_satisfy(
            [](const std::string& key, const mapped_type& /*mapped*/) {
                return !key.empty();
            }));
        CHECK(map.check_any_satisfy(
            [](const std::string& key, const mapped_type& mapped) {
                return std::to_string(mapped) == key;
            }));
        CHECK_FALSE(map.check_any_satisfy(
            [](const std::string& key, const mapped_type& /*mapped*/) {
                return key.empty();
            }));
    }

    SECTION("check_none_satisfy") {
        map_type map;

        CHECK(map.emplace("123", 123));
        CHECK(map.emplace("12345", 123));

        CHECK_FALSE(map.check_none_satisfy(
            [](const std::string& key, const mapped_type& /*mapped*/) {
                return !key.empty();
            }));
        CHECK_FALSE(map.check_none_satisfy(
            [](const std::string& key, const mapped_type& mapped) {
                return std::to_string(mapped) == key;
            }));
        CHECK(map.check_none_satisfy(
            [](const std::string& key, const mapped_type& /*mapped*/) {
                return key.empty();
            }));
    }

    SECTION("reserve") {
        map_type map;

        constexpr int mapped = 123;
        const auto key = std::to_string(mapped);
        CHECK(map.emplace(key, mapped));

        CHECK(map.size() == 1);
        CHECK(map.num_nodes() ==
            hash_tables::tables::internal::
                    multi_open_address_table_st_default_min_num_tables *
                map_type::table_type::default_num_internal_nodes);

        SECTION("to larger size") {
            constexpr std::size_t size = 128;
            CHECK_NOTHROW(map.reserve(size));
            CHECK(map.size() == 1);
            CHECK(map.num_nodes() > size);
            CHECK(map.at(key) == mapped);
        }

        SECTION("to smaller size") {
            constexpr std::size_t size = 1;
            CHECK_NOTHROW(map.reserve(size));
            CHECK(map.size() == 1);
            CHECK(map.num_nodes() ==
                hash_tables::tables::internal::
                        multi_open_address_table_st_default_min_num_tables *
                    map_type::table_type::default_num_internal_nodes);
            CHECK(map.at(key) == mapped);
        }
    }

    SECTION("reserve") {
        map_type map;

        constexpr std::size_t size = 128;
        CHECK_NOTHROW(map.reserve_approx(size));
    }
}
