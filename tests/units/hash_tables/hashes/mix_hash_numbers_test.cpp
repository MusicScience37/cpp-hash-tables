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
 * \brief Test of mix_hash_numbers class.
 */
#include "hash_tables/hashes/mix_hash_numbers.h"

#include <catch2/catch_test_macros.hpp>

// NOLINTNEXTLINE
TEST_CASE("hash_tables::hashes::mix_hash_numbers") {
    using hash_tables::hashes::mix_hash_numbers;

    SECTION("check change of hash number in 32 bit") {
        constexpr std::uint32_t num1 = 0x8f624058U;
        constexpr std::uint32_t num2 = 0xf3763350U;

        std::uint32_t res = num1;
        mix_hash_numbers(res, num2);
        const std::uint32_t res_orig = res;

        res = num1;
        mix_hash_numbers(res, num2);
        CHECK(res == res_orig);

        res = num1 + 1;
        mix_hash_numbers(res, num2);
        CHECK(res != res_orig);

        res = num1 - 1;
        mix_hash_numbers(res, num2);
        CHECK(res != res_orig);

        res = num1;
        mix_hash_numbers(res, num2 + 1);
        CHECK(res != res_orig);

        res = num1;
        mix_hash_numbers(res, num2 - 1);
        CHECK(res != res_orig);
    }

    SECTION("check change of hash number in 64 bit") {
        constexpr std::uint64_t num1 = 0x5bd94a70e01d4726ULL;
        constexpr std::uint64_t num2 = 0x8e0caceed7825347ULL;

        std::uint64_t res = num1;
        mix_hash_numbers(res, num2);
        const std::uint64_t res_orig = res;

        res = num1;
        mix_hash_numbers(res, num2);
        CHECK(res == res_orig);

        res = num1 + 1;
        mix_hash_numbers(res, num2);
        CHECK(res != res_orig);

        res = num1 - 1;
        mix_hash_numbers(res, num2);
        CHECK(res != res_orig);

        res = num1;
        mix_hash_numbers(res, num2 + 1);
        CHECK(res != res_orig);

        res = num1;
        mix_hash_numbers(res, num2 - 1);
        CHECK(res != res_orig);
    }

    SECTION("check change of hash number in std::size_t") {
        constexpr auto num1 = static_cast<std::size_t>(0x5bd94a70e01d4726ULL);
        constexpr auto num2 = static_cast<std::size_t>(0x8e0caceed7825347ULL);

        std::size_t res = num1;
        mix_hash_numbers(res, num2);
        const std::size_t res_orig = res;

        res = num1;
        mix_hash_numbers(res, num2);
        CHECK(res == res_orig);

        res = num1 + 1;
        mix_hash_numbers(res, num2);
        CHECK(res != res_orig);

        res = num1 - 1;
        mix_hash_numbers(res, num2);
        CHECK(res != res_orig);

        res = num1;
        mix_hash_numbers(res, num2 + 1);
        CHECK(res != res_orig);

        res = num1;
        mix_hash_numbers(res, num2 - 1);
        CHECK(res != res_orig);
    }
}
