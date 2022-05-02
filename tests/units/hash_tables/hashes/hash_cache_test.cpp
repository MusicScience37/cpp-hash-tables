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
 * \brief Test of hash_cache class.
 */
#include "hash_tables/hashes/hash_cache.h"

#include <catch2/catch_test_macros.hpp>

#include "hash_tables/hashes/default_hash.h"

// NOLINTNEXTLINE
TEST_CASE("hash_tables::hashes::hash_cache") {
    using hash_tables::hashes::default_hash;
    using hash_tables::hashes::hash_cache;

    SECTION("construct") {
        const auto key = std::string("abc");
        const hash_cache<std::string> cache = key;
        CHECK(cache.key() == key);
        CHECK(static_cast<std::string>(cache) == key);
        CHECK(cache.hash_number() == default_hash<std::string>()(key));
    }

    SECTION("construct with move") {
        const auto key = std::string("abc");
        const hash_cache<std::string> cache{std::string(key)};
        CHECK(cache.key() == key);
        CHECK(static_cast<std::string>(cache) == key);
        CHECK(cache.hash_number() == default_hash<std::string>()(key));
    }
}
