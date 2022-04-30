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
 * \brief Definition of fixed_hash class.
 */
#pragma once

#include <functional>

namespace hash_tables_test::hashes {

/*!
 * \brief Class of hash function returning fixed values.
 *
 * \tparam KeyType Type of keys.
 */
template <typename KeyType>
class fixed_hash {
public:
    //! Type of keys.
    using key_type = KeyType;

    //! Type of hash numbers.
    using hash_number_type = std::size_t;

    /*!
     * \brief Constructor.
     *
     * \param[in] number Hash number.
     */
    explicit fixed_hash(
        hash_number_type number = static_cast<hash_number_type>(-1))
        : number_(number) {}

    /*!
     * \brief Calculate a hash number.
     *
     * \param[in] key Key.
     * \return Hash number.
     */
    [[nodiscard]] auto operator()(const KeyType& key) const
        -> hash_number_type {
        (void)key;
        return number_;
    }

private:
    //! Hash number.
    hash_number_type number_;
};

}  // namespace hash_tables_test::hashes
