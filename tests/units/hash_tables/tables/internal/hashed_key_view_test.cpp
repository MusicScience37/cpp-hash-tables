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

#include <catch2/catch_test_macros.hpp>

// NOLINTNEXTLINE
TEST_CASE("hash_tables::tables::internal::hashed_key_view") {
    using hash_tables::tables::internal::hashed_key_view;
    using hash_tables::tables::internal::hashed_key_view_hash;

    using key_type = std::string;
    using view_type = hashed_key_view<key_type>;
    using hash_type = hashed_key_view_hash<key_type>;

    SECTION("get data given to constructor") {
        const key_type key = "abc";
        constexpr std::size_t hash_number = 12345;
        const auto view = view_type(key, hash_number);

        CHECK(view.key() == key);
        CHECK(view.hash_number() == hash_number);
    }

    SECTION("compare views") {
        const key_type key1 = "abc";
        const auto view1 = view_type(key1, 3);
        const key_type key2 = "def";
        const auto view2 = view_type(key2, 3);
        const key_type key3 = "ghi";
        const auto view3 = view_type(key3, 5);

        CHECK(key1 == key1);
        CHECK_FALSE(key1 != key1);

        CHECK_FALSE(key1 == key2);
        CHECK(key1 != key2);

        CHECK_FALSE(key1 == key3);
        CHECK(key1 != key3);
    }

    SECTION("calculate hash") {
        const key_type key = "abc";
        constexpr std::size_t hash_number = 12345;
        const auto view = view_type(key, hash_number);

        CHECK(hash_type()(view) == hash_number);
    }
}
