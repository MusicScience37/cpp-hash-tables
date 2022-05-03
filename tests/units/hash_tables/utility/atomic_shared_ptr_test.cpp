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
 * \brief Test of atomic_shared_ptr class.
 */
#include "hash_tables/utility/atomic_shared_ptr.h"

#include <atomic>
#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>

// NOLINTNEXTLINE
TEST_CASE("hash_tables::utility::atomic_shared_ptr") {
    using hash_tables::utility::atomic_shared_ptr;

    SECTION("default constructor") {
        atomic_shared_ptr<std::string> ptr;
        CHECK(ptr.load().use_count() == 0);
    }

    SECTION("constructor with pointer") {
        const auto value = std::make_shared<std::string>("abc");
        atomic_shared_ptr<std::string> ptr{value};
        CHECK(ptr.load() == value);
    }

    SECTION("is_lock_free") {
        const auto value = std::make_shared<std::string>("abc");
        atomic_shared_ptr<std::string> ptr{value};
        // Result is implementation-defined, so check only whether this function
        // is callable.
        CHECK_NOTHROW((void)ptr.is_lock_free());
    }

    SECTION("store and load") {
        SECTION("without memory order") {
            atomic_shared_ptr<std::string> ptr;

            const auto value = std::make_shared<std::string>("abc");
            CHECK_NOTHROW(ptr.store(value));

            CHECK(ptr.load() == value);
        }

        SECTION("with memory order") {
            atomic_shared_ptr<std::string> ptr;

            const auto value = std::make_shared<std::string>("abc");
            CHECK_NOTHROW(ptr.store(value, std::memory_order_relaxed));

            CHECK(ptr.load(std::memory_order_relaxed) == value);
        }
    }

    SECTION("exchange") {
        SECTION("without memory order") {
            const auto value1 = std::make_shared<std::string>("abc");
            atomic_shared_ptr<std::string> ptr{value1};

            const auto value2 = std::make_shared<std::string>("def");
            CHECK(ptr.exchange(value2) == value1);

            CHECK(ptr.load() == value2);
        }

        SECTION("with memory order") {
            const auto value1 = std::make_shared<std::string>("abc");
            atomic_shared_ptr<std::string> ptr{value1};

            const auto value2 = std::make_shared<std::string>("def");
            CHECK(ptr.exchange(value2, std::memory_order_relaxed) == value1);

            CHECK(ptr.load() == value2);
        }
    }

    SECTION("compare_exchange_weak") {
        SECTION("without memory order") {
            const auto orig = std::make_shared<std::string>("abc");
            atomic_shared_ptr<std::string> ptr{orig};

            auto expected = std::make_shared<std::string>("def");
            const auto desired = std::make_shared<std::string>("ghi");
            CHECK_FALSE(ptr.compare_exchange_weak(expected, desired));
            CHECK(expected == orig);
            CHECK(ptr.load() == orig);
        }

        SECTION("with one memory order") {
            const auto orig = std::make_shared<std::string>("abc");
            atomic_shared_ptr<std::string> ptr{orig};

            auto expected = std::make_shared<std::string>("def");
            const auto desired = std::make_shared<std::string>("ghi");
            CHECK_FALSE(ptr.compare_exchange_weak(
                expected, desired, std::memory_order_relaxed));
            CHECK(expected == orig);
            CHECK(ptr.load() == orig);
        }

        SECTION("with two memory orders") {
            const auto orig = std::make_shared<std::string>("abc");
            atomic_shared_ptr<std::string> ptr{orig};

            auto expected = std::make_shared<std::string>("def");
            const auto desired = std::make_shared<std::string>("ghi");
            CHECK_FALSE(ptr.compare_exchange_weak(expected, desired,
                std::memory_order_relaxed, std::memory_order_relaxed));
            CHECK(expected == orig);
            CHECK(ptr.load() == orig);
        }
    }

    SECTION("compare_exchange_strong") {
        SECTION("without memory order") {
            const auto orig = std::make_shared<std::string>("abc");
            atomic_shared_ptr<std::string> ptr{orig};

            auto expected = std::make_shared<std::string>("def");
            const auto desired = std::make_shared<std::string>("ghi");
            CHECK_FALSE(ptr.compare_exchange_strong(expected, desired));
            CHECK(expected == orig);
            CHECK(ptr.load() == orig);
        }

        SECTION("with one memory order") {
            const auto orig = std::make_shared<std::string>("abc");
            atomic_shared_ptr<std::string> ptr{orig};

            auto expected = std::make_shared<std::string>("def");
            const auto desired = std::make_shared<std::string>("ghi");
            CHECK_FALSE(ptr.compare_exchange_strong(
                expected, desired, std::memory_order_relaxed));
            CHECK(expected == orig);
            CHECK(ptr.load() == orig);
        }

        SECTION("with two memory orders") {
            const auto orig = std::make_shared<std::string>("abc");
            atomic_shared_ptr<std::string> ptr{orig};

            auto expected = std::make_shared<std::string>("def");
            const auto desired = std::make_shared<std::string>("ghi");
            CHECK_FALSE(ptr.compare_exchange_strong(expected, desired,
                std::memory_order_relaxed, std::memory_order_relaxed));
            CHECK(expected == orig);
            CHECK(ptr.load() == orig);
        }
    }
}
