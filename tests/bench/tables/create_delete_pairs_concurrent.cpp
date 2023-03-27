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
 * \brief Benchmark to create and delete pairs in tables concurrently.
 */
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <fmt/core.h>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/current_invocation_context.h>
#include <stat_bench/do_not_optimize.h>
#include <stat_bench/fixture_base.h>
#include <stat_bench/invocation_context.h>
#include <stat_bench/param/parameter_value_vector.h>

#include "hash_tables/extract_key_functions/extract_first_from_pair.h"
#include "hash_tables/hashes/std_hash.h"
#include "hash_tables/tables/multi_open_address_table_mt.h"
#include "hash_tables/tables/open_address_table_st.h"
#include "hash_tables/tables/separate_shared_chain_table_mt.h"
#include "hash_tables_test/create_random_int_vector.h"
#include "hash_tables_test/create_random_string_vector.h"

using key_type = int;
using value_type = std::pair<int, std::string>;
using extract_key =
    hash_tables::extract_key_functions::extract_first_from_pair<value_type>;

class create_delete_pairs_concurrent : public stat_bench::FixtureBase {
public:
    create_delete_pairs_concurrent() {
        add_param<std::size_t>("size")
            ->add(100)   // NOLINT
            ->add(1000)  // NOLINT
#ifdef NDEBUG
            ->add(10000)   // NOLINT
            ->add(100000)  // NOLINT
#endif
            ;
        // NOLINTNEXTLINE
        add_threads_param()->add(1)->add(2)->add(4);
    }

    void setup(stat_bench::InvocationContext& context) override {
        size_ = context.get_param<std::size_t>("size");
        keys_ = hash_tables_test::create_random_int_vector<key_type>(size_);
        second_values_ = hash_tables_test::create_random_string_vector(size_);
    }

protected:
    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::size_t size_{};

    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::vector<key_type> keys_{};

    // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
    std::vector<std::string> second_values_{};
};

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(create_delete_pairs_concurrent,
    "create_delete_pairs_concurrent", "mutex_open_address_st") {
    hash_tables::tables::open_address_table_st<value_type, key_type,
        extract_key>
        table;
    std::mutex mutex;

    const std::size_t num_threads =
        stat_bench::current_invocation_context().threads();
    const std::size_t size_per_thread = (size_ + num_threads - 1) / num_threads;

    STAT_BENCH_MEASURE_INDEXED(thread_ind, /*sample_ind*/, /*iteration_ind*/) {
        const std::size_t begin_ind = thread_ind * size_per_thread;
        const std::size_t end_ind =
            std::min((thread_ind + 1) * size_per_thread, size_);
        for (std::size_t i = begin_ind; i < end_ind; ++i) {
            const auto& key = keys_.at(i);
            const auto& second_value = second_values_.at(i);
            std::unique_lock<std::mutex> lock(mutex);
            table.emplace(key, key, second_value);
        }
        for (std::size_t i = begin_ind; i < end_ind; ++i) {
            const auto& key = keys_.at(i);
            std::unique_lock<std::mutex> lock(mutex);
            table.erase(key);
        }
    };

    assert(table.empty());  // NOLINT
    stat_bench::do_not_optimize(table);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(create_delete_pairs_concurrent,
    "create_delete_pairs_concurrent", "multi_open_address_mt") {
    hash_tables::tables::multi_open_address_table_mt<value_type, key_type,
        extract_key>
        table;

    const std::size_t num_threads =
        stat_bench::current_invocation_context().threads();
    const std::size_t size_per_thread = (size_ + num_threads - 1) / num_threads;

    STAT_BENCH_MEASURE_INDEXED(thread_ind, /*sample_ind*/, /*iteration_ind*/) {
        const std::size_t begin_ind = thread_ind * size_per_thread;
        const std::size_t end_ind =
            std::min((thread_ind + 1) * size_per_thread, size_);
        for (std::size_t i = begin_ind; i < end_ind; ++i) {
            const auto& key = keys_.at(i);
            const auto& second_value = second_values_.at(i);
            table.emplace(key, key, second_value);
        }
        for (std::size_t i = begin_ind; i < end_ind; ++i) {
            const auto& key = keys_.at(i);
            table.erase(key);
        }
    };

    assert(table.empty());  // NOLINT
    stat_bench::do_not_optimize(table);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(create_delete_pairs_concurrent,
    "create_delete_pairs_concurrent", "shared_chain_mt") {
    hash_tables::tables::separate_shared_chain_table_mt<value_type, key_type,
        extract_key>
        table;

    const std::size_t num_threads =
        stat_bench::current_invocation_context().threads();
    const std::size_t size_per_thread = (size_ + num_threads - 1) / num_threads;

    STAT_BENCH_MEASURE_INDEXED(thread_ind, /*sample_ind*/, /*iteration_ind*/) {
        const std::size_t begin_ind = thread_ind * size_per_thread;
        const std::size_t end_ind =
            std::min((thread_ind + 1) * size_per_thread, size_);
        for (std::size_t i = begin_ind; i < end_ind; ++i) {
            const auto& key = keys_.at(i);
            const auto& second_value = second_values_.at(i);
            table.emplace(key, key, second_value);
        }
        for (std::size_t i = begin_ind; i < end_ind; ++i) {
            const auto& key = keys_.at(i);
            table.erase(key);
        }
    };

    assert(table.empty());  // NOLINT
    stat_bench::do_not_optimize(table);
}
