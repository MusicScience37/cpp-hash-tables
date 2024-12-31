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
 * \brief Definition of open_address_set_st class.
 */
#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>  // IWYU pragma: keep

#include "hash_tables/extract_key_functions/identity.h"
#include "hash_tables/hashes/default_hash.h"
#include "hash_tables/tables/open_address_table_st.h"

namespace hash_tables::sets {

/*!
 * \brief Class to save values in a hash table using open addressing.
 *
 * \tparam KeyType Type of keys.
 * \tparam Hash Type of the hash function.
 * \tparam KeyEqual Type of the function to check whether keys are equal.
 * \tparam Allocator Type of allocators.
 */
template <typename KeyType, typename Hash = hashes::default_hash<KeyType>,
    typename KeyEqual = std::equal_to<KeyType>,
    typename Allocator = std::allocator<KeyType>>
class open_address_set_st {
public:
    //! Type of keys.
    using key_type = KeyType;

    //! Type of values.
    using value_type = key_type;

    //! Type of the hash function.
    using hash_type = Hash;

    //! Type of the function to check whether keys are equal.
    using key_equal_type = KeyEqual;

    //! Type of allocators.
    using allocator_type = Allocator;

    //! Type of sizes.
    using size_type = std::size_t;

    //! Type of the function to extract keys from values.
    using extract_key_type = extract_key_functions::identity<value_type>;

    //! Type of the internal hash table.
    using table_type = tables::open_address_table_st<value_type, key_type,
        extract_key_type, hash_type, key_equal_type, allocator_type>;

    /*!
     * \brief Constructor.
     */
    open_address_set_st() : table_() {}

    /*!
     * \brief Constructor.
     *
     * \param[in] min_num_nodes Minimum number of nodes.
     * \param[in] hash Hash function.
     * \param[in] key_equal Function to check whether keys are equal.
     * \param[in] allocator Allocator.
     */
    explicit open_address_set_st(size_type min_num_nodes,
        hash_type hash = hash_type(),
        key_equal_type key_equal = key_equal_type(),
        allocator_type allocator = allocator_type())
        : table_(
              min_num_nodes, extract_key_type(), hash, key_equal, allocator) {}

    /*!
     * \brief Copy constructor.
     */
    open_address_set_st(const open_address_set_st&) = default;

    /*!
     * \brief Move constructor.
     */
    open_address_set_st(open_address_set_st&&)
#ifndef HASH_TABLES_DOCUMENTATION
        noexcept(std::is_nothrow_move_constructible_v<table_type>)
#endif
        = default;

    /*!
     * \brief Copy assignment operator.
     *
     * \return This.
     */
    auto operator=(const open_address_set_st&) -> open_address_set_st&
#ifndef HASH_TABLES_DOCUMENTATION
        = default
#endif
        ;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    auto operator=(open_address_set_st&&)
#ifndef HASH_TABLES_DOCUMENTATION
        noexcept(std::is_nothrow_move_assignable_v<table_type>)
#endif
            -> open_address_set_st&
#ifndef HASH_TABLES_DOCUMENTATION
        = default
#endif
        ;

    /*!
     * \brief Destructor.
     */
    ~open_address_set_st() noexcept = default;

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
        return table_.insert(value);
    }

    /*!
     * \brief Insert a value.
     *
     * \param[in] value Value.
     * \retval true Value is inserted.
     * \retval false Value is not inserted due to a duplicated key.
     */
    auto insert(value_type&& value) -> bool {
        return table_.insert(std::move(value));
    }

    ///@}

    /*!
     * \name Read values.
     */
    ///@{

    /*!
     * \brief Check whether a key exists.
     *
     * \param[in] key Key.
     * \retval true Key exists.
     * \retval false Key doesn't exists.
     */
    [[nodiscard]] auto has(const key_type& key) const -> bool {
        return table_.has(key);
    }

    /*!
     * \brief Call a function with all values.
     *
     * \tparam Function Type of the function.
     * \param[in] function Function.
     */
    template <typename Function>
    void for_all(Function&& function) const {
        table_.for_all(function);
    }

    ///@}

    /*!
     * \name Delete values.
     */
    ///@{

    /*!
     * \brief Delete all values.
     */
    void clear() noexcept { table_.clear(); }

    /*!
     * \brief Delete a value.
     *
     * \param[in] key Key.
     * \retval true Deleted the value.
     * \retval false Failed to delete the value because the key not found.
     */
    auto erase(const key_type& key) -> bool { return table_.erase(key); }

    /*!
     * \brief Delete values which satisfy a condition.
     *
     * \tparam Function Type of the function.
     * \param[in] function Function to check the condition.
     * \return Number of removed values.
     */
    template <typename Function>
    auto erase_if(Function&& function) -> size_type {
        return table_.erase_if(function);
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
        return table_.check_all_satisfy(std::forward<Function>(function));
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
        return table_.check_any_satisfy(std::forward<Function>(function));
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
        return table_.check_none_satisfy(std::forward<Function>(function));
    }

    ///@}

    /*!
     * \name Operations of sets.
     */
    ///@{

    /*!
     * \brief Merge another set.
     *
     * \param[in] other Set to merge.
     */
    void merge(const open_address_set_st& other) {
        other.for_all([this](const value_type& value) { this->insert(value); });
    }

    /*!
     * \brief Merge another set.
     *
     * \param[in] other Set to merge.
     * \return This.
     */
    auto operator+=(const open_address_set_st& other) -> open_address_set_st& {
        merge(other);
        return *this;
    }

    /*!
     * \brief Merge another set.
     *
     * \param[in] right Right-hand-side object.
     * \return Merged set.
     */
    auto operator+(const open_address_set_st& right) const
        -> open_address_set_st {
        return open_address_set_st(*this) += right;
    }

    /*!
     * \brief Delete values in another set.
     *
     * \param[in] other Another set.
     */
    void erase(const open_address_set_st& other) {
        other.for_all([this](const value_type& value) { this->erase(value); });
    }

    /*!
     * \brief Delete values in another set.
     *
     * \param[in] other Another set.
     * \return This.
     */
    auto operator-=(const open_address_set_st& other) -> open_address_set_st& {
        erase(other);
        return *this;
    }

    /*!
     * \brief Delete values in another set.
     *
     * \param[in] right Right-hand-side object.
     * \return A set with elements same as this set except for elements in the
     * right-hand-side set.
     */
    auto operator-(const open_address_set_st& right) const
        -> open_address_set_st {
        return open_address_set_st(*this) -= right;
    }

    /*!
     * \brief Remove values not in the intersection with another set.
     *
     * \param[in] other Another set.
     */
    void keep_only_intersection_with(const open_address_set_st& other) {
        erase_if(
            [&other](const value_type& value) { return !other.has(value); });
    }

    /*!
     * \brief Determine whether this set and the given set have common elements.
     *
     * \param[in] other Another set.
     * \return Whether this set and the given set have common elements.
     */
    [[nodiscard]] auto has_intersection_with(
        const open_address_set_st& other) const {
        return check_any_satisfy(
            [&other](const value_type& value) { return other.has(value); });
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
        return table_.size();
    }

    /*!
     * \brief Check whether this object is empty.
     *
     * \retval true This object is empty.
     * \retval false This object is not empty.
     */
    [[nodiscard]] auto empty() const noexcept -> bool { return table_.empty(); }

    /*!
     * \brief Get the maximum number of values.
     *
     * \return Maximum number of values.
     */
    [[nodiscard]] auto max_size() const noexcept -> size_type {
        return table_.max_size();
    }

    /*!
     * \brief Reserve enough place for values.
     *
     * \param[in] size Number of values.
     */
    void reserve(size_type size) { table_.reserve(size); }

    ///@}

    /*!
     * \name Access to internal information.
     */
    ///@{

    /*!
     * \brief Get the hash function.
     *
     * \return Hash function.
     */
    [[nodiscard]] auto hash() const noexcept -> const hash_type& {
        return table_.hash();
    }

    /*!
     * \brief Get the function to check whether keys are equal.
     *
     * \return Function to check whether keys are equal.
     */
    [[nodiscard]] auto key_equal() const noexcept -> const key_equal_type& {
        return table_.key_equal();
    }

    /*!
     * \brief Get the allocator.
     *
     * \return Allocator.
     */
    [[nodiscard]] auto allocator() const -> allocator_type {
        return table_.allocator();
    }

    /*!
     * \brief Get the number of nodes.
     *
     * \return Number of nodes.
     */
    [[nodiscard]] auto num_nodes() const noexcept -> size_type {
        return table_.num_nodes();
    }

    /*!
     * \brief Change the number of nodes.
     *
     * \param[in] min_num_node Minimum number of nodes.
     */
    void rehash(size_type min_num_node) { table_.rehash(min_num_node); }

    /*!
     * \brief Get the load factor (number of values / number of nodes).
     *
     * \return Load factor.
     */
    auto load_factor() -> float { return table_.load_factor(); }

    /*!
     * \brief Get the maximum load factor (number of values / number of nodes).
     *
     * \return Maximum load factor.
     */
    auto max_load_factor() -> float { return table_.max_load_factor(); }

    /*!
     * \brief Set the maximum load factor (number of values / number of nodes).
     *
     * \param[in] value Maximum load factor.
     */
    void max_load_factor(float value) { table_.max_load_factor(value); }

    ///@}

private:
    //! Hash table.
    table_type table_;
};

}  // namespace hash_tables::sets
