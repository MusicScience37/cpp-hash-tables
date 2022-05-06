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
 * \brief Definition of value_storage class.
 */
#pragma once

#include <new>
#include <utility>

namespace hash_tables::utility {

/*!
 * \brief Class of storage of values.
 *
 * \tparam ValueType Type of values.
 *
 * \warning This class doesn't handle existence or lifetime of values unlike
 * `std::optional`.
 */
template <typename ValueType>
class value_storage {
public:
    //! Type of values.
    using value_type = ValueType;

    /*!
     * \brief Constructor.
     */
    value_storage() noexcept = default;

    // Copy and move can easily cause undefined behavior, so prohibit them.
    value_storage(const value_storage&) = delete;
    value_storage(value_storage&&) = delete;
    auto operator=(const value_storage&) -> value_storage& = delete;
    auto operator=(value_storage&&) -> value_storage& = delete;

    /*!
     * \brief Destructor.
     */
    ~value_storage() noexcept = default;

    /*!
     * \brief Construct a value.
     *
     * \tparam Args Type of arguments.
     * \param[in] args Arguments of the constructor.
     */
    template <typename... Args>
    void emplace(Args&&... args) {
        ::new (storage_) value_type(std::forward<Args>(args)...);
    }

    /*!
     * \brief Clear the value.
     */
    void clear() noexcept { get_pointer()->~value_type(); }

    /*!
     * \brief Get the pointer of the value.
     *
     * \return Pointer.
     */
    [[nodiscard]] auto get_pointer() noexcept -> value_type* {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return std::launder(reinterpret_cast<value_type*>(storage_));
    }

    /*!
     * \brief Get the pointer of the value.
     *
     * \return Pointer.
     */
    [[nodiscard]] auto get_pointer() const noexcept -> const value_type* {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return std::launder(reinterpret_cast<const value_type*>(storage_));
    }

    /*!
     * \brief Access the value.
     *
     * \return Reference.
     */
    [[nodiscard]] auto get() noexcept -> value_type& { return *get_pointer(); }

    /*!
     * \brief Access the value.
     *
     * \return Reference.
     */
    [[nodiscard]] auto get() const noexcept -> const value_type& {
        return *get_pointer();
    }

private:
    //! Storage for a value.
    alignas(alignof(value_type)) char storage_[sizeof(value_type)];  // NOLINT
};

}  // namespace hash_tables::utility
