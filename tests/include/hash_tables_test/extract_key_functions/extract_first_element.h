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
 * \brief Definition of extract_first_element class.
 */
#pragma once

namespace hash_tables_test::extract_key_functions {

/*!
 * \brief Class to extract the first element from a value.
 *
 * \tparam ValueType Type of values.
 */
template <typename ValueType>
class extract_first_element {
public:
    //! Type of values.
    using value_type = ValueType;

    /*!
     * \brief Extract the first element.
     *
     * \param[in] value Value.
     * \return First element.
     */
    [[nodiscard]] auto operator()(const value_type& value) const ->
        typename value_type::const_reference {
        return value.at(0);
    }
};

}  // namespace hash_tables_test::extract_key_functions
