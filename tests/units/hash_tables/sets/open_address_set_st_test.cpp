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
 * \brief Test of open_address_set_st class.
 */
#include "hash_tables/sets/open_address_set_st.h"

#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "hash_tables/hashes/std_hash.h"
#include "hash_tables_test/hashes/fixed_hash.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE("hash_tables::sets::open_address_set_st", "",
    (std::tuple<hash_tables::hashes::std_hash<std::string>>),
    (std::tuple<hash_tables_test::hashes::fixed_hash<std::string>>)) {
    using hash_tables::sets::open_address_set_st;

    using key_type = std::string;
    using hash_type = std::tuple_element_t<0, TestType>;
    using set_type = open_address_set_st<key_type, hash_type>;

    SECTION("default constructor") {
        set_type set;
        CHECK(set.size() == 0);  // NOLINT
        CHECK(set.empty());
    }

    SECTION("copy constructor") {
        set_type orig;

        const auto key = std::string("abc");
        CHECK(orig.insert(key));

        const set_type copy{orig};  // NOLINT
        CHECK(copy.size() == 1);
        CHECK(copy.has(key));
    }

    SECTION("move constructor") {
        set_type orig;

        const auto key = std::string("abc");
        CHECK(orig.insert(key));

        const set_type copy{std::move(orig)};  // NOLINT
        CHECK(copy.size() == 1);
        CHECK(copy.has(key));
    }

    SECTION("copy assignment operator") {
        set_type orig;

        const auto key = std::string("abc");
        CHECK(orig.insert(key));

        const set_type copy = orig;  // NOLINT
        CHECK(copy.size() == 1);
        CHECK(copy.has(key));
    }

    SECTION("move assignment operator") {
        set_type orig;

        const auto key = std::string("abc");
        CHECK(orig.insert(key));

        const set_type copy = std::move(orig);  // NOLINT
        CHECK(copy.size() == 1);
        CHECK(copy.has(key));
    }

    SECTION("insert (const reference)") {
        set_type set;

        const auto key = std::string("abc");
        CHECK(set.insert(key));
        CHECK(set.size() == 1);
        CHECK(set.has(key));

        CHECK_FALSE(set.insert(key));
        CHECK(set.size() == 1);
        CHECK(set.has(key));
    }

    SECTION("insert (rvalue reference)") {
        set_type set;

        const auto key = std::string("abc");
        CHECK(set.insert(std::string(key)));
        CHECK(set.size() == 1);
        CHECK(set.has(key));

        CHECK_FALSE(set.insert(std::string(key)));
        CHECK(set.size() == 1);
        CHECK(set.has(key));
    }

    SECTION("merge") {
        set_type set1;
        set_type set2;
        set1.insert("abc");
        set1.insert("def");
        set2.insert("def");
        set2.insert("ghi");

        set1.merge(set2);

        CHECK(set1.size() == 3U);
        CHECK(set1.has("abc"));
        CHECK(set1.has("def"));
        CHECK(set1.has("ghi"));
    }

    SECTION("operator+=") {
        set_type set1;
        set_type set2;
        set1.insert("abc");
        set1.insert("def");
        set2.insert("def");
        set2.insert("ghi");

        auto& ret = set1 += set2;

        CHECK(static_cast<void*>(&ret) == static_cast<void*>(&set1));
        CHECK(set1.size() == 3U);
        CHECK(set1.has("abc"));
        CHECK(set1.has("def"));
        CHECK(set1.has("ghi"));
    }

    SECTION("operator+") {
        set_type set1;
        set_type set2;
        set1.insert("abc");
        set1.insert("def");
        set2.insert("def");
        set2.insert("ghi");

        const set_type ret = set1 + set2;

        CHECK(ret.size() == 3U);
        CHECK(ret.has("abc"));
        CHECK(ret.has("def"));
        CHECK(ret.has("ghi"));
    }

    SECTION("has") {
        set_type set;

        const auto key = std::string("abc");
        CHECK(set.insert(key));
        CHECK(set.size() == 1);

        const auto& const_set = set;
        CHECK(const_set.has(key));
        const auto key2 = std::string("def");
        CHECK_FALSE(const_set.has(key2));
    }

    SECTION("for_all") {
        set_type set;

        const auto key1 = std::string("abc");
        CHECK(set.insert(key1));
        const auto key2 = std::string("def");
        CHECK(set.insert(key2));

        const auto& const_set = set;
        std::unordered_set<key_type> keys;
        const_set.for_all(
            [&keys](const key_type& key) { CHECK(keys.insert(key).second); });
        CHECK(keys == std::unordered_set<key_type>{key1, key2});
    }

    SECTION("clear") {
        set_type set;

        const auto key1 = std::string("abc");
        CHECK(set.insert(key1));
        const auto key2 = std::string("def");
        CHECK(set.insert(key2));

        CHECK(set.size() == 2);
        CHECK_NOTHROW(set.clear());
        CHECK(set.size() == 0);  // NOLINT
        CHECK_FALSE(set.has(key1));
        CHECK_FALSE(set.has(key2));
    }

    SECTION("erase") {
        set_type set;

        const auto key1 = std::string("abc");
        CHECK(set.insert(key1));
        const auto key2 = std::string("def");
        CHECK(set.insert(key2));

        CHECK(set.size() == 2);
        CHECK(set.erase(key2));
        CHECK(set.size() == 1);
        CHECK(set.has(key1));
        CHECK_FALSE(set.has(key2));

        const auto key3 = std::string("ghi");
        CHECK_FALSE(set.erase(key3));
    }

    SECTION("erase_if") {
        set_type set;

        const auto key1 = std::string("abc");
        CHECK(set.insert(key1));
        const auto key2 = std::string("def");
        CHECK(set.insert(key2));

        CHECK(set.size() == 2);
        std::unordered_set<key_type> keys;
        CHECK(set.erase_if([&key1, &keys](const key_type& key) {
            CHECK(keys.insert(key).second);
            return key == key1;
        }) == 1);
        CHECK(keys == std::unordered_set<key_type>{key1, key2});
        CHECK(set.size() == 1);
        CHECK_FALSE(set.has(key1));
        CHECK(set.has(key2));
    }

    SECTION("erase with set") {
        set_type set1;
        set_type set2;
        set1.insert("abc");
        set1.insert("def");
        set2.insert("def");
        set2.insert("ghi");

        set1.erase(set2);

        CHECK(set1.size() == 1U);
        CHECK(set1.has("abc"));
    }

    SECTION("operator-=") {
        set_type set1;
        set_type set2;
        set1.insert("abc");
        set1.insert("def");
        set2.insert("def");
        set2.insert("ghi");

        auto& ret = set1 -= set2;

        CHECK(static_cast<void*>(&ret) == static_cast<void*>(&set1));
        CHECK(set1.size() == 1U);
        CHECK(set1.has("abc"));
    }

    SECTION("operator-") {
        set_type set1;
        set_type set2;
        set1.insert("abc");
        set1.insert("def");
        set2.insert("def");
        set2.insert("ghi");

        const set_type ret = set1 - set2;

        CHECK(ret.size() == 1U);
        CHECK(ret.has("abc"));
    }

    SECTION("keep_only_intersection_with") {
        set_type set1;
        set_type set2;
        set1.insert("abc");
        set1.insert("def");
        set2.insert("def");
        set2.insert("ghi");

        set1.keep_only_intersection_with(set2);

        CHECK(set1.size() == 1U);
        CHECK(set1.has("def"));
    }

    SECTION("reserve") {
        set_type set;

        const auto key = std::string("abc");
        CHECK(set.insert(key));

        CHECK(set.size() == 1);
        CHECK(set.num_nodes() == set_type::table_type::default_num_nodes);

        SECTION("to larger size") {
            constexpr std::size_t size = 128;
            CHECK_NOTHROW(set.reserve(size));
            CHECK(set.size() == 1);
            CHECK(set.num_nodes() > size);
            CHECK(set.has(key));
        }

        SECTION("to smaller size") {
            constexpr std::size_t size = 1;
            CHECK_NOTHROW(set.reserve(size));
            CHECK(set.size() == 1);
            CHECK(set.num_nodes() == set_type::table_type::default_num_nodes);
            CHECK(set.has(key));
        }
    }

    SECTION("rehash") {
        set_type set;

        const auto key = std::string("abc");
        CHECK(set.insert(key));

        CHECK(set.size() == 1);
        CHECK(set.num_nodes() == set_type::table_type::default_num_nodes);

        SECTION("to larger size") {
            constexpr std::size_t min_num_nodes = 200;
            constexpr std::size_t expected_num_nodes = 256;
            CHECK_NOTHROW(set.rehash(min_num_nodes));
            CHECK(set.size() == 1);
            CHECK(set.num_nodes() == expected_num_nodes);
            CHECK(set.has(key));
        }

        SECTION("to larger size (power of two)") {
            constexpr std::size_t min_num_nodes = 128;
            CHECK_NOTHROW(set.rehash(min_num_nodes));
            CHECK(set.size() == 1);
            CHECK(set.num_nodes() == min_num_nodes);
            CHECK(set.has(key));
        }

        SECTION("to smaller size") {
            constexpr std::size_t min_num_nodes = 1;
            CHECK_NOTHROW(set.rehash(min_num_nodes));
            CHECK(set.size() == 1);
            CHECK(set.num_nodes() == set_type::table_type::default_num_nodes);
            CHECK(set.has(key));
        }
    }

    SECTION("load_factor") {
        set_type set;
        CHECK(set.load_factor() == 0.0F);

        CHECK(set.insert("abc"));
        CHECK(set.size() == 1);
        CHECK(set.load_factor() ==
            static_cast<float>(set.size()) /
                static_cast<float>(set.num_nodes()));

        CHECK(set.insert("def"));
        CHECK(set.size() == 2);
        CHECK(set.load_factor() ==
            static_cast<float>(set.size()) /
                static_cast<float>(set.num_nodes()));
    }

    SECTION("max_load_factor") {
        set_type set;

        constexpr float value = 0.1F;
        CHECK_NOTHROW(set.max_load_factor(value));
        CHECK(set.max_load_factor() == value);

        CHECK_THROWS(set.max_load_factor(0.0F));    // NOLINT
        CHECK_NOTHROW(set.max_load_factor(0.01F));  // NOLINT
        CHECK_NOTHROW(set.max_load_factor(0.99F));  // NOLINT
        CHECK_THROWS(set.max_load_factor(1.0F));    // NOLINT
    }
}
