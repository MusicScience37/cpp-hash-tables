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
 * \brief Definition of hash_cache class.
 */
#pragma once

#include <cstddef>

#include "hash_tables/hashes/default_hash.h"
#include "hash_tables/hashes/std_hash.h"

namespace hash_tables::hashes {

/*!
 * \brief Class to cache hash numbers with keys.
 *
 * \tparam KeyType Type of keys.
 * \tparam HashType Type of the hash function.
 */
template <typename KeyType, typename HashType = default_hash<KeyType>>
class hash_cache {
public:
    //! Type of keys.
    using key_type = KeyType;

    //! Type of the hash function.
    using hash_type = HashType;

    //! Type of hash numbers.
    using hash_number_type = std::size_t;

    /*!
     * \brief Constructor.
     *
     * \param[in] key Key.
     * \param[in] hash Hash function.
     */
    hash_cache(const key_type& key,  // NOLINT
        const hash_type& hash = hash_type())
        : key_(key), hash_number_(hash(key)) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] key Key.
     * \param[in] hash Hash function.
     */
    hash_cache(key_type&& key,  // NOLINT
        const hash_type& hash = hash_type())
        : key_(key), hash_number_(hash(key)) {}

    /*!
     * \brief Get teh key.
     *
     * \return Key.
     */
    [[nodiscard]] auto key() const noexcept -> const key_type& { return key_; }

    /*!
     * \brief Get the hash number.
     *
     * \return Hash number.
     */
    [[nodiscard]] auto hash_number() const noexcept -> hash_number_type {
        return hash_number_;
    }

    /*!
     * \brief Convert to the key.
     *
     * \return Key.
     */
    [[nodiscard]] explicit operator key_type() const { return key_; }

    /*!
     * \brief Compare with another object.
     *
     * \param[in] right Right-hand-side object.
     * \retval true Two object has the same key.
     * \retval false Two object has the different key.
     */
    [[nodiscard]] auto operator==(const hash_cache& right) const -> bool {
        return (hash_number_ == right.hash_number_) && (key_ == right.key_);
    }

    /*!
     * \brief Compare with another object.
     *
     * \param[in] right Right-hand-side object.
     * \retval true Two object has the different key.
     * \retval false Two object has the same key.
     */
    [[nodiscard]] auto operator!=(const hash_cache& right) const -> bool {
        return !operator==(right);
    }

private:
    //! Key.
    key_type key_;

    //! Hash number.
    hash_number_type hash_number_;
};

/*!
 * \brief Specialization of hash_tables::hashes::std_hash for
 * hash_tables::hashes::hash_cache.
 *
 * \tparam KeyType Type of the actual keys.
 * \tparam HashType Type of the hash number.
 */
template <typename KeyType, typename HashType>
class std_hash<hash_cache<KeyType, HashType>> {
public:
    //! Type of keys.
    using key_type = hash_cache<KeyType, HashType>;

    //! Type of hash numbers.
    using hash_number_type = std::size_t;

    /*!
     * \brief Get the hash number.
     *
     * \param[in] key Key.
     * \return Hash number.
     */
    [[nodiscard]] auto operator()(
        const key_type& key) const -> hash_number_type {
        return key.hash_number();
    }
};

}  // namespace hash_tables::hashes
