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
 * \brief Test to find pairs with string keys in maps.
 */
#include <cassert>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <fmt/core.h>
#include <stat_bench/bench/invocation_context.h>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/param/parameter_value_vector.h>
#include <stat_bench/util/do_not_optimize.h>

#include "hash_tables/hashes/hash_cache.h"
#include "hash_tables/hashes/std_hash.h"
#include "hash_tables/maps/open_address_map_st.h"
#include "hash_tables_test/create_random_int_vector.h"

using key_type = std::string;
using mapped_type = int;

class fixture : public stat_bench::FixtureBase {
public:
    fixture() {
        add_param<std::size_t>("size")
            ->add(10)    // NOLINT
            ->add(100)   // NOLINT
            ->add(1000)  // NOLINT
#ifdef NDEBUG
            ->add(10000)  // NOLINT
#endif
            ;
    }

    void setup(stat_bench::bench::InvocationContext& context) override {
        size_ = context.get_param<std::size_t>("size");
        second_values_ = hash_tables_test::create_random_int_vector<int>(size_);
        keys_.clear();
        keys_.reserve(second_values_.size());
        for (const auto& second_value : second_values_) {
            keys_.push_back(std::to_string(second_value));
        }
    }

protected:
    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::size_t size_{};

    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::vector<key_type> keys_{};

    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::vector<mapped_type> second_values_{};
};

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(fixture, "find_with_string_keys", "unordered_map") {
    std::unordered_map<key_type, mapped_type> map;
    map.reserve(size_);
    for (std::size_t i = 0; i < size_; ++i) {
        const auto& key = keys_.at(i);
        const auto& second_value = second_values_.at(i);
        map.try_emplace(key, second_value);
    }
    assert(map.size() == size_);  // NOLINT

    STAT_BENCH_MEASURE() {
        for (std::size_t i = 0; i < size_; ++i) {
            const auto& key = keys_.at(i);
            stat_bench::util::do_not_optimize(map.at(key));
        };
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(fixture, "find_with_string_keys", "open_address_st") {
    hash_tables::maps::open_address_map_st<key_type, mapped_type> map;
    map.reserve(size_);
    for (std::size_t i = 0; i < size_; ++i) {
        const auto& key = keys_.at(i);
        const auto& second_value = second_values_.at(i);
        map.emplace(key, second_value);
    }
    assert(map.size() == size_);  // NOLINT

    STAT_BENCH_MEASURE() {
        for (std::size_t i = 0; i < size_; ++i) {
            const auto& key = keys_.at(i);
            stat_bench::util::do_not_optimize(map.at(key));
        };
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(fixture, "find_with_string_keys", "open_address_st_cache") {
    hash_tables::maps::open_address_map_st<
        hash_tables::hashes::hash_cache<key_type>, mapped_type>
        map;
    map.reserve(size_);
    for (std::size_t i = 0; i < size_; ++i) {
        const auto& key = keys_.at(i);
        const auto& second_value = second_values_.at(i);
        map.emplace(key, second_value);
    }
    assert(map.size() == size_);  // NOLINT

    STAT_BENCH_MEASURE() {
        for (std::size_t i = 0; i < size_; ++i) {
            const auto& key = keys_.at(i);
            stat_bench::util::do_not_optimize(map.at(key));
        };
    };
}

STAT_BENCH_MAIN
