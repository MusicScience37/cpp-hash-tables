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
 * \brief Test to create values in sets.
 */
#include <cassert>
#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include <fmt/core.h>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/do_not_optimize.h>
#include <stat_bench/fixture_base.h>
#include <stat_bench/invocation_context.h>
#include <stat_bench/param/parameter_value_vector.h>

#include "hash_tables/hashes/std_hash.h"
#include "hash_tables/sets/open_address_set_st.h"
#include "hash_tables_test/create_random_string_vector.h"

using key_type = std::string;

class create_values_fixture : public stat_bench::FixtureBase {
public:
    create_values_fixture() {
        add_param<std::size_t>("size")
            ->add(10)    // NOLINT
            ->add(100)   // NOLINT
            ->add(1000)  // NOLINT
#ifdef HASH_TABLES_ENABLE_HEAVY_BENCH
            ->add(10000)  // NOLINT
#endif
            ;
    }

    void setup(stat_bench::InvocationContext& context) override {
        size_ = context.get_param<std::size_t>("size");
        keys_ = hash_tables_test::create_random_string_vector(size_);
    }

protected:
    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::size_t size_{};

    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::vector<key_type> keys_{};
};

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(create_values_fixture, "create_values", "unordered_set") {
    STAT_BENCH_MEASURE() {
        std::unordered_set<key_type> set;
        set.reserve(size_);
        for (std::size_t i = 0; i < size_; ++i) {
            const auto& key = keys_.at(i);
            set.insert(key);
        }
        assert(set.size() == size_);  // NOLINT
        stat_bench::do_not_optimize(set);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(create_values_fixture, "create_values", "open_address_st") {
    STAT_BENCH_MEASURE() {
        hash_tables::sets::open_address_set_st<key_type> set;
        set.reserve(size_);
        for (std::size_t i = 0; i < size_; ++i) {
            const auto& key = keys_.at(i);
            set.insert(key);
        }
        assert(set.size() == size_);  // NOLINT
        stat_bench::do_not_optimize(set);
    };
}

STAT_BENCH_MAIN
