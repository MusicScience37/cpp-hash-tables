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
 * \brief Test of value_storage class.
 */
#include "hash_tables/utility/value_storage.h"

#include <type_traits>

#include <catch2/catch_test_macros.hpp>

// NOLINTNEXTLINE
TEST_CASE("hash_tables::utility::value_storage") {
    using hash_tables::utility::value_storage;

    SECTION("copy and move are prohibited") {
        STATIC_CHECK(!std::is_copy_constructible_v<value_storage<int>>);
        STATIC_CHECK(!std::is_copy_assignable_v<value_storage<int>>);
        STATIC_CHECK(!std::is_move_constructible_v<value_storage<int>>);
        STATIC_CHECK(!std::is_move_assignable_v<value_storage<int>>);
    }

    SECTION("construct an int value") {
        constexpr int val = 123;
        value_storage<int> storage;
        const auto& const_storage = storage;

        REQUIRE_NOTHROW(storage.emplace(val));
        REQUIRE(storage.get() == val);
        REQUIRE(const_storage.get() == val);

        REQUIRE_NOTHROW(storage.clear());
    }

    SECTION("construct a string") {
        const auto str = std::string("abc");
        value_storage<std::string> storage;
        const auto& const_storage = storage;

        REQUIRE_NOTHROW(storage.emplace(str.c_str()));
        REQUIRE(storage.get() == str);
        REQUIRE(const_storage.get() == str);

        REQUIRE_NOTHROW(storage.clear());
    }
}
