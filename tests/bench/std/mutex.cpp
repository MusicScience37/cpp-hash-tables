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
 * \brief Benchmark of mutexes.
 */
#include <mutex>
#include <shared_mutex>
#include <type_traits>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/param/parameter_value_vector.h>

class fixture : public stat_bench::FixtureBase {
public:
    fixture() {
        // NOLINTNEXTLINE
        add_threads_param()->add(1)->add(2)->add(4);
    }
};

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(fixture, "lock", "std::mutex") {
    std::mutex mutex;
    STAT_BENCH_MEASURE() {
        mutex.lock();
        mutex.unlock();
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(fixture, "lock", "std::shared_mutex (unique)") {
    std::shared_mutex mutex;
    STAT_BENCH_MEASURE() {
        mutex.lock();
        mutex.unlock();
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(fixture, "lock", "std::shared_mutex (shared)") {
    std::shared_mutex mutex;
    STAT_BENCH_MEASURE() {
        mutex.lock_shared();
        mutex.unlock_shared();
    };
}

STAT_BENCH_MAIN
