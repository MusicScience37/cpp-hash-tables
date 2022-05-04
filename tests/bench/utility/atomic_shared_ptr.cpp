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
 * \brief Benchmark to use atomic_shared_ptr class.
 */
#include "hash_tables/utility/atomic_shared_ptr.h"

#include <atomic>
#include <memory>
#include <mutex>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/util/do_not_optimize.h>

// NOLINTNEXTLINE
STAT_BENCH_CASE("store_load", "atomic_shared_ptr_relaxed") {
    hash_tables::utility::atomic_shared_ptr<int> ptr;
    const auto value = std::make_shared<int>(0);

    STAT_BENCH_MEASURE() {
        ptr.store(value, std::memory_order_relaxed);
        stat_bench::util::do_not_optimize(ptr.load(std::memory_order_relaxed));
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("store_load", "atomic_shared_ptr_acq_rel") {
    hash_tables::utility::atomic_shared_ptr<int> ptr;
    const auto value = std::make_shared<int>(0);

    STAT_BENCH_MEASURE() {
        ptr.store(value, std::memory_order_acquire);
        stat_bench::util::do_not_optimize(ptr.load(std::memory_order_release));
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("store_load", "atomic_shared_ptr_seq_cst") {
    hash_tables::utility::atomic_shared_ptr<int> ptr;
    const auto value = std::make_shared<int>(0);

    STAT_BENCH_MEASURE() {
        ptr.store(value, std::memory_order_seq_cst);
        stat_bench::util::do_not_optimize(ptr.load(std::memory_order_seq_cst));
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("store_load", "mutex_shared_ptr") {
    std::shared_ptr<int> ptr;
    std::mutex mutex;
    const auto value = std::make_shared<int>(0);

    STAT_BENCH_MEASURE() {
        do {
            std::unique_lock<std::mutex> lock(mutex);
            ptr = value;
            lock.unlock();
        } while (false);
        do {
            std::unique_lock<std::mutex> lock(mutex);
            auto copy = ptr;
            stat_bench::util::do_not_optimize(copy);
            lock.unlock();
        } while (false);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("CAS", "atomic_shared_ptr_weak") {
    hash_tables::utility::atomic_shared_ptr<int> ptr;
    const auto value = std::make_shared<int>(0);
    std::shared_ptr<int> expected;

    STAT_BENCH_MEASURE() { ptr.compare_exchange_weak(expected, value); };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("CAS", "mutex_shared_ptr") {
    std::shared_ptr<int> ptr;
    std::mutex mutex;
    const auto value = std::make_shared<int>(0);
    std::shared_ptr<int> expected;

    STAT_BENCH_MEASURE() {
        std::unique_lock<std::mutex> lock(mutex);
        if (ptr == expected) {
            ptr = value;
        } else {
            expected = ptr;
        }
    };
}

STAT_BENCH_MAIN
