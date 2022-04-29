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
 * \brief Definition of extract_first_from_pair class.
 */
#pragma once

namespace hash_tables::extract_key_functions {

/*!
 * \brief Class to extract the first element from a pair.
 *
 * \tparam ValueType Type of values.
 */
template <typename ValueType>
class extract_first_from_pair {
public:
    //! Type of values.
    using value_type = ValueType;

    /*!
     * \brief Extract the first element.
     *
     * \param[in] value Value.
     * \return First element.
     */
    [[nodiscard]] auto operator()(const value_type& value) const -> const
        typename value_type::first_type& {
        return value.first;
    }
};

}  // namespace hash_tables::extract_key_functions
