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
 * \brief Definition of identity class.
 */
#pragma once

namespace hash_tables::extract_key_functions {

/*!
 * \brief Class to return the argument as is.
 *
 * \tparam ValueType Type of values.
 */
template <typename ValueType>
class identity {
public:
    //! Type of values.
    using value_type = ValueType;

    /*!
     * \brief Return the argument as is..
     *
     * \param[in] value Value.
     * \return Value.
     */
    [[nodiscard]] auto operator()(const value_type& value) const
        -> const value_type& {
        return value;
    }
};

}  // namespace hash_tables::extract_key_functions
