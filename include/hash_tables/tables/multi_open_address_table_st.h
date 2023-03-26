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
 * \brief Definition of multi_open_address_table_st class.
 */
#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <utility>

#include "hash_tables/hashes/default_hash.h"
#include "hash_tables/tables/internal/hashed_key_view.h"
#include "hash_tables/tables/open_address_table_st.h"
#include "hash_tables/utility/count_right_zero_bits.h"
#include "hash_tables/utility/round_up_to_power_of_two.h"

namespace hash_tables::tables {

/*!
 * \brief Class of hash tables made of multiple hash tables using open
 * addressing.
 *
 * \tparam ValueType Type of values.
 * \tparam KeyType Type of keys.
 * \tparam ExtractKey Type of the function to extract keys from values.
 * \tparam Hash Type of the hash function.
 * \tparam KeyEqual Type of the function to check whether keys are equal.
 * \tparam Allocator Type of allocators.
 *
 * \thread_safety Safe if only functions without modifications of data are
 * called.
 */
template <typename ValueType, typename KeyType, typename ExtractKey,
    typename Hash = hashes::default_hash<KeyType>,
    typename KeyEqual = std::equal_to<KeyType>,
    typename Allocator = std::allocator<ValueType>>
class multi_open_address_table_st {
public:
    //! Type of values.
    using value_type = ValueType;

    //! Type of keys.
    using key_type = KeyType;

    //! Type of the function to extract keys from values.
    using extract_key_type = ExtractKey;

    //! Type of the hash function.
    using hash_type = Hash;

    //! Type of the function to check whether keys are equal.
    using key_equal_type = KeyEqual;

    //! Type of allocators.
    using allocator_type = Allocator;

    //! Type of sizes.
    using size_type = std::size_t;

    //! Default number of internal tables.
    static constexpr size_type default_num_tables = 16U;

    /*!
     * \brief Constructor.
     */
    multi_open_address_table_st()
        : multi_open_address_table_st(default_num_tables) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] min_num_tables Minimum number of internal tables.
     * \param[in] min_internal_num_nodes Minimum number of nodes in internal
     * tables.
     * \param[in] extract_key Function to extract keys from values.
     * \param[in] hash Hash function.
     * \param[in] key_equal Function to check whether keys are equal.
     * \param[in] allocator Allocator.
     */
    explicit multi_open_address_table_st(size_type min_num_tables,
        size_type min_internal_num_nodes =
            internal_table_type::default_num_nodes,
        extract_key_type extract_key = extract_key_type(),
        hash_type hash = hash_type(),
        key_equal_type key_equal = key_equal_type(),
        allocator_type allocator = allocator_type())
        : internal_tables_(internal_table_allocator_type(allocator)),
          extract_key_(extract_key),
          hash_(hash),
          key_equal_(key_equal) {
        const size_type num_tables = utility::round_up_to_power_of_two(
            std::max<size_type>(min_num_tables, 2U));
        internal_table_index_mask_ = num_tables - 1U;
        internal_table_hash_shift_ = utility::count_right_zero_bits(num_tables);

        internal_tables_.reserve(num_tables);
        for (size_type i = 0; i < num_tables; ++i) {
            internal_tables_.emplace_back(min_internal_num_nodes,
                internal_extract_key_type(extract_key_), internal_hash_type(),
                internal_key_equal_type(key_equal_),
                internal_allocator_type(allocator));
        }
    }

private:
    /*!
     * \brief Type of values in internal tables.
     *
     * This type is a pair of the actual value and the hash number in the
     * internal tables.
     */
    using internal_value_type = std::pair<value_type, std::size_t>;

    //! Type of keys in internal tables.
    using internal_key_type = internal::hashed_key_view<key_type>;

    //! Type of the function to extract keys from values in internal tables.
    using internal_extract_key_type =
        internal::extract_hashed_key_view<value_type, key_type,
            extract_key_type>;

    //! Type of the hash function in internal tables.
    using internal_hash_type = internal::hashed_key_view_hash<key_type>;

    //! Type of the function to check whether keys are equal.
    using internal_key_equal_type =
        internal::hashed_key_view_equal<key_type, key_equal_type>;

    //! Type of allocators in internal tables.
    using internal_allocator_type = typename std::allocator_traits<
        allocator_type>::template rebind_alloc<internal_value_type>;

    //! Type of internal tables.
    using internal_table_type = open_address_table_st<internal_value_type,
        internal_key_type, internal_extract_key_type, internal_hash_type,
        internal_key_equal_type, internal_allocator_type>;

    //! Type of allocators of internal tables.
    using internal_table_allocator_type = typename std::allocator_traits<
        allocator_type>::template rebind_alloc<internal_table_type>;

    //! Internal tables.
    std::vector<internal_table_type, internal_table_allocator_type>
        internal_tables_;

    //! Function to extract keys from values.
    extract_key_type extract_key_;

    //! Hash function.
    hash_type hash_;

    //! Function to check whether keys are equal.
    key_equal_type key_equal_;

    //! Bit mask to get the index of the internal table.
    size_type internal_table_index_mask_{};

    //! Number of bits to shift to calculate hash numbers in internal tables.
    size_type internal_table_hash_shift_{};
};

}  // namespace hash_tables::tables
