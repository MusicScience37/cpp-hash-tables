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
 * \brief Test of round_up_to_power_of_two function.
 */
#include "hash_tables/utility/round_up_to_power_of_two.h"

#include <cstddef>
#include <string>

#include <catch2/catch_test_macros.hpp>

// NOLINTNEXTLINE
TEST_CASE("hash_tables::utility::round_up_to_power_of_two") {
    using hash_tables::utility::round_up_to_power_of_two;

    SECTION("calculate") {
        CHECK(round_up_to_power_of_two(static_cast<std::size_t>(0)) ==
            static_cast<std::size_t>(1));
        CHECK(round_up_to_power_of_two(static_cast<std::size_t>(1)) ==
            static_cast<std::size_t>(1));
        CHECK(round_up_to_power_of_two(static_cast<std::size_t>(2)) ==
            static_cast<std::size_t>(2));
        CHECK(round_up_to_power_of_two(static_cast<std::size_t>(3)) ==
            static_cast<std::size_t>(4));
        CHECK(round_up_to_power_of_two(static_cast<std::size_t>(4)) ==
            static_cast<std::size_t>(4));
        CHECK(round_up_to_power_of_two(static_cast<std::size_t>(5)) ==
            static_cast<std::size_t>(8));
        CHECK(round_up_to_power_of_two(static_cast<std::size_t>(6)) ==
            static_cast<std::size_t>(8));
        CHECK(round_up_to_power_of_two(static_cast<std::size_t>(7)) ==
            static_cast<std::size_t>(8));
        CHECK(round_up_to_power_of_two(static_cast<std::size_t>(8)) ==
            static_cast<std::size_t>(8));
        CHECK(round_up_to_power_of_two(static_cast<std::size_t>(9)) ==
            static_cast<std::size_t>(16));
    }

    SECTION("check of overflow") {
        constexpr auto largest = static_cast<std::size_t>(-1);
        constexpr std::size_t safe_limit = (largest >> 1U) + 1U;
        CHECK(round_up_to_power_of_two(safe_limit - 1U) == safe_limit);
        CHECK(round_up_to_power_of_two(safe_limit) == safe_limit);
        CHECK_THROWS(round_up_to_power_of_two(safe_limit + 1U));
    }
}
