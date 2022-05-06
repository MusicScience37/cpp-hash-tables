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
 * \brief Test to create and delete pairs in maps.
 */
#include <cassert>
#include <cstddef>
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

#include "hash_tables/hashes/std_hash.h"
#include "hash_tables/maps/open_address_map_st.h"
#include "hash_tables/maps/separate_shared_chain_map_mt.h"
#include "hash_tables_test/create_random_int_vector.h"

using key_type = int;
using mapped_type = std::string;

class fixture : public stat_bench::FixtureBase {
public:
    fixture() {
        add_param<std::size_t>("size")
            ->add(100)   // NOLINT
            ->add(1000)  // NOLINT
#ifdef NDEBUG
            ->add(10000)  // NOLINT
#endif
            ;
        add_threads_param()->add(1)->add(2)->add(4);
    }

    void setup(stat_bench::bench::InvocationContext& context) override {
        size_ = context.get_param<std::size_t>("size");
        keys_ = hash_tables_test::create_random_int_vector<key_type>(size_);
        second_values_.clear();
        second_values_.reserve(keys_.size());
        for (const auto& key : keys_) {
            second_values_.push_back(std::to_string(key));
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
STAT_BENCH_CASE_F(fixture, "create_delete_pairs", "mutex_unordered_map") {
    std::unordered_map<key_type, mapped_type> map{2U * size_};
    std::mutex mutex;

    const std::size_t num_threads = STAT_BENCH_CONTEXT_NAME.threads();
    const std::size_t size_per_thread = (size_ + num_threads - 1) / num_threads;

    STAT_BENCH_MEASURE_INDEXED(thread_ind, /*sample_ind*/, /*iteration_ind*/) {
        const std::size_t begin_ind = thread_ind * size_per_thread;
        const std::size_t end_ind =
            std::min((thread_ind + 1) * size_per_thread, size_);
        for (std::size_t i = begin_ind; i < end_ind; ++i) {
            const auto& key = keys_.at(i);
            const auto& second_value = second_values_.at(i);
            std::unique_lock<std::mutex> lock(mutex);
            map.emplace(key, second_value);
        }
        for (std::size_t i = begin_ind; i < end_ind; ++i) {
            const auto& key = keys_.at(i);
            std::unique_lock<std::mutex> lock(mutex);
            map.erase(key);
        }
    };

    stat_bench::util::do_not_optimize(map);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(fixture, "create_delete_pairs", "mutex_open_address_st") {
    hash_tables::maps::open_address_map_st<key_type, mapped_type> map{
        2U * size_};
    std::mutex mutex;

    const std::size_t num_threads = STAT_BENCH_CONTEXT_NAME.threads();
    const std::size_t size_per_thread = (size_ + num_threads - 1) / num_threads;

    STAT_BENCH_MEASURE_INDEXED(thread_ind, /*sample_ind*/, /*iteration_ind*/) {
        const std::size_t begin_ind = thread_ind * size_per_thread;
        const std::size_t end_ind =
            std::min((thread_ind + 1) * size_per_thread, size_);
        for (std::size_t i = begin_ind; i < end_ind; ++i) {
            const auto& key = keys_.at(i);
            const auto& second_value = second_values_.at(i);
            std::unique_lock<std::mutex> lock(mutex);
            map.emplace(key, second_value);
        }
        for (std::size_t i = begin_ind; i < end_ind; ++i) {
            const auto& key = keys_.at(i);
            std::unique_lock<std::mutex> lock(mutex);
            map.erase(key);
        }
    };

    stat_bench::util::do_not_optimize(map);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(fixture, "create_delete_pairs", "shared_chain_mt") {
    hash_tables::maps::separate_shared_chain_map_mt<key_type, mapped_type> map{
        2U * size_};

    const std::size_t num_threads = STAT_BENCH_CONTEXT_NAME.threads();
    const std::size_t size_per_thread = (size_ + num_threads - 1) / num_threads;

    STAT_BENCH_MEASURE_INDEXED(thread_ind, /*sample_ind*/, /*iteration_ind*/) {
        const std::size_t begin_ind = thread_ind * size_per_thread;
        const std::size_t end_ind =
            std::min((thread_ind + 1) * size_per_thread, size_);
        for (std::size_t i = begin_ind; i < end_ind; ++i) {
            const auto& key = keys_.at(i);
            const auto& second_value = second_values_.at(i);
            map.emplace(key, second_value);
        }
        for (std::size_t i = begin_ind; i < end_ind; ++i) {
            const auto& key = keys_.at(i);
            map.erase(key);
        }
    };

    stat_bench::util::do_not_optimize(map);
}

STAT_BENCH_MAIN
