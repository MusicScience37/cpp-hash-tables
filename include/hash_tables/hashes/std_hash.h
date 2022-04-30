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
 * \brief Definition of std_hash class.
 */
#pragma once

#include <cstddef>
#include <functional>

namespace hash_tables::hashes {

/*!
 * \brief Class of hash function in C++ STL.
 *
 * \tparam KeyType Type of keys.
 */
template <typename KeyType>
class std_hash {
public:
    //! Type of keys.
    using key_type = KeyType;

    //! Type of hash numbers.
    using hash_number_type = std::size_t;

    /*!
     * \brief Calculate a hash number.
     *
     * \param[in] key Key.
     * \return Hash number.
     */
    [[nodiscard]] auto operator()(const KeyType& key) const
        -> hash_number_type {
        return hash_(key);
    }

private:
    //! Actual hash function.
    std::hash<KeyType> hash_{};
};

}  // namespace hash_tables::hashes
