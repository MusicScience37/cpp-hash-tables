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
 * \brief Definition of mix_hash_numbers function.
 */
#pragma once

#include <cstdint>

namespace hash_tables::hashes {

/*!
 * \brief Mix two hash numbers.
 *
 * \param[in,out] to A hash number. Result is assigned to this variable.
 * \param[in] number A hash number. Used only for an input.
 *
 * \note This implementation is based on `hash_combine` in Boost.
 * (
 * https://github.com/boostorg/container_hash/blob/boost-1.55.0/include/boost/functional/hash/hash.hpp
 * )
 */
inline void mix_hash_numbers(std::uint32_t& to, std::uint32_t number) {
    constexpr std::uint32_t offset = 0x9E3779B9;
    constexpr unsigned int left_digits = 6;
    constexpr unsigned int right_digits = 2;
    to ^= number + offset + (to << left_digits) + (to >> right_digits);
}

/*!
 * \brief Mix two hash numbers.
 *
 * \param[in,out] to A hash number. Result is assigned to this variable.
 * \param[in] number A hash number. Used only for an input.
 *
 * \note This implementation is based on `hash_combine` in Boost.
 * (
 * https://github.com/boostorg/container_hash/blob/boost-1.55.0/include/boost/functional/hash/hash.hpp
 * )
 */
inline void mix_hash_numbers(std::uint64_t& to, std::uint64_t number) {
    constexpr std::uint64_t offset = 0x9E3779B97F4A7C15;
    constexpr unsigned int left_digits = 12;
    constexpr unsigned int right_digits = 4;
    to ^= number + offset + (to << left_digits) + (to >> right_digits);
}

}  // namespace hash_tables::hashes
