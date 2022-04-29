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
 * \brief Test of extract_first_from_pair class.
 */
#include "hash_tables/extract_key_functions/extract_first_from_pair.h"

#include <catch2/catch_test_macros.hpp>

// NOLINTNEXTLINE
TEST_CASE("hash_tables::extract_key_functions::extract_first_from_pair") {
    using hash_tables::extract_key_functions::extract_first_from_pair;

    SECTION("extract") {
        using value_type = std::pair<int, std::string>;

        extract_first_from_pair<value_type> extract_key;

        constexpr int first = 123;
        const auto second = std::string("abc");
        CHECK(extract_key(std::make_pair(first, second)) == first);
    }
}
