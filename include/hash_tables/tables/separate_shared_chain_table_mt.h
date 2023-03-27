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
 * \brief Definition of separate_shared_chain_table_mt class.
 */
#pragma once

// IWYU pragma: no_include <string>

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <utility>
#include <vector>

#include "hash_tables/exceptions.h"
#include "hash_tables/hashes/default_hash.h"
#include "hash_tables/utility/cache_line.h"
#include "hash_tables/utility/round_up_to_power_of_two.h"

namespace hash_tables::tables {

/*!
 * \brief Class of concurrent hash tables using separate chains.
 *
 * \tparam ValueType Type of values.
 * \tparam KeyType Type of keys.
 * \tparam ExtractKey Type of the function to extract keys from values.
 * \tparam Hash Type of the hash function.
 * \tparam KeyEqual Type of the function to check whether keys are equal.
 * \tparam Allocator Type of allocators.
 *
 * \thread_safety Safe even for the same object.
 */
template <typename ValueType, typename KeyType, typename ExtractKey,
    typename Hash = hashes::default_hash<KeyType>,
    typename KeyEqual = std::equal_to<KeyType>,
    typename Allocator = std::allocator<ValueType>>
class separate_shared_chain_table_mt {
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

    //! Default number of buckets.
    static constexpr size_type default_num_buckets = 128;

    /*!
     * \brief Constructor.
     */
    separate_shared_chain_table_mt()
        : separate_shared_chain_table_mt(default_num_buckets) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] min_num_buckets Minimum number of buckets.
     * \param[in] extract_key Function to extract keys from values.
     * \param[in] hash Hash function.
     * \param[in] key_equal Function to check whether keys are equal.
     * \param[in] allocator Allocator.
     */
    explicit separate_shared_chain_table_mt(size_type min_num_buckets,
        extract_key_type extract_key = extract_key_type(),
        hash_type hash = hash_type(),
        key_equal_type key_equal = key_equal_type(),
        const allocator_type& allocator = allocator_type())
        : buckets_(bucket_allocator_type(allocator)),
          extract_key_(std::move(extract_key)),
          hash_(std::move(hash)),
          key_equal_(std::move(key_equal)) {
        size_type num_buckets =
            utility::round_up_to_power_of_two(min_num_buckets);
        constexpr size_type num_buckets_limit = 2;
        if (num_buckets < num_buckets_limit) {
            num_buckets = num_buckets_limit;
        }
        bucket_ind_mask_ = num_buckets - 1U;

        buckets_.reserve(num_buckets);
        for (size_type i = 0; i < num_buckets; ++i) {
            buckets_.push_back(std::make_unique<bucket_type>(allocator));
        }
    }

    separate_shared_chain_table_mt(
        const separate_shared_chain_table_mt&) = delete;
    separate_shared_chain_table_mt(separate_shared_chain_table_mt&&) = delete;
    auto operator=(const separate_shared_chain_table_mt&) = delete;
    auto operator=(separate_shared_chain_table_mt&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~separate_shared_chain_table_mt() noexcept = default;

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
        auto& bucket = bucket_for(key);
        std::unique_lock<std::shared_mutex> lock(bucket.mutex);
        if (std::none_of(bucket.nodes.begin(), bucket.nodes.end(),
                value_has_key_equal_to(key))) {
            bucket.nodes.emplace_back(std::forward<Args>(args)...);
            ++size_;
            return true;
        }
        return false;
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
        auto& bucket = bucket_for(key);
        std::unique_lock<std::shared_mutex> lock(bucket.mutex);
        const auto iter = std::find_if(bucket.nodes.begin(), bucket.nodes.end(),
            value_has_key_equal_to(key));
        if (iter != bucket.nodes.end()) {
            *iter = value_type(std::forward<Args>(args)...);
            return false;
        }
        bucket.nodes.emplace_back(std::forward<Args>(args)...);
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
        auto& bucket = bucket_for(key);
        std::unique_lock<std::shared_mutex> lock(bucket.mutex);
        const auto iter = std::find_if(bucket.nodes.begin(), bucket.nodes.end(),
            value_has_key_equal_to(key));
        if (iter != bucket.nodes.end()) {
            *iter = value_type(std::forward<Args>(args)...);
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
    [[nodiscard]] auto at(const key_type& key) const -> value_type {
        auto& bucket = bucket_for(key);
        std::shared_lock<std::shared_mutex> lock(bucket.mutex);
        const auto iter = std::find_if(bucket.nodes.begin(), bucket.nodes.end(),
            value_has_key_equal_to(key));
        if (iter != bucket.nodes.end()) {
            return *iter;
        }
        throw key_not_found();
    }

    /*!
     * \brief Get a value.
     *
     * \tparam ValueOutput Type of the value for output.
     * \param[in] key Key.
     * \param[out] value Value.
     */
    template <typename ValueOutput>
    void get_to(ValueOutput& value, const key_type& key) const {
        auto& bucket = bucket_for(key);
        std::shared_lock<std::shared_mutex> lock(bucket.mutex);
        const auto iter = std::find_if(bucket.nodes.begin(), bucket.nodes.end(),
            value_has_key_equal_to(key));
        if (iter != bucket.nodes.end()) {
            value = *iter;
            return;
        }
        throw key_not_found();
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
        -> value_type {
        auto& bucket = bucket_for(key);
        std::unique_lock<std::shared_mutex> lock(bucket.mutex);
        const auto iter = std::find_if(bucket.nodes.begin(), bucket.nodes.end(),
            value_has_key_equal_to(key));
        if (iter != bucket.nodes.end()) {
            return *iter;
        }
        auto& value = bucket.nodes.emplace_back(std::forward<Args>(args)...);
        ++size_;
        return value;
    }

    /*!
     * \brief Get a value constructing it if not found.
     *
     * \tparam ValueOutput Type of the value for output.
     * \tparam Args Type of arguments of the constructor.
     * \param[out] value Value.
     * \param[in] key Key.
     * \param[in] args Arguments of the constructor.
     */
    template <typename ValueOutput, typename... Args>
    void get_or_create_to(
        ValueOutput& value, const key_type& key, Args&&... args) {
        auto& bucket = bucket_for(key);
        std::unique_lock<std::shared_mutex> lock(bucket.mutex);
        const auto iter = std::find_if(bucket.nodes.begin(), bucket.nodes.end(),
            value_has_key_equal_to(key));
        if (iter != bucket.nodes.end()) {
            value = *iter;
            return;
        }
        const auto& value_temp =
            bucket.nodes.emplace_back(std::forward<Args>(args)...);
        ++size_;
        value = value_temp;
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
        const key_type& key, Function&& function) -> value_type {
        auto& bucket = bucket_for(key);
        std::unique_lock<std::shared_mutex> lock(bucket.mutex);
        const auto iter = std::find_if(bucket.nodes.begin(), bucket.nodes.end(),
            value_has_key_equal_to(key));
        if (iter != bucket.nodes.end()) {
            return *iter;
        }
        auto& value = bucket.nodes.emplace_back(
            std::invoke(std::forward<Function>(function)));
        ++size_;
        return value;
    }

    /*!
     * \brief Get a value constructing using a factory function it if not found.
     *
     * \tparam ValueOutput Type of the value for output.
     * \tparam Function Type of the factory function.
     * \param[out] value Value.
     * \param[in] key Key.
     * \param[in] function Factory function.
     */
    template <typename ValueOutput, typename Function>
    void get_or_create_with_factory_to(
        ValueOutput& value, const key_type& key, Function&& function) {
        auto& bucket = bucket_for(key);
        std::unique_lock<std::shared_mutex> lock(bucket.mutex);
        const auto iter = std::find_if(bucket.nodes.begin(), bucket.nodes.end(),
            value_has_key_equal_to(key));
        if (iter != bucket.nodes.end()) {
            value = *iter;
            return;
        }
        const auto& value_temp = bucket.nodes.emplace_back(
            std::invoke(std::forward<Function>(function)));
        ++size_;
        value = value_temp;
    }

    /*!
     * \brief Get a value if found.
     *
     * \param[in] key Key.
     * \return Value if found, otherwise null.
     */
    [[nodiscard]] auto try_get(const key_type& key) const
        -> std::optional<value_type> {
        auto& bucket = bucket_for(key);
        std::shared_lock<std::shared_mutex> lock(bucket.mutex);
        const auto iter = std::find_if(bucket.nodes.begin(), bucket.nodes.end(),
            value_has_key_equal_to(key));
        if (iter != bucket.nodes.end()) {
            return *iter;
        }
        return std::nullopt;
    }

    /*!
     * \brief Get a value if found.
     *
     * \tparam ValueOutput Type of the value for output.
     * \param[in] key Key.
     * \param[out] value Value.
     * \retval true Value was found and assigned to value.
     * \retval false Value was not found.
     */
    template <typename ValueOutput>
    auto try_get_to(ValueOutput& value, const key_type& key) const -> bool {
        auto& bucket = bucket_for(key);
        std::shared_lock<std::shared_mutex> lock(bucket.mutex);
        const auto iter = std::find_if(bucket.nodes.begin(), bucket.nodes.end(),
            value_has_key_equal_to(key));
        if (iter != bucket.nodes.end()) {
            value = *iter;
            return true;
        }
        return false;
    }

    /*!
     * \brief Check whether a key exists.
     *
     * \param[in] key Key.
     * \retval true Key exists.
     * \retval false Key doesn't exists.
     */
    [[nodiscard]] auto has(const key_type& key) const -> bool {
        auto& bucket = bucket_for(key);
        std::shared_lock<std::shared_mutex> lock(bucket.mutex);
        const auto iter = std::find_if(bucket.nodes.begin(), bucket.nodes.end(),
            value_has_key_equal_to(key));
        return iter != bucket.nodes.end();
    }

    /*!
     * \brief Call a function with all values.
     *
     * \tparam Function Type of the function.
     * \param[in] function Function.
     */
    template <typename Function>
    void for_all(Function&& function) {
        for (auto& bucket_ptr : buckets_) {
            auto& bucket = *bucket_ptr;
            std::shared_lock<std::shared_mutex> lock(bucket.mutex);
            for (auto& node : bucket.nodes) {
                std::invoke(function, static_cast<value_type&>(node));
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
        for (auto& bucket_ptr : buckets_) {
            auto& bucket = *bucket_ptr;
            std::shared_lock<std::shared_mutex> lock(bucket.mutex);
            for (auto& node : bucket.nodes) {
                std::invoke(function, static_cast<const value_type&>(node));
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
    void clear() {
        for (auto& bucket_ptr : buckets_) {
            auto& bucket = *bucket_ptr;
            std::unique_lock<std::shared_mutex> lock(bucket.mutex);
            size_type erased_size = bucket.nodes.size();
            bucket.nodes.clear();
            size_ -= erased_size;
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
        auto& bucket = bucket_for(key);
        std::unique_lock<std::shared_mutex> lock(bucket.mutex);
        const auto iter = std::find_if(bucket.nodes.begin(), bucket.nodes.end(),
            value_has_key_equal_to(key));
        if (iter != bucket.nodes.end()) {
            bucket.nodes.erase(iter);
            --size_;
            return true;
        }
        return false;
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
        size_type erased_count = 0;
        for (auto& bucket_ptr : buckets_) {
            auto& bucket = *bucket_ptr;
            std::unique_lock<std::shared_mutex> lock(bucket.mutex);
            for (auto iter = bucket.nodes.begin();
                 iter != bucket.nodes.end();) {
                if (std::invoke(
                        function, static_cast<const value_type&>(*iter))) {
                    iter = bucket.nodes.erase(iter);
                    --size_;
                    ++erased_count;
                } else {
                    ++iter;
                }
            }
        }
        return erased_count;
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
        return buckets_.at(0)->nodes.max_size() * buckets_.size();
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
        return allocator_type(buckets_.get_allocator());
    }

    /*!
     * \brief Get the number of buckets.
     *
     * \return Number of buckets.
     */
    [[nodiscard]] auto num_buckets() const noexcept -> size_type {
        return buckets_.size();
    }

    /*!
     * \brief Get the load factor (number of values / number of buckets).
     *
     * \return Load factor.
     */
    auto load_factor() -> float {
        return static_cast<float>(size_) / static_cast<float>(buckets_.size());
    }

    ///@}

private:
    //! Struct of buckets.
    struct alignas(utility::cache_line) bucket_type {
        //! Nodes.
        std::vector<value_type, allocator_type> nodes;

        //! Mutex.
        std::shared_mutex mutex{};

        /*!
         * \brief Constructor.
         *
         * \param[in] allocator Allocator.
         */
        explicit bucket_type(const allocator_type& allocator)
            : nodes(allocator) {}
    };

    //! Type of pointers of buckets.
    using bucket_ptr_type = std::unique_ptr<bucket_type>;

    //! Type of allocators of buckets.
    using bucket_allocator_type = typename std::allocator_traits<
        allocator_type>::template rebind_alloc<bucket_ptr_type>;

    /*!
     * \brief Calculate the bucket index using hash number.
     *
     * \param[in] key Key.
     * \return Bucket index.
     */
    [[nodiscard]] auto bucket_ind_of(const key_type& key) const -> size_type {
        const size_type hash_number = hash_(key);
        return hash_number & bucket_ind_mask_;
    }

    /*!
     * \brief Access to the bucket for a key.
     *
     * \param[in] key Key.
     * \return Bucket.
     */
    [[nodiscard]] auto bucket_for(const key_type& key) const -> bucket_type& {
        return *buckets_[bucket_ind_of(key)];
    }

    /*!
     * \brief Get a function to check whether a value has a key equal to the
     * given key.
     *
     * \param[in] key Key.
     * \return Function.
     */
    [[nodiscard]] auto value_has_key_equal_to(const key_type& key) const {
        return [this, &key](const value_type& value) -> bool {
            return key_equal_(extract_key_(value), key);
        };
    }

    //! Buckets.
    std::vector<bucket_ptr_type, bucket_allocator_type> buckets_;

    //! Number of values.
    std::atomic<size_type> size_{0};

    //! Function to extract keys from values.
    extract_key_type extract_key_;

    //! Hash function.
    hash_type hash_;

    //! Function to check whether keys are equal.
    key_equal_type key_equal_;

    //! Bit mask to get bucket index determined by hash number.
    size_type bucket_ind_mask_{};
};

}  // namespace hash_tables::tables
