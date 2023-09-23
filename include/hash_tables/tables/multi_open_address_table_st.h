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
 * \brief Definition of multi_open_address_table_st class.
 */
#pragma once

// IWYU pragma: no_include <assert.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "hash_tables/hashes/default_hash.h"
#include "hash_tables/tables/internal/hashed_key_view.h"
#include "hash_tables/tables/open_address_table_st.h"
#include "hash_tables/utility/count_right_zero_bits.h"
#include "hash_tables/utility/round_up_to_power_of_two.h"
#include "hash_tables/utility/value_storage.h"

namespace hash_tables::tables {

namespace internal {

/*!
 * \brief Default of minimum number of internal tables in
 * multi_open_address_table_st.
 */
constexpr std::size_t multi_open_address_table_st_default_min_num_tables = 16;

}  // namespace internal

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
 * \tparam MinNumTables Minimum number of internal tables.
 *
 * \thread_safety Safe if only functions without modifications of data are
 * called.
 */
template <typename ValueType, typename KeyType, typename ExtractKey,
    typename Hash = hashes::default_hash<KeyType>,
    typename KeyEqual = std::equal_to<KeyType>,
    typename Allocator = std::allocator<ValueType>,
    std::size_t MinNumTables =
        internal::multi_open_address_table_st_default_min_num_tables>
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

    //! Default number of nodes in internal tables.
    static constexpr size_type default_num_internal_nodes = 32U;

    /*!
     * \brief Constructor.
     *
     * \param[in] min_internal_num_nodes Minimum number of nodes in internal
     * tables.
     * \param[in] extract_key Function to extract keys from values.
     * \param[in] hash Hash function.
     * \param[in] key_equal Function to check whether keys are equal.
     * \param[in] allocator Allocator.
     */
    explicit multi_open_address_table_st(
        size_type min_internal_num_nodes = default_num_internal_nodes,
        extract_key_type extract_key = extract_key_type(),
        hash_type hash = hash_type(),
        key_equal_type key_equal = key_equal_type(),
        allocator_type allocator = allocator_type())
        : extract_key_(extract_key), hash_(hash), key_equal_(key_equal) {
        for (size_type i = 0; i < num_internal_tables; ++i) {
            internal_tables_[i].emplace(min_internal_num_nodes,
                internal_extract_key_type(extract_key_), internal_hash_type(),
                internal_key_equal_type(key_equal_),
                internal_allocator_type(allocator));
        }
    }

    /*!
     * \brief Copy constructor.
     *
     * \param other Another object to copy from.
     */
    multi_open_address_table_st(const multi_open_address_table_st& other)
        : extract_key_(other.extract_key_),
          hash_(other.hash_),
          key_equal_(other.key_equal_) {
        for (size_type i = 0; i < num_internal_tables; ++i) {
            internal_tables_[i].emplace(other.internal_tables_[i].get());
        }
    }

    /*!
     * \brief Move constructor.
     *
     * \param other Another object to move from.
     */
    multi_open_address_table_st(multi_open_address_table_st&& other)
#ifndef HASH_TABLES_DOCUMENTATION
        noexcept(std::is_nothrow_move_constructible_v<internal_table_type>&&  //
                std::is_nothrow_move_constructible_v<extract_key_type>&&      //
                    std::is_nothrow_move_constructible_v<hash_type>&&         //
                        std::is_nothrow_move_constructible_v<key_equal_type>)
#endif
        : extract_key_(std::move(other.extract_key_)),
          hash_(std::move(other.hash_)),
          key_equal_(std::move(other.key_equal_)) {
        for (size_type i = 0; i < num_internal_tables; ++i) {
            internal_tables_[i].emplace(
                std::move(other.internal_tables_[i].get()));
        }
    }

    /*!
     * \brief Copy assignment operator.
     *
     * \param other Another object to move from.
     * \return This.
     */
    auto operator=(const multi_open_address_table_st& other)
        -> multi_open_address_table_st& {
        if (this != &other) {
            *this = multi_open_address_table_st(other);
        }
        return *this;
    }

    /*!
     * \brief Move assignment operator.
     *
     * \param other Another object to move from.
     * \return This.
     */
    auto operator=(multi_open_address_table_st&& other)
#ifndef HASH_TABLES_DOCUMENTATION
        noexcept(std::is_nothrow_move_assignable_v<internal_table_type>&&  //
                std::is_nothrow_move_assignable_v<extract_key_type>&&      //
                    std::is_nothrow_move_assignable_v<hash_type>&&         //
                        std::is_nothrow_move_assignable_v<key_equal_type>)
#endif
            -> multi_open_address_table_st& {
        extract_key_ = std::move(other.extract_key_);
        hash_ = std::move(other.hash_);
        key_equal_ = std::move(other.key_equal_);
        for (size_type i = 0; i < num_internal_tables; ++i) {
            internal_tables_[i].get() =
                std::move(other.internal_tables_[i].get());
        }
        return *this;
    }

    /*!
     * \brief Destructor.
     */
    ~multi_open_address_table_st() noexcept {
        for (size_type i = 0; i < num_internal_tables; ++i) {
            internal_tables_[i].clear();
        }
    }

    /*!
     * \name Create or update values.
     */
    ///@{

    /*!
     * \brief Insert a value.
     *
     * \param[in] value Value.
     * \retval true Value is inserted.
     * \retval false Value is not inserted due to a duplicated key.
     */
    auto insert(const value_type& value) -> bool {
        return emplace(extract_key_(value), value);
    }

    /*!
     * \brief Insert a value.
     *
     * \param[in] value Value.
     * \retval true Value is inserted.
     * \retval false Value is not inserted due to a duplicated key.
     */
    auto insert(value_type&& value) -> bool {
        return emplace(extract_key_(value), std::move(value));
    }

    /*!
     * \brief Insert a value from the arguments of its constructor.
     *
     * \tparam Args Type of arguments of the constructor.
     * \param[in] key Key of the value. (Assumed to be equal to the key of the
     * value constructed from arguments.)
     * \param[in] args Arguments of the constructor.
     * \retval true Value is inserted.
     * \retval false Value is not inserted due to a duplicated key.
     */
    template <typename... Args>
    auto emplace(const key_type& key, Args&&... args) -> bool {
        const auto [internal_table_index, internal_key] =
            prepare_for_search(key);
        return internal_tables_[internal_table_index].get().emplace(
            internal_key, std::piecewise_construct,
            std::forward_as_tuple(std::forward<Args>(args)...),
            std::forward_as_tuple(internal_key.hash_number()));
    }

    /*!
     * \brief Insert a value from the arguments of its constructor if not exist,
     * or assign to an existing value.
     *
     * \tparam Args Type of arguments of the constructor.
     * \param[in] key Key of the value. (Assumed to be equal to the key of the
     * value constructed from arguments.)
     * \param[in] args Arguments of the constructor.
     * \retval true Value is inserted.
     * \retval false Value is assigned to an existing value.
     */
    template <typename... Args>
    auto emplace_or_assign(const key_type& key, Args&&... args) -> bool {
        const auto [internal_table_index, internal_key] =
            prepare_for_search(key);
        return internal_tables_[internal_table_index].get().emplace_or_assign(
            internal_key, std::piecewise_construct,
            std::forward_as_tuple(std::forward<Args>(args)...),
            std::forward_as_tuple(internal_key.hash_number()));
    }

    /*!
     * \brief Assign a value to an existing key from the arguments of its
     * constructor.
     *
     * \tparam Args Type of arguments of the constructor.
     * \param[in] key Key of the value. (Assumed to be equal to the key of the
     * value constructed from arguments.)
     * \param[in] args Arguments of the constructor.
     * \retval true Value is assigned.
     * \retval false Value is not assigned due to non-existing key.
     */
    template <typename... Args>
    auto assign(const key_type& key, Args&&... args) -> bool {
        const auto [internal_table_index, internal_key] =
            prepare_for_search(key);
        return internal_tables_[internal_table_index].get().assign(internal_key,
            std::piecewise_construct,
            std::forward_as_tuple(std::forward<Args>(args)...),
            std::forward_as_tuple(internal_key.hash_number()));
    }

    ///@}

    /*!
     * \name Read values.
     */
    ///@{

    /*!
     * \brief Get a value.
     *
     * \param[in] key Key.
     * \return Value.
     */
    [[nodiscard]] auto at(const key_type& key) -> value_type& {
        const auto [internal_table_index, internal_key] =
            prepare_for_search(key);
        return internal_tables_[internal_table_index]
            .get()
            .at(internal_key)
            .first;
    }

    /*!
     * \brief Get a value.
     *
     * \param[in] key Key.
     * \return Value.
     */
    [[nodiscard]] auto at(const key_type& key) const -> const value_type& {
        const auto [internal_table_index, internal_key] =
            prepare_for_search(key);
        return internal_tables_[internal_table_index]
            .get()
            .at(internal_key)
            .first;
    }

    /*!
     * \brief Get a value constructing it if not found.
     *
     * \tparam Args Type of arguments of the constructor.
     * \param[in] key Key.
     * \param[in] args Arguments of the constructor.
     * \return Value.
     */
    template <typename... Args>
    [[nodiscard]] auto get_or_create(const key_type& key, Args&&... args)
        -> value_type& {
        const auto [internal_table_index, internal_key] =
            prepare_for_search(key);
        return internal_tables_[internal_table_index]
            .get()
            .get_or_create(internal_key, std::piecewise_construct,
                std::forward_as_tuple(std::forward<Args>(args)...),
                std::forward_as_tuple(internal_key.hash_number()))
            .first;
    }

    /*!
     * \brief Get a value constructing using a factory function it if not found.
     *
     * \tparam Function Type of the factory function.
     * \param[in] key Key.
     * \param[in] function Factory function.
     * \return Value.
     */
    template <typename Function>
    [[nodiscard]] auto get_or_create_with_factory(
        const key_type& key, Function&& function) -> value_type& {
        const auto [internal_table_index, internal_key] =
            prepare_for_search(key);
        return internal_tables_[internal_table_index]
            .get()
            .get_or_create_with_factory(internal_key,
                [internal_hash_number = internal_key.hash_number(), &function] {
                    return std::make_pair(
                        std::invoke(std::forward<Function>(function)),
                        internal_hash_number);
                })
            .first;
    }

    /*!
     * \brief Get a value if found.
     *
     * \param[in] key Key.
     * \return Pointer to the value if found, otherwise nullptr.
     */
    [[nodiscard]] auto try_get(const key_type& key) -> value_type* {
        const auto [internal_table_index, internal_key] =
            prepare_for_search(key);
        internal_value_type* ptr =
            internal_tables_[internal_table_index].get().try_get(internal_key);
        if (ptr == nullptr) {
            return nullptr;
        }
        return &(ptr->first);
    }

    /*!
     * \brief Get a value if found.
     *
     * \param[in] key Key.
     * \return Pointer to the value if found, otherwise nullptr.
     */
    [[nodiscard]] auto try_get(const key_type& key) const -> const value_type* {
        const auto [internal_table_index, internal_key] =
            prepare_for_search(key);
        const internal_value_type* ptr =
            internal_tables_[internal_table_index].get().try_get(internal_key);
        if (ptr == nullptr) {
            return nullptr;
        }
        return &(ptr->first);
    }

    /*!
     * \brief Check whether a key exists.
     *
     * \param[in] key Key.
     * \retval true Key exists.
     * \retval false Key doesn't exists.
     */
    [[nodiscard]] auto has(const key_type& key) const -> bool {
        const auto [internal_table_index, internal_key] =
            prepare_for_search(key);
        return internal_tables_[internal_table_index].get().has(internal_key);
    }

    /*!
     * \brief Call a function with all values.
     *
     * \tparam Function Type of the function.
     * \param[in] function Function.
     */
    template <typename Function>
    void for_all(Function&& function) {
        for (auto& internal_table : internal_tables_) {
            internal_table.get().for_all(
                [&function](internal_value_type& value) {
                    std::invoke(function, value.first);
                });
        }
    }

    /*!
     * \brief Call a function with all values.
     *
     * \tparam Function Type of the function.
     * \param[in] function Function.
     */
    template <typename Function>
    void for_all(Function&& function) const {
        for (const auto& internal_table : internal_tables_) {
            internal_table.get().for_all(
                [&function](const internal_value_type& value) {
                    std::invoke(function, value.first);
                });
        }
    }

    ///@}

    /*!
     * \name Delete values.
     */
    ///@{

    /*!
     * \brief Delete all values.
     */
    void clear() noexcept {
        for (auto& internal_table : internal_tables_) {
            internal_table.get().clear();
        }
    }

    /*!
     * \brief Delete a value.
     *
     * \param[in] key Key.
     * \retval true Deleted the value.
     * \retval false Failed to delete the value because the key not found.
     */
    auto erase(const key_type& key) -> bool {
        const auto [internal_table_index, internal_key] =
            prepare_for_search(key);
        return internal_tables_[internal_table_index].get().erase(internal_key);
    }

    /*!
     * \brief Delete values which satisfy a condition.
     *
     * \tparam Function Type of the function.
     * \param[in] function Function to check the condition.
     * \return Number of removed values.
     */
    template <typename Function>
    auto erase_if(Function&& function) -> size_type {
        size_type res = 0U;
        for (auto& internal_table : internal_tables_) {
            res += internal_table.get().erase_if(
                [&function](const internal_value_type& value) {
                    return std::invoke(function, value.first);
                });
        }
        return res;
    }

    ///@}

    /*!
     * \name Check conditions for elements.
     */
    ///@{

    /*!
     * \brief Check whether all elements satisfy a condition.
     *
     * \tparam Function Type of the function.
     * \param[in] function Function to check each element.
     * \retval true All elements satisfied the condition.
     * \retval false Some elements didn't satisfy the condition.
     */
    template <typename Function>
    auto check_all_satisfy(Function&& function) const -> bool {
        for (auto& internal_table : internal_tables_) {
            if (!internal_table.get().check_all_satisfy(
                    [&function](const internal_value_type& value) {
                        return std::invoke(function, value.first);
                    })) {
                return false;
            }
        }
        return true;
    }

    /*!
     * \brief Check whether at least one element satisfies a condition.
     *
     * \tparam Function Type of the function.
     * \param[in] function Function to check each element.
     * \retval true At least one element satisfied the condition.
     * \retval false No element satisfied the condition.
     */
    template <typename Function>
    auto check_any_satisfy(Function&& function) const -> bool {
        for (auto& internal_table : internal_tables_) {
            if (internal_table.get().check_any_satisfy(
                    [&function](const internal_value_type& value) {
                        return std::invoke(function, value.first);
                    })) {
                return true;
            }
        }
        return false;
    }

    /*!
     * \brief Check whether no element satisfies a condition.
     *
     * \tparam Function Type of the function.
     * \param[in] function Function to check each element.
     * \retval true No element satisfied the condition.
     * \retval false At least one element satisfied the condition.
     */
    template <typename Function>
    auto check_none_satisfy(Function&& function) const -> bool {
        for (auto& internal_table : internal_tables_) {
            if (!internal_table.get().check_none_satisfy(
                    [&function](const internal_value_type& value) {
                        return std::invoke(function, value.first);
                    })) {
                return false;
            }
        }
        return true;
    }

    ///@}

    /*!
     * \name Handle size.
     */
    ///@{

    /*!
     * \brief Get the number of values.
     *
     * \return Number of values.
     */
    [[nodiscard]] auto size() const noexcept -> size_type {
        size_type res = 0U;
        for (const auto& internal_table : internal_tables_) {
            res += internal_table.get().size();
        }
        return res;
    }

    /*!
     * \brief Check whether this object is empty.
     *
     * \retval true This object is empty.
     * \retval false This object is not empty.
     */
    [[nodiscard]] auto empty() const noexcept -> bool { return size() == 0U; }

    /*!
     * \brief Get the maximum number of values.
     *
     * \return Maximum number of values.
     */
    [[nodiscard]] auto max_size() const noexcept -> size_type {
        return internal_tables_[0].get().max_size();
    }

    /*!
     * \brief Reserve enough place for values.
     *
     * \param[in] size Number of values.
     */
    void reserve(size_type size) {
        for (auto& internal_table : internal_tables_) {
            internal_table.get().reserve(size);
        }
    }

    /*!
     * \brief Reserve approximately enough place for values.
     *
     * \note Use of this function may be faster than reserve function for real
     * application.
     *
     * \param[in] size Number of values.
     */
    void reserve_approx(size_type size) {
        size_type approx_size_for_internal_tables =
            size / internal_tables_.size();
        approx_size_for_internal_tables += approx_size_for_internal_tables / 2U;
        for (auto& internal_table : internal_tables_) {
            internal_table.get().reserve(approx_size_for_internal_tables);
        }
    }

    ///@}

    /*!
     * \name Access to internal information.
     */
    ///@{

    /*!
     * \brief Get the function to extract keys from values.
     *
     * \return Function to extract keys from values.
     */
    [[nodiscard]] auto extract_key() const noexcept -> const extract_key_type& {
        return extract_key_;
    }

    /*!
     * \brief Get the hash function.
     *
     * \return Hash function.
     */
    [[nodiscard]] auto hash() const noexcept -> const hash_type& {
        return hash_;
    }

    /*!
     * \brief Get the function to check whether keys are equal.
     *
     * \return Function to check whether keys are equal.
     */
    [[nodiscard]] auto key_equal() const noexcept -> const key_equal_type& {
        return key_equal_;
    }

    /*!
     * \brief Get the allocator.
     *
     * \return Allocator.
     */
    [[nodiscard]] auto allocator() const -> allocator_type {
        return allocator_type(internal_tables_.get_allocator());
    }

    /*!
     * \brief Get the total number of nodes in internal tables.
     *
     * \return Total number of nodes.
     */
    [[nodiscard]] auto num_nodes() const noexcept -> size_type {
        size_type res = 0U;
        for (const auto& internal_table : internal_tables_) {
            res += internal_table.get().num_nodes();
        }
        return res;
    }

    /*!
     * \brief Set the maximum load factor (number of values / number of nodes).
     *
     * \param[in] value Maximum load factor.
     */
    void max_load_factor(float value) {
        for (auto& internal_table : internal_tables_) {
            internal_table.get().max_load_factor(value);
        }
    }

    ///@}

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

    /*!
     * \brief Prepare for search of positions to create, get, or remove values
     * of a key.
     *
     * \param[in] key Key.
     * \return Index of internal table and key for the internal table.
     */
    [[nodiscard]] auto prepare_for_search(const key_type& key) const
        -> std::pair<size_type, internal::hashed_key_view<key_type>> {
        const size_type hash_number = hash_(key);
        const size_type internal_table_index =
            hash_number & internal_table_index_mask;
        const size_type internal_table_hash_number =
            hash_number >> internal_table_hash_shift;
        assert(hash_number ==
            internal_table_hash_number * internal_tables_.size() +
                internal_table_index);
        return {internal_table_index,
            internal::hashed_key_view<key_type>(
                key, internal_table_hash_number)};
    }

    //! Number of internal tables.
    static constexpr size_type num_internal_tables =
        utility::round_up_to_power_of_two(
            std::max<size_type>(MinNumTables, 2U));

    //! Bit mask to get the index of the internal table.
    static constexpr size_type internal_table_index_mask =
        num_internal_tables - 1U;

    //! Number of bits to shift to calculate hash numbers in internal tables.
    static constexpr size_type internal_table_hash_shift =
        utility::count_right_zero_bits(num_internal_tables);

    //! Internal tables.
    std::array<utility::value_storage<internal_table_type>, num_internal_tables>
        internal_tables_{};

    //! Function to extract keys from values.
    extract_key_type extract_key_;

    //! Hash function.
    hash_type hash_;

    //! Function to check whether keys are equal.
    key_equal_type key_equal_;
};

}  // namespace hash_tables::tables
