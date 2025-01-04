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
 * \brief Benchmark to check existence of keys in tables.
 */
// IWYU pragma: no_include <assert.h>
#include <cassert>
#include <cstddef>
#include <exception>
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
#include "hash_tables/tables/open_address_table_st.h"
#include "hash_tables_test/create_random_int_vector.h"
#include "hash_tables_test/create_random_string_vector.h"

using key_type = int;
using value_type = std::pair<int, std::string>;
using extract_key =
    hash_tables::extract_key_functions::extract_first_from_pair<value_type>;

class check_existence_fixture : public stat_bench::FixtureBase {
public:
    check_existence_fixture() {
        add_param<std::size_t>("size")
            ->add(100)   // NOLINT
            ->add(1000)  // NOLINT
#ifdef HASH_TABLES_ENABLE_HEAVY_BENCH
            ->add(10000)   // NOLINT
            ->add(100000)  // NOLINT
#endif
            ;
        add_param<std::string>("exist")->add("yes")->add("no");
    }

    void setup(stat_bench::InvocationContext& context) override {
        size_ = context.get_param<std::size_t>("size");
        const bool key_exist = context.get_param<std::string>("exist") == "yes";
        if (key_exist) {
            keys_ = hash_tables_test::create_random_int_vector<key_type>(size_);
            keys_search_ = keys_;
        } else {
            const auto keys =
                hash_tables_test::create_random_int_vector<key_type>(
                    size_ * 2U);
            keys_ = std::vector<key_type>(keys.begin(),
                keys.begin() + static_cast<std::ptrdiff_t>(size_));
            keys_search_ = std::vector<key_type>(
                keys.begin() + static_cast<std::ptrdiff_t>(size_), keys.end());
        }
        second_values_ = hash_tables_test::create_random_string_vector(size_);
    }

protected:
    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::size_t size_{};

    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::vector<key_type> keys_{};

    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::vector<key_type> keys_search_{};

    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::vector<std::string> second_values_{};
};

STAT_BENCH_GROUP("check_existence").add_parameter_to_time_plot_log("size");

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    check_existence_fixture, "check_existence", "open_address_st") {
    hash_tables::tables::open_address_table_st<value_type, key_type,
        extract_key>
        table;
    for (std::size_t i = 0; i < size_; ++i) {
        const auto& key = keys_.at(i);
        const auto& second_value = second_values_.at(i);
        table.emplace(key, key, second_value);
    }
    assert(table.size() == size_);  // NOLINT

    STAT_BENCH_MEASURE() {
        for (std::size_t i = 0; i < size_; ++i) {
            stat_bench::do_not_optimize(table.try_get(keys_.at(i)));
        }
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    check_existence_fixture, "check_existence", "multi_open_address_mt") {
    hash_tables::tables::multi_open_address_table_mt<value_type, key_type,
        extract_key>
        table;
    for (std::size_t i = 0; i < size_; ++i) {
        const auto& key = keys_.at(i);
        const auto& second_value = second_values_.at(i);
        table.emplace(key, key, second_value);
    }
    assert(table.size() == size_);  // NOLINT

    STAT_BENCH_MEASURE() {
        for (std::size_t i = 0; i < size_; ++i) {
            stat_bench::do_not_optimize(table.try_get(keys_.at(i)));
        }
    };
}
