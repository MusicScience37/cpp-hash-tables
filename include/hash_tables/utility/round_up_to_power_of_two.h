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
 * \brief Definition of round_up_to_power_of_two function.
 */
#pragma once

#include <limits>
#include <stdexcept>
#include <type_traits>

namespace hash_tables::utility {

namespace internal {

#ifndef HASH_TABLES_DOCUMENTATION
template <typename Integer, unsigned int TotalDigits,
    unsigned int ShiftBitsInThisIteration,
    std::enable_if_t<(ShiftBitsInThisIteration >= TotalDigits), void*> =
        nullptr>
inline constexpr auto propagate_bits_to_right_impl(Integer val) noexcept
    -> Integer {
    return val;
}
#endif

/*!
 * \brief Propagate bits in an integer to right.
 *
 * \tparam Integer Type of the integer.
 * \tparam TotalDigits Number of total digits of the integer.
 * \tparam ShiftBitsInThisIteration Number of bits shifted in this iteration.
 * \param[in] val Integer value.
 * \return Shifted value.
 */
template <typename Integer, unsigned int TotalDigits,
    unsigned int ShiftBitsInThisIteration
#ifndef HASH_TABLES_DOCUMENTATION
    ,
    std::enable_if_t<(ShiftBitsInThisIteration < TotalDigits), void*> = nullptr
#endif
    >
inline constexpr auto propagate_bits_to_right_impl(Integer val) -> Integer {
    return propagate_bits_to_right_impl<Integer, TotalDigits,
        (ShiftBitsInThisIteration << 1U)>(
        val | val >> ShiftBitsInThisIteration);
}

/*!
 * \brief Propagate bits in an integer to right.
 *
 * \tparam Integer Type of the integer.
 * \tparam TotalDigits Number of total digits of the integer.
 * \param[in] val Integer value.
 * \return Shifted value.
 */
template <typename Integer,
    unsigned int TotalDigits = static_cast<unsigned int>(
        std::numeric_limits<Integer>::digits)>
inline constexpr auto propagate_bits_to_right(Integer val) noexcept -> Integer {
    static_assert(std::is_integral_v<Integer>);
    static_assert(std::is_unsigned_v<Integer>);
    return propagate_bits_to_right_impl<Integer, TotalDigits, 1U>(val);
}

}  // namespace internal

/*!
 * \brief Round an integer up to a power of two.
 *
 * \tparam Integer Type of the integer.
 * \param[in] val Integer value.
 * \return Result.
 */
template <typename Integer>
inline constexpr auto round_up_to_power_of_two(Integer val) {
    static_assert(std::is_integral_v<Integer>);
    static_assert(std::is_unsigned_v<Integer>);

    constexpr auto one = static_cast<Integer>(1);

    if (val == static_cast<Integer>(0)) {
        return one;
    }

    val -= one;
    val = internal::propagate_bits_to_right(val);
    if (val == static_cast<Integer>(-1)) {
        throw std::logic_error("Overflow in finding the next power of two.");
    }
    val += one;
    return val;
}

}  // namespace hash_tables::utility
