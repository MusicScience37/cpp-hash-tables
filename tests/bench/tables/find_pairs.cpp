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
 * \brief Benchmark to find pairs in tables.
 */
#include <cassert>
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <fmt/core.h>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/do_not_optimize.h>
#include <stat_bench/fixture_base.h>
#include <stat_bench/invocation_context.h>
#include <stat_bench/param/parameter_value_vector.h>

#include "hash_tables/extract_key_functions/extract_first_from_pair.h"
#include "hash_tables/hashes/std_hash.h"
#include "hash_tables/tables/multi_open_address_table_mt.h"
#include "hash_tables/tables/multi_open_address_table_st.h"
#include "hash_tables/tables/open_address_table_st.h"
#include "hash_tables/tables/separate_shared_chain_table_mt.h"
#include "hash_tables_test/create_random_int_vector.h"
#include "hash_tables_test/create_random_string_vector.h"

using key_type = int;
using value_type = std::pair<int, std::string>;
using extract_key =
    hash_tables::extract_key_functions::extract_first_from_pair<value_type>;

class find_pairs_fixture : public stat_bench::FixtureBase {
public:
    find_pairs_fixture() {
        // NOLINTNEXTLINE
        add_param<std::size_t>("size")->add(10)->add(1000);
        // NOLINTNEXTLINE
        add_param<float>("load")->add(0.1)->add(0.2)->add(0.5)->add(0.8);
    }

    void setup(stat_bench::InvocationContext& context) override {
        size_ = context.get_param<std::size_t>("size");
        max_load_factor_ = context.get_param<float>("load");
        keys_ = hash_tables_test::create_random_int_vector<key_type>(size_);
        second_values_ = hash_tables_test::create_random_string_vector(size_);
    }

protected:
    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::size_t size_{};

    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    float max_load_factor_{};

    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::vector<key_type> keys_{};

    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::vector<std::string> second_values_{};
};

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(find_pairs_fixture, "find_pairs", "open_address_st") {
    hash_tables::tables::open_address_table_st<value_type, key_type,
        extract_key>
        table;
    table.max_load_factor(max_load_factor_);
    table.reserve(size_);
    for (std::size_t i = 0; i < size_; ++i) {
        const auto& key = keys_.at(i);
        const auto& second_value = second_values_.at(i);
        table.emplace(key, key, second_value);
    }
    assert(table.size() == size_);  // NOLINT

    STAT_BENCH_MEASURE() {
        for (std::size_t i = 0; i < size_; ++i) {
            const auto& key = keys_.at(i);
            stat_bench::do_not_optimize(table.at(keys_.at(i)));
        }
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(find_pairs_fixture, "find_pairs", "multi_open_address_st") {
    hash_tables::tables::multi_open_address_table_st<value_type, key_type,
        extract_key>
        table;
    table.max_load_factor(max_load_factor_);
    table.reserve(size_);
    for (std::size_t i = 0; i < size_; ++i) {
        const auto& key = keys_.at(i);
        const auto& second_value = second_values_.at(i);
        table.emplace(key, key, second_value);
    }
    assert(table.size() == size_);  // NOLINT

    STAT_BENCH_MEASURE() {
        for (std::size_t i = 0; i < size_; ++i) {
            const auto& key = keys_.at(i);
            stat_bench::do_not_optimize(table.at(keys_.at(i)));
        }
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(find_pairs_fixture, "find_pairs", "multi_open_address_mt") {
    hash_tables::tables::multi_open_address_table_mt<value_type, key_type,
        extract_key>
        table;
    table.max_load_factor(max_load_factor_);
    table.reserve(size_);
    for (std::size_t i = 0; i < size_; ++i) {
        const auto& key = keys_.at(i);
        const auto& second_value = second_values_.at(i);
        table.emplace(key, key, second_value);
    }
    assert(table.size() == size_);  // NOLINT

    STAT_BENCH_MEASURE() {
        for (std::size_t i = 0; i < size_; ++i) {
            const auto& key = keys_.at(i);
            stat_bench::do_not_optimize(table.at(keys_.at(i)));
        }
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(find_pairs_fixture, "find_pairs", "shared_chain_mt") {
    const auto min_num_buckets =
        static_cast<std::size_t>(static_cast<float>(size_) / max_load_factor_);
    hash_tables::tables::separate_shared_chain_table_mt<value_type, key_type,
        extract_key>
        table{min_num_buckets};
    for (std::size_t i = 0; i < size_; ++i) {
        const auto& key = keys_.at(i);
        const auto& second_value = second_values_.at(i);
        table.emplace(key, key, second_value);
    }
    assert(table.size() == size_);  // NOLINT

    STAT_BENCH_MEASURE() {
        for (std::size_t i = 0; i < size_; ++i) {
            const auto& key = keys_.at(i);
            stat_bench::do_not_optimize(table.at(keys_.at(i)));
        }
    };
}
