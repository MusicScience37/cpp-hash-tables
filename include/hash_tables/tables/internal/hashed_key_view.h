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
#include <utility>

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

/*!
 * \brief Class to compare hashed_key_view objects.
 *
 * \tparam ActualKeyType Type of actual keys.
 * \tparam KeyEqual Type of the actual function to check whether keys are equal.
 */
template <typename ActualKeyType, typename ActualKeyEqual>
class hashed_key_view_equal {
public:
    //! Type of keys.
    using key_type = hashed_key_view<ActualKeyType>;

    /*!
     * \brief Constructor.
     *
     * \param[in] key_equal Actual function to check whether keys are equal.
     */
    explicit hashed_key_view_equal(const ActualKeyEqual& key_equal)
        : key_equal_(key_equal) {}

    /*!
     * \brief Compare two keys.
     *
     * \param[in] left Left-hand-side key.
     * \param[in] right Right-hand-side key.
     * \retval true Two keys are equal.
     * \retval false Two keys are not equal.
     */
    [[nodiscard]] auto operator()(
        const key_type& left, const key_type& right) const {
        return (left.hash_number() == right.hash_number()) &&
            key_equal_(left.key(), right.key());
    }

private:
    //! Actual function to check whether keys are equal.
    ActualKeyEqual key_equal_;
};

/*!
 * \brief Class to extract hashed_key_view objects from values.
 *
 * \tparam ActualValueType Type of actual values.
 * \tparam ActualKeyType Type of actual keys.
 * \tparam ActualExtractKey Type of the actual function to extract keys from
 * values.
 */
template <typename ActualValueType, typename ActualKeyType,
    typename ActualExtractKey>
class extract_hashed_key_view {
public:
    //! Type of values. (Actual value and its hash number.)
    using value_type = std::pair<ActualValueType, std::size_t>;

    /*!
     * \brief Constructor.
     *
     * \param[in] extract_key Actual function to extract keys from values.
     */
    explicit extract_hashed_key_view(const ActualExtractKey& extract_key)
        : extract_key_(extract_key) {}

    /*!
     * \brief Extract a hashed_key_view object.
     *
     * \param[in] value Value.
     * \return The hashed_key_view object.
     */
    [[nodiscard]] auto operator()(const value_type& value) const
        -> hashed_key_view<ActualKeyType> {
        return hashed_key_view<ActualKeyType>(
            extract_key_(value.first), value.second);
    }

private:
    //! Actual function to extract keys from values.
    ActualExtractKey extract_key_;
};

}  // namespace hash_tables::tables::internal
