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
 * \brief Definition of multi_open_address_map_mt class.
 */
#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <tuple>
#include <utility>

#include "hash_tables/extract_key_functions/extract_first_from_pair.h"
#include "hash_tables/hashes/default_hash.h"
#include "hash_tables/maps/internal/mapped_value_getter.h"
#include "hash_tables/tables/multi_open_address_table_mt.h"

namespace hash_tables::maps {

/*!
 * \brief Class to save key-value relations in a concurrent hash table made of
 * multiple hash tables using open addressing.
 *
 * \tparam KeyType Type of keys.
 * \tparam MappedType Type of mapped values.
 * \tparam Hash Type of the hash function.
 * \tparam KeyEqual Type of the function to check whether keys are equal.
 * \tparam Allocator Type of allocators.
 *
 * \thread_safety Safe even for the same object.
 */
template <typename KeyType, typename MappedType,
    typename Hash = hashes::default_hash<KeyType>,
    typename KeyEqual = std::equal_to<KeyType>,
    typename Allocator = std::allocator<std::pair<KeyType, MappedType>>>
class multi_open_address_map_mt {
public:
    //! Type of keys.
    using key_type = KeyType;

    //! Type of mapped values.
    using mapped_type = MappedType;

    //! Type of values.
    using value_type = std::pair<key_type, mapped_type>;

    //! Type of the hash function.
    using hash_type = Hash;

    //! Type of the function to check whether keys are equal.
    using key_equal_type = KeyEqual;

    //! Type of allocators.
    using allocator_type = Allocator;

    //! Type of sizes.
    using size_type = std::size_t;

    //! Type of the function to extract keys from values.
    using extract_key_type =
        extract_key_functions::extract_first_from_pair<value_type>;

    //! Type of the internal hash table.
    using table_type = tables::multi_open_address_table_mt<value_type, key_type,
        extract_key_type, hash_type, key_equal_type, allocator_type>;

    /*!
     * \brief Constructor.
     */
    multi_open_address_map_mt() : table_() {}

    /*!
     * \brief Constructor.
     *
     * \param[in] min_internal_num_nodes Minimum number of nodes in internal
     * tables.
     * \param[in] hash Hash function.
     * \param[in] key_equal Function to check whether keys are equal.
     * \param[in] allocator Allocator.
     */
    explicit multi_open_address_map_mt(size_type min_internal_num_nodes,
        hash_type hash = hash_type(),
        key_equal_type key_equal = key_equal_type(),
        const allocator_type& allocator = allocator_type())
        : table_(min_internal_num_nodes, extract_key_type(), hash, key_equal,
              allocator) {}

    multi_open_address_map_mt(const multi_open_address_map_mt&) = delete;
    multi_open_address_map_mt(multi_open_address_map_mt&&) = delete;
    auto operator=(const multi_open_address_map_mt&) = delete;
    auto operator=(multi_open_address_map_mt&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~multi_open_address_map_mt() noexcept = default;

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

    /*!
     * \brief Insert a value from the arguments of its constructor.
     *
     * \tparam Args Type of arguments of the constructor of the mapped value.
     * \param[in] key Key of the value.
     * \param[in] args Arguments of the constructor of tha mapped value.
     * \retval true Value is inserted.
     * \retval false Value is not inserted due to a duplicated key.
     */
    template <typename... Args>
    auto emplace(const key_type& key, Args&&... args) -> bool {
        return table_.emplace(key, std::piecewise_construct,
            std::forward_as_tuple(key),
            std::forward_as_tuple(std::forward<Args>(args)...));
    }

    /*!
     * \brief Insert a value from the arguments of its constructor.
     *
     * \tparam Args Type of arguments of the constructor of the mapped value.
     * \param[in] key Key of the value.
     * \param[in] args Arguments of the constructor of tha mapped value.
     * \retval true Value is inserted.
     * \retval false Value is not inserted due to a duplicated key.
     */
    template <typename... Args>
    auto emplace(key_type&& key, Args&&... args) -> bool {
        // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved): false positive
        return table_.emplace(key, std::piecewise_construct,
            std::forward_as_tuple(std::move(key)),
            std::forward_as_tuple(std::forward<Args>(args)...));
    }

    /*!
     * \brief Insert a value from the arguments of its constructor if not exist,
     * or assign to an existing value.
     *
     * \tparam Args Type of arguments of the constructor of the mapped value.
     * \param[in] key Key of the value.
     * \param[in] args Arguments of the constructor of tha mapped value.
     * \retval true Value is inserted.
     * \retval false Value is assigned to an existing key.
     */
    template <typename... Args>
    auto emplace_or_assign(const key_type& key, Args&&... args) -> bool {
        // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved): false positive
        return table_.emplace_or_assign(key, std::piecewise_construct,
            std::forward_as_tuple(key),
            std::forward_as_tuple(std::forward<Args>(args)...));
    }

    /*!
     * \brief Insert a value from the arguments of its constructor if not exist,
     * or assign to an existing value.
     *
     * \tparam Args Type of arguments of the constructor of the mapped value.
     * \param[in] key Key of the value.
     * \param[in] args Arguments of the constructor of tha mapped value.
     * \retval true Value is inserted.
     * \retval false Value is assigned to an existing key.
     */
    template <typename... Args>
    auto emplace_or_assign(key_type&& key, Args&&... args) -> bool {
        // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved): false positive
        return table_.emplace_or_assign(key, std::piecewise_construct,
            std::forward_as_tuple(std::move(key)),
            std::forward_as_tuple(std::forward<Args>(args)...));
    }

    /*!
     * \brief Insert a value from the arguments of its constructor if not exist,
     * or assign to an existing value.
     *
     * \tparam Args Type of arguments of the constructor of the mapped value.
     * \param[in] key Key of the value.
     * \param[in] args Arguments of the constructor of tha mapped value.
     * \retval true Value is inserted.
     * \retval false Value is assigned to an existing key.
     */
    template <typename... Args>
    auto assign(const key_type& key, Args&&... args) -> bool {
        // NOLINTNEXTLINE(bugprone-use-after-move,hicpp-invalid-access-moved): false positive
        return table_.assign(key, std::piecewise_construct,
            std::forward_as_tuple(std::move(key)),
            std::forward_as_tuple(std::forward<Args>(args)...));
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
     * \return Mapped value.
     */
    [[nodiscard]] auto at(const key_type& key) const -> mapped_type {
        internal::mapped_value_getter<mapped_type> value;
        table_.get_to(value, key);
        return value.release();
    }

    /*!
     * \brief Get a value constructing it if not found.
     *
     * \tparam Args Type of arguments of the constructor of the mapped value.
     * \param[in] key Key of the value.
     * \param[in] args Arguments of the constructor of tha mapped value.
     * \return Mapped value.
     */
    template <typename... Args>
    [[nodiscard]] auto get_or_create(const key_type& key, Args&&... args)
        -> mapped_type {
        internal::mapped_value_getter<mapped_type> value;
        table_.get_or_create_to(value, key, std::piecewise_construct,
            std::forward_as_tuple(key),
            std::forward_as_tuple(std::forward<Args>(args)...));
        return value.release();
    }

    /*!
     * \brief Get a value constructing using a factory function if not found.
     *
     * \tparam Function Type of the factory function.
     * \param[in] key Key.
     * \param[in] function Factory function.
     * \return Mapped value.
     */
    template <typename Function>
    [[nodiscard]] auto get_or_create_with_factory(
        const key_type& key, Function&& function) -> mapped_type {
        internal::mapped_value_getter<mapped_type> value;
        table_.get_or_create_with_factory_to(value, key, [&key, &function] {
            return value_type(key, std::invoke(function));
        });
        return value.release();
    }

    /*!
     * \brief Get a value.
     *
     * \param[in] key Key.
     * \return Mapped value.
     */
    [[nodiscard]] auto operator[](const key_type& key) const -> mapped_type {
        return at(key);
    }

    /*!
     * \brief Get a value if found.
     *
     * \param[in] key Key.
     * \return Value if found, otherwise null.
     */
    [[nodiscard]] auto try_get(const key_type& key) const
        -> std::optional<mapped_type> {
        internal::mapped_value_getter<mapped_type> value;
        if (table_.try_get_to(value, key)) {
            return value.release();
        }
        return std::nullopt;
    }

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
    void for_all(Function&& function) {
        table_.for_all([&function](value_type& value) {
            std::invoke(function, static_cast<const key_type&>(value.first),
                static_cast<mapped_type&>(value.second));
        });
    }

    /*!
     * \brief Call a function with all values.
     *
     * \tparam Function Type of the function.
     * \param[in] function Function.
     */
    template <typename Function>
    void for_all(Function&& function) const {
        table_.for_all([&function](const value_type& value) {
            std::invoke(function, static_cast<const key_type&>(value.first),
                static_cast<const mapped_type&>(value.second));
        });
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
        return table_.erase_if([&function](const value_type& value) {
            return std::invoke(function,
                static_cast<const key_type&>(value.first),
                static_cast<const mapped_type&>(value.second));
        });
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
        return table_.check_all_satisfy([&function](const value_type& value) {
            return std::invoke(function,
                static_cast<const key_type&>(value.first),
                static_cast<const mapped_type&>(value.second));
        });
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
        return table_.check_any_satisfy([&function](const value_type& value) {
            return std::invoke(function,
                static_cast<const key_type&>(value.first),
                static_cast<const mapped_type&>(value.second));
        });
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
        return table_.check_none_satisfy([&function](const value_type& value) {
            return std::invoke(function,
                static_cast<const key_type&>(value.first),
                static_cast<const mapped_type&>(value.second));
        });
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

    /*!
     * \brief Reserve approximately enough place for values.
     *
     * \note Use of this function may be faster than reserve function for real
     * application.
     *
     * \param[in] size Number of values.
     */
    void reserve_approx(size_type size) { table_.reserve_approx(size); }

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
     * \brief Get the total number of nodes in internal tables.
     *
     * \return Total number of nodes.
     */
    [[nodiscard]] auto num_nodes() const noexcept -> size_type {
        return table_.num_nodes();
    }

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

}  // namespace hash_tables::maps
