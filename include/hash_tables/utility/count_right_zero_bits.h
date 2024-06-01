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
 * \brief Definition of count_right_zero_bits function.
 */
#pragma once

#include <limits>
#include <type_traits>  // IWYU pragma: keep

namespace hash_tables::utility {

/*!
 * \brief Count the number of sequential zero bits from the least significant
 * bit.
 *
 * \tparam Integer Type of the integer.
 * \param[in] val Integer.
 * \return Count.
 */
template <typename Integer>
constexpr auto count_right_zero_bits(Integer val) {
    static_assert(std::is_integral_v<Integer>);
    static_assert(std::is_unsigned_v<Integer>);

    // TODO: efficient implementation.

    constexpr auto zero = static_cast<Integer>(0);
    constexpr auto one = static_cast<Integer>(1);

    if (val == zero) {
        return static_cast<Integer>(std::numeric_limits<Integer>::digits);
    }

    Integer count = 0U;
    while ((val & one) == zero) {
        val >>= one;
        ++count;
    }

    return count;
}

}  // namespace hash_tables::utility
