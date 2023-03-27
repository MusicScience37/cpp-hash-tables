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
 * \brief Implementation of hashed_key_view class.
 */
#include "hash_tables/tables/internal/hashed_key_view.h"

#include <functional>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include "hash_tables/extract_key_functions/extract_first_from_pair.h"

// NOLINTNEXTLINE
TEST_CASE("hash_tables::tables::internal::hashed_key_view") {
    using hash_tables::extract_key_functions::extract_first_from_pair;
    using hash_tables::tables::internal::extract_hashed_key_view;
    using hash_tables::tables::internal::hashed_key_view;
    using hash_tables::tables::internal::hashed_key_view_equal;
    using hash_tables::tables::internal::hashed_key_view_hash;

    using key_type = std::string;
    using view_type = hashed_key_view<key_type>;
    using hash_type = hashed_key_view_hash<key_type>;
    // NOLINTNEXTLINE: check types for test.
    using equal_type = hashed_key_view_equal<key_type, std::equal_to<key_type>>;
    using value_type = std::pair<key_type, int>;
    using extract_key_type = extract_hashed_key_view<value_type, key_type,
        extract_first_from_pair<value_type>>;

    SECTION("get data given to constructor") {
        const key_type key = "abc";
        constexpr std::size_t hash_number = 12345;
        const auto view = view_type(key, hash_number);

        CHECK(view.key() == key);
        CHECK(view.hash_number() == hash_number);
    }

    SECTION("calculate hash") {
        const key_type key = "abc";
        constexpr std::size_t hash_number = 12345;
        const auto view = view_type(key, hash_number);

        CHECK(hash_type()(view) == hash_number);
    }

    SECTION("compare views") {
        const key_type key1 = "abc";
        const auto view1 = view_type(key1, 3);
        const key_type key2 = "def";
        const auto view2 = view_type(key2, 3);
        const key_type key3 = "ghi";
        const auto view3 = view_type(key3, 5);
        const auto equal =
            equal_type(std::equal_to<key_type>());  // NOLINT: false positive.

        CHECK(equal(view1, view1));
        CHECK_FALSE(equal(view1, view2));
        CHECK_FALSE(equal(view1, view3));
    }

    SECTION("extract views from values") {
        const key_type key = "abc";
        constexpr std::size_t hash_number = 12345;
        const auto hashed_value =
            std::make_pair(std::make_pair(key, 3), hash_number);
        const auto extract_key =
            extract_key_type(extract_first_from_pair<value_type>());

        const auto view = extract_key(hashed_value);

        CHECK(view.key() == key);
        CHECK(view.hash_number() == hash_number);
    }
}
