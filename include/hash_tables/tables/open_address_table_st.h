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
 * \brief Definition of open_address_table_st class.
 */
#pragma once

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <type_traits>  // IWYU pragma: keep
#include <utility>
#include <vector>

#include "hash_tables/exceptions.h"
#include "hash_tables/hashes/default_hash.h"
#include "hash_tables/utility/move_if_nothrow_move_constructible.h"
#include "hash_tables/utility/round_up_to_power_of_two.h"
#include "hash_tables/utility/value_storage.h"

namespace hash_tables::tables {

namespace internal {

/*!
 * \brief Class of nodes in open_address_table_st class.
 *
 * \tparam ValueType Type of values.
 */
template <typename ValueType>
class open_address_table_st_node {
public:
    //! Type of values.
    using value_type = ValueType;

    //! Enumeration of states of nodes.
    enum class node_state : std::uint8_t {
        //! Initial state.
        init,

        //! Has a value.
        filled,

        //! Once filled with a value, but empty now.
        erased
    };

    /*!
     * \brief Constructor.
     */
    open_address_table_st_node() noexcept = default;

    /*!
     * \brief Copy constructor.
     *
     * \param[in] obj Object to copy from.
     */
    open_address_table_st_node(const open_address_table_st_node& obj) {
        if (obj.state_ == node_state::filled) {
            storage_.emplace(obj.value());
        }
        state_ = obj.state_;
    }

    open_address_table_st_node(open_address_table_st_node&&) = delete;

    /*!
     * \brief Copy assignment operator.
     *
     * \param[in] obj Object to copy from.
     * \return This.
     */
    auto operator=(
        const open_address_table_st_node& obj) -> open_address_table_st_node& {
        if (this == &obj) {
            return *this;
        }

        clear();
        if (obj.state_ == node_state::filled) {
            storage_.emplace(obj.value());
        }
        state_ = obj.state_;
        return *this;
    }

    auto operator=(open_address_table_st_node&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~open_address_table_st_node() noexcept { clear(); }

    /*!
     * \brief Construct a value.
     *
     * \tparam Args Type of arguments.
     * \param[in] args Arguments of the constructor.
     */
    template <typename... Args>
    void emplace(Args&&... args) {
        assert(state_ != node_state::filled);
        storage_.emplace(std::forward<Args>(args)...);
        state_ = node_state::filled;
    }

    /*!
     * \brief Assign a value.
     *
     * \tparam Args Type of arguments.
     * \param[in] args Arguments of the constructor.
     */
    template <typename... Args>
    void assign(Args&&... args) {
        assert(state_ == node_state::filled);
        value() = value_type(std::forward<Args>(args)...);
    }

    /*!
     * \brief Clear the value.
     */
    void clear() noexcept {
        if (state_ == node_state::filled) {
            storage_.clear();
            state_ = node_state::erased;
        }
    }

    /*!
     * \brief Get the state.
     *
     * \return State.
     */
    [[nodiscard]] auto state() const noexcept -> node_state { return state_; }

    /*!
     * \brief Get the value.
     *
     * \return Value.
     */
    [[nodiscard]] auto value() noexcept -> value_type& {
        assert(state_ == node_state::filled);
        return storage_.get();
    }

    /*!
     * \brief Get the value.
     *
     * \return Value.
     */
    [[nodiscard]] auto value() const noexcept -> const value_type& {
        assert(state_ == node_state::filled);
        return storage_.get();
    }

private:
    //! Storage for a value.
    utility::value_storage<value_type> storage_{};

    //! State.
    node_state state_{node_state::init};
};

}  // namespace internal

/*!
 * \brief Class of hash tables using open addressing.
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
class open_address_table_st {
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

    //! Default number of nodes.
    static constexpr size_type default_num_nodes = 32;

    /*!
     * \brief Constructor.
     */
    open_address_table_st() : open_address_table_st(default_num_nodes) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] min_num_nodes Minimum number of nodes.
     * \param[in] extract_key Function to extract keys from values.
     * \param[in] hash Hash function.
     * \param[in] key_equal Function to check whether keys are equal.
     * \param[in] allocator Allocator.
     */
    explicit open_address_table_st(size_type min_num_nodes,
        extract_key_type extract_key = extract_key_type(),
        hash_type hash = hash_type(),
        key_equal_type key_equal = key_equal_type(),
        allocator_type allocator = allocator_type())
        : nodes_(determine_num_node_from_min_num_node(min_num_nodes),
              node_allocator_type(std::move(allocator))),
          extract_key_(std::move(extract_key)),
          hash_(std::move(hash)),
          key_equal_(std::move(key_equal)),
          desired_node_ind_mask_(nodes_.size() - 1U) {}

    /*!
     * \brief Copy constructor.
     */
    open_address_table_st(const open_address_table_st&) = default;

    /*!
     * \brief Move constructor.
     */
    open_address_table_st(open_address_table_st&&)
#ifndef HASH_TABLES_DOCUMENTATION
        noexcept(std::is_nothrow_move_constructible_v<extract_key_type> &&  //
            std::is_nothrow_move_constructible_v<hash_type> &&              //
            std::is_nothrow_move_constructible_v<key_equal_type> &&         //
            std::is_nothrow_move_constructible_v<
                std::vector<internal::open_address_table_st_node<value_type>,
                    typename std::allocator_traits<allocator_type>::
                        template rebind_alloc<
                            internal::open_address_table_st_node<value_type>>>>)
#endif
        = default;

    /*!
     * \brief Copy assignment operator.
     *
     * \return This.
     */
    auto operator=(const open_address_table_st&) -> open_address_table_st&
#ifndef HASH_TABLES_DOCUMENTATION
        = default
#endif
        ;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    auto operator=(open_address_table_st&&)
#ifndef HASH_TABLES_DOCUMENTATION
        noexcept(std::is_nothrow_move_assignable_v<extract_key_type> &&  //
            std::is_nothrow_move_assignable_v<hash_type> &&              //
            std::is_nothrow_move_assignable_v<key_equal_type> &&         //
            std::is_nothrow_move_assignable_v<
                std::vector<internal::open_address_table_st_node<value_type>,
                    typename std::allocator_traits<allocator_type>::
                        template rebind_alloc<
                            internal::open_address_table_st_node<value_type>>>>)
#endif
            -> open_address_table_st&
#ifndef HASH_TABLES_DOCUMENTATION
        = default
#endif
        ;

    /*!
     * \brief Destructor.
     */
    ~open_address_table_st() noexcept = default;

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
        reserve(size_ + 1U);
        return insert_without_rehash(value);
    }

    /*!
     * \brief Insert a value.
     *
     * \param[in] value Value.
     * \retval true Value is inserted.
     * \retval false Value is not inserted due to a duplicated key.
     */
    auto insert(value_type&& value) -> bool {
        reserve(size_ + 1U);
        return insert_without_rehash(std::move(value));
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
        reserve(size_ + 1U);
        return emplace_without_rehash(key, std::forward<Args>(args)...);
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
        reserve(size_ + 1U);
        const auto [node_ptr, dist] = prepare_place_for(key);
        if (node_ptr->state() == node_type::node_state::filled) {
            node_ptr->assign(std::forward<Args>(args)...);
            return false;
        }
        node_ptr->emplace(std::forward<Args>(args)...);
        update_max_dist_if_needed(dist);
        ++size_;
        return true;
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
        const auto [node_ptr, dist] = prepare_place_for(key);
        if (node_ptr->state() == node_type::node_state::filled) {
            node_ptr->assign(std::forward<Args>(args)...);
            return true;
        }
        return false;
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
        return nodes_[require_node_ind_for(key)].value();
    }

    /*!
     * \brief Get a value.
     *
     * \param[in] key Key.
     * \return Value.
     */
    [[nodiscard]] auto at(const key_type& key) const -> const value_type& {
        return nodes_[require_node_ind_for(key)].value();
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
    [[nodiscard]] auto get_or_create(
        const key_type& key, Args&&... args) -> value_type& {
        reserve(size_ + 1U);
        const auto [node_ptr, dist] = prepare_place_for(key);
        if (node_ptr->state() != node_type::node_state::filled) {
            node_ptr->emplace(std::forward<Args>(args)...);
            update_max_dist_if_needed(dist);
            ++size_;
        }
        return node_ptr->value();
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
        reserve(size_ + 1U);
        const auto [node_ptr, dist] = prepare_place_for(key);
        if (node_ptr->state() != node_type::node_state::filled) {
            node_ptr->emplace(std::invoke(std::forward<Function>(function)));
            update_max_dist_if_needed(dist);
            ++size_;
        }
        return node_ptr->value();
    }

    /*!
     * \brief Get a value if found.
     *
     * \param[in] key Key.
     * \return Pointer to the value if found, otherwise nullptr.
     */
    [[nodiscard]] auto try_get(const key_type& key) -> value_type* {
        const auto node_ind = find_node_ind_for(key);
        if (!node_ind) {
            return nullptr;
        }
        return &nodes_[*node_ind].value();
    }

    /*!
     * \brief Get a value if found.
     *
     * \param[in] key Key.
     * \return Pointer to the value if found, otherwise nullptr.
     */
    [[nodiscard]] auto try_get(const key_type& key) const -> const value_type* {
        const auto node_ind = find_node_ind_for(key);
        if (!node_ind) {
            return nullptr;
        }
        return &nodes_[*node_ind].value();
    }

    /*!
     * \brief Check whether a key exists.
     *
     * \param[in] key Key.
     * \retval true Key exists.
     * \retval false Key doesn't exists.
     */
    [[nodiscard]] auto has(const key_type& key) const -> bool {
        return static_cast<bool>(find_node_ind_for(key));
    }

    /*!
     * \brief Call a function with all values.
     *
     * \tparam Function Type of the function.
     * \param[in] function Function.
     */
    template <typename Function>
    void for_all(Function&& function) {
        for (auto& node : nodes_) {
            if (node.state() == node_type::node_state::filled) {
                std::invoke(function, static_cast<value_type&>(node.value()));
            }
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
        for (const auto& node : nodes_) {
            if (node.state() == node_type::node_state::filled) {
                std::invoke(
                    function, static_cast<const value_type&>(node.value()));
            }
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
        for (auto& node : nodes_) {
            node.clear();
        }
        size_ = 0;
    }

    /*!
     * \brief Delete a value.
     *
     * \param[in] key Key.
     * \retval true Deleted the value.
     * \retval false Failed to delete the value because the key not found.
     */
    auto erase(const key_type& key) -> bool {
        const auto node_ind = find_node_ind_for(key);
        if (!node_ind) {
            return false;
        }
        nodes_[*node_ind].clear();
        --size_;
        return true;
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
        size_type removed = 0;
        for (auto& node : nodes_) {
            if (node.state() == node_type::node_state::filled) {
                if (std::invoke(function,
                        static_cast<const value_type&>(node.value()))) {
                    node.clear();
                    ++removed;
                    --size_;
                }
            }
        }
        return removed;
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
        for (auto& node : nodes_) {
            if (node.state() == node_type::node_state::filled) {
                if (!std::invoke(function,
                        static_cast<const value_type&>(node.value()))) {
                    return false;
                }
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
        for (auto& node : nodes_) {
            if (node.state() == node_type::node_state::filled) {
                if (std::invoke(function,
                        static_cast<const value_type&>(node.value()))) {
                    return true;
                }
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
        for (auto& node : nodes_) {
            if (node.state() == node_type::node_state::filled) {
                if (std::invoke(function,
                        static_cast<const value_type&>(node.value()))) {
                    return false;
                }
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
    [[nodiscard]] auto size() const noexcept -> size_type { return size_; }

    /*!
     * \brief Check whether this object is empty.
     *
     * \retval true This object is empty.
     * \retval false This object is not empty.
     */
    [[nodiscard]] auto empty() const noexcept -> bool { return size_ == 0; }

    /*!
     * \brief Get the maximum number of values.
     *
     * \return Maximum number of values.
     */
    [[nodiscard]] auto max_size() const noexcept -> size_type {
        return nodes_.max_size();
    }

    /*!
     * \brief Reserve enough place for values.
     *
     * \param[in] size Number of values.
     */
    void reserve(size_type size) {
        rehash(static_cast<size_type>(
            std::ceil(static_cast<float>(size) / max_load_factor_)));
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
        return allocator_type(nodes_.get_allocator());
    }

    /*!
     * \brief Get the number of nodes.
     *
     * \return Number of nodes.
     */
    [[nodiscard]] auto num_nodes() const noexcept -> size_type {
        return nodes_.size();
    }

    /*!
     * \brief Change the number of nodes.
     *
     * \param[in] min_num_node Minimum number of nodes.
     */
    void rehash(size_type min_num_node) {
        if (min_num_node < nodes_.size()) {
            return;
        }

        open_address_table_st new_table{
            min_num_node, extract_key_, hash_, key_equal_, allocator()};
        for (const auto& node : nodes_) {
            if (node.state() == node_type::node_state::filled) {
                new_table.insert_without_rehash(
                    utility::move_if_nothrow_move_constructible(node.value()));
            }
        }
        std::swap(nodes_, new_table.nodes_);
        std::swap(desired_node_ind_mask_, new_table.desired_node_ind_mask_);
    }

    /*!
     * \brief Get the load factor (number of values / number of nodes).
     *
     * \return Load factor.
     */
    auto load_factor() -> float {
        return static_cast<float>(size_) / static_cast<float>(nodes_.size());
    }

    /*!
     * \brief Get the maximum load factor (number of values / number of nodes).
     *
     * \return Maximum load factor.
     */
    auto max_load_factor() -> float { return max_load_factor_; }

    /*!
     * \brief Set the maximum load factor (number of values / number of nodes).
     *
     * \param[in] value Maximum load factor.
     */
    void max_load_factor(float value) {
        if (value <= 0.0F || 1.0F <= value) {
            throw std::invalid_argument("Invalid maximum load factor.");
        }
        max_load_factor_ = value;
    }

    ///@}

private:
    //! Type of nodes.
    using node_type = internal::open_address_table_st_node<value_type>;

    //! Type of allocators for nodes.
    using node_allocator_type = typename std::allocator_traits<
        allocator_type>::template rebind_alloc<node_type>;

    /*!
     * \brief Determine the number of nodes from the minimum number of nodes.
     *
     * \param[in] min_num_node Minimum number of nodes.
     * \return Number of nodes to use.
     */
    [[nodiscard]] static auto determine_num_node_from_min_num_node(
        size_type min_num_node) -> size_type {
        size_type required_num_nodes =
            utility::round_up_to_power_of_two(min_num_node);
        if (required_num_nodes < default_num_nodes) {
            required_num_nodes = default_num_nodes;
        }
        return required_num_nodes;
    }

    /*!
     * \brief Calculate the node index determined by hash number.
     *
     * \param[in] key Key.
     * \return Node index.
     */
    [[nodiscard]] auto desired_node_ind(
        const key_type& key) const -> size_type {
        const size_type hash_number = hash_(key);
        return hash_number & desired_node_ind_mask_;
    }

    /*!
     * \name Internal functions to create or update values.
     */
    ///@{

    /*!
     * \brief Insert a value without changing the number of nodes.
     *
     * \param[in] value Value.
     * \retval true Value is inserted.
     * \retval false Value is not inserted due to a duplicated key.
     */
    auto insert_without_rehash(const value_type& value) -> bool {
        return emplace_without_rehash(extract_key_(value), value);
    }

    /*!
     * \brief Insert a value without changing the number of nodes.
     *
     * \param[in] value Value.
     * \retval true Value is inserted.
     * \retval false Value is not inserted due to a duplicated key.
     */
    auto insert_without_rehash(value_type&& value) -> bool {
        return emplace_without_rehash(extract_key_(value), std::move(value));
    }

    /*!
     * \brief Insert a value from the arguments of its constructor without
     * changing the number of nodes.
     *
     * \tparam Args Type of arguments of the constructor.
     * \param[in] key Key of the value. (Assumed to be equal to the key of the
     * value constructed from arguments.)
     * \param[in] args Arguments of the constructor.
     * \retval true Value is inserted.
     * \retval false Value is not inserted due to a duplicated key.
     */
    template <typename... Args>
    auto emplace_without_rehash(const key_type& key, Args&&... args) -> bool {
        const auto [node_ptr, dist] = prepare_place_for(key);
        if (node_ptr->state() == node_type::node_state::filled) {
            return false;
        }
        node_ptr->emplace(std::forward<Args>(args)...);
        update_max_dist_if_needed(dist);
        ++size_;
        return true;
    }

    /*!
     * \brief Find the place to insert or assign a value.
     *
     * \param[in] key Key of the value.
     * \return Pointer to the node and distance from the place determined by
     * hash number.
     */
    auto prepare_place_for(
        const key_type& key) -> std::tuple<node_type*, size_type> {
        const size_type start_node_ind = desired_node_ind(key);
        auto iter = nodes_.begin() + start_node_ind;
        size_type dist = 0;
        std::optional<std::tuple<node_type*, size_type>> empty_place;
        while (true) {
            if (iter->state() == node_type::node_state::filled) {
                if (key_equal_(extract_key_(iter->value()), key)) {
                    return {&(*iter), dist};
                }
            } else {
                if (!empty_place) {
                    empty_place.emplace(&(*iter), dist);
                }
            }
            if (iter->state() == node_type::node_state::init) {
                return *empty_place;
            }
            ++dist;
            if (dist > max_dist_ && empty_place) {
                return *empty_place;
            }
            ++iter;
            if (iter == nodes_.end()) {
                iter = nodes_.begin();
            }
        }
    }

    /*!
     * \brief Update maximum distance from the place determined by hash number
     * if needed.
     *
     * \param[in] dist Distance of the inserted value from the place determined
     * by hash number.
     */
    void update_max_dist_if_needed(size_type dist) {
        if (dist > max_dist_) {
            max_dist_ = dist;
        }
    }

    ///@}

    /*!
     * \name Internal functions to read values.
     */
    ///@{

    /*!
     * \brief Find a node index.
     *
     * \param[in] key Key.
     * \return Node index. (Null if not found.)
     */
    [[nodiscard]] auto find_node_ind_for(
        const key_type& key) const -> std::optional<size_type> {
        const size_type start_node_ind = desired_node_ind(key);
        auto iter = nodes_.begin() + start_node_ind;
        size_type dist = 0;
        while (true) {
            if (iter->state() == node_type::node_state::filled &&
                key_equal_(extract_key_(iter->value()), key)) {
                return iter - nodes_.begin();
            }
            if (iter->state() == node_type::node_state::init) {
                return std::nullopt;
            }
            ++dist;
            if (dist > max_dist_) {
                break;
            }
            ++iter;
            if (iter == nodes_.end()) {
                iter = nodes_.begin();
            }
        }
        return std::nullopt;
    }

    /*!
     * \brief Find a node index.
     *
     * \param[in] key Key.
     * \return Node index.
     * \throw std::out_of_range If not found.
     */
    [[nodiscard]] auto require_node_ind_for(
        const key_type& key) const -> size_type {
        const auto node_ind = find_node_ind_for(key);
        if (!node_ind) {
            throw key_not_found();
        }
        return *node_ind;
    }

    ///@}

    //! Nodes.
    std::vector<node_type, node_allocator_type> nodes_;

    //! Number of values.
    size_type size_{0};

    //! Function to extract keys from values.
    extract_key_type extract_key_;

    //! Hash function.
    hash_type hash_;

    //! Function to check whether keys are equal.
    key_equal_type key_equal_;

    //! Default maximum load factor.
    static constexpr float default_max_load_factor = 0.8F;

    //! Maximum load factor.
    float max_load_factor_{default_max_load_factor};

    //! Current maximum distance from the place determined by hash number.
    size_type max_dist_{0};

    //! Bit mask to get node index determined by hash number.
    size_type desired_node_ind_mask_{};
};

}  // namespace hash_tables::tables
