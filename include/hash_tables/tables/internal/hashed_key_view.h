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
 * \brief Definition of hashed_key_view class.
 */
#pragma once

#include <cstddef>

namespace hash_tables::tables::internal {

/*!
 * \brief Class to temporarily hold a pair of a reference to a key and the hash
 * number of the key.
 *
 * \tparam KeyType Type of keys.
 */
template <typename KeyType>
class hashed_key_view {
public:
    //! Type of keys.
    using key_type = KeyType;

    //! Type of hash numbers.
    using hash_number_type = std::size_t;

    /*!
     * \brief Constructor.
     *
     * \param[in] key Key.
     * \param[in] hash_number Hash number.
     */
    hashed_key_view(const key_type& key, std::size_t hash_number)
        : key_(&key), hash_number_(hash_number) {}

    /*!
     * \brief Get the key.
     *
     * \return Key.
     */
    [[nodiscard]] auto key() const noexcept -> const key_type& { return *key_; }

    /*!
     * \brief Get the hash number.
     *
     * \return Hash number.
     */
    [[nodiscard]] auto hash_number() const noexcept -> hash_number_type {
        return hash_number_;
    }

    /*!
     * \brief Compare with another object.
     *
     * \param[in] other Another object.
     * \retval true This object is same with the given object.
     * \retval false This object is not same with the given object.
     */
    auto operator==(const hashed_key_view& other) const noexcept -> bool {
        return (hash_number_ == other.hash_number_) && (*key_ == *other.key_);
    }

    /*!
     * \brief Compare with another object.
     *
     * \param[in] other Another object.
     * \retval true This object is not same with the given object.
     * \retval false This object is same with the given object.
     */
    auto operator!=(const hashed_key_view& other) const noexcept -> bool {
        return !operator==(other);
    }

private:
    //! Key.
    const key_type* key_;

    //! Hash number.
    hash_number_type hash_number_;
};

/*!
 * \brief Class of hash function of hashed_key_view objects.
 *
 * \tparam ActualKeyType Type of actual keys.
 */
template <typename ActualKeyType>
class hashed_key_view_hash {
public:
    //! Type of keys.
    using key_type = hashed_key_view<ActualKeyType>;

    //! Type of hash numbers.
    using hash_number_type = std::size_t;

    /*!
     * \brief Calculate a hash number.
     *
     * \param[in] key Key.
     * \return Hash number.
     */
    [[nodiscard]] auto operator()(const key_type& key) const {
        return key.hash_number();
    }
};

}  // namespace hash_tables::tables::internal
