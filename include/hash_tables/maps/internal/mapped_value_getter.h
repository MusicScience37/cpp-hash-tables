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
 * \brief Definition of mapped_value_getter class.
 */
#pragma once

#include <optional>
#include <utility>

namespace hash_tables::maps::internal {

/*!
 * \brief Class to get mapped values.
 *
 * \tparam MappedType Type of mapped values.
 */
template <typename MappedType>
class mapped_value_getter {
public:
    /*!
     * \brief Constructor.
     */
    mapped_value_getter() noexcept = default;

    /*!
     * \brief Assign a value.
     *
     * \tparam KeyType Type of the key.
     * \param[in] value Value.
     * \return This.
     */
    template <typename KeyType>
    auto operator=(const std::pair<KeyType, MappedType>& value)
        -> mapped_value_getter& {
        mapped_value_ = value.second;
        return *this;
    }

    /*!
     * \brief Check whether this has a value.
     *
     * \retval true This has a value.
     * \retval false This doesn't have a value.
     */
    explicit operator bool() const noexcept {
        return static_cast<bool>(mapped_value_);
    }

    /*!
     * \brief Release the value.
     *
     * \return Value.
     */
    [[nodiscard]] auto release() -> MappedType {
        return MappedType(std::move(mapped_value_.value()));
    }

private:
    //! Mapped value.
    std::optional<MappedType> mapped_value_{};
};

}  // namespace hash_tables::maps::internal
