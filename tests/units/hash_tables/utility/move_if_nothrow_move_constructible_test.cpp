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
 * \brief Test of move_if_nothrow_move_constructible function.
 */
#include "hash_tables/utility/move_if_nothrow_move_constructible.h"

#include <type_traits>

#include <catch2/catch_test_macros.hpp>

// NOLINTNEXTLINE
TEST_CASE("hash_tables::utility::move_if_nothrow_constructible") {
    using hash_tables::utility::move_if_nothrow_move_constructible;

    SECTION("for noexcept class") {
        struct nothrow_type {
            nothrow_type() = default;
            nothrow_type(const nothrow_type&) = default;
            nothrow_type(nothrow_type&&) noexcept = default;
            auto operator=(const nothrow_type&) -> nothrow_type& = default;
            auto operator=(nothrow_type&&) noexcept(false)
                -> nothrow_type& = default;
            ~nothrow_type() = default;
        };

        STATIC_CHECK(std::is_same_v<
            std::invoke_result_t<
                decltype(&move_if_nothrow_move_constructible<nothrow_type>),
                nothrow_type>,
            nothrow_type&&>);
    }

    SECTION("for non-noexcept class") {
        struct throw_type {
            throw_type() = default;
            throw_type(const throw_type&) noexcept = default;
            throw_type(throw_type&&) noexcept(false) = default;
            auto operator=(const throw_type&) noexcept -> throw_type& = default;
            auto operator=(throw_type&&) noexcept -> throw_type& = default;
            ~throw_type() = default;
        };

        STATIC_CHECK(std::is_same_v<
            std::invoke_result_t<
                decltype(&move_if_nothrow_move_constructible<throw_type>),
                throw_type>,
            const throw_type&>);
    }

    SECTION("move a string") {
        std::string orig{"abc"};
        std::string moved = move_if_nothrow_move_constructible(orig);
        CHECK(moved == "abc");
    }
}
