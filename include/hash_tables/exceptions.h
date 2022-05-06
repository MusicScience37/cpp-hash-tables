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
 * \brief Definition of exceptions.
 */
#pragma once

#include <stdexcept>

namespace hash_tables {

/*!
 * \brief Class of exceptions thrown when key cannot be found.
 */
class key_not_found : public std::out_of_range {
public:
    /*!
     * \brief Constructor.
     */
    key_not_found() : std::out_of_range("Key not found.") {}

    using std::out_of_range::out_of_range;
    using std::out_of_range::operator=;
};

}  // namespace hash_tables
