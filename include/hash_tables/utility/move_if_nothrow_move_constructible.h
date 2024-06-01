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
 * \brief Definition of move_if_nothrow_move_constructible function.
 */
#pragma once

#include <type_traits>

namespace hash_tables::utility {

/*!
 * \brief Move if the object is move constructible without exception.
 *
 * \note This functions uses SFINAE to check exceptions.
 *
 * \tparam T Type.
 * \param[in] obj Object.
 * \return Rvalue reference or constant lvalue reference of the object.
 */
template <typename T
#ifndef HASH_TABLES_DOCUMENTATION
    ,
    std::enable_if_t<std::is_nothrow_move_constructible_v<std::decay_t<T>>,
        void*> = nullptr
#endif
    >
[[nodiscard]] auto move_if_nothrow_move_constructible(T&& obj)
#ifndef HASH_TABLES_DOCUMENTATION
    -> std::remove_reference_t<T>&&
#endif
{
    return static_cast<std::remove_reference_t<T>&&>(obj);
}

#ifndef HASH_TABLES_DOCUMENTATION
template <typename T,
    std::enable_if_t<!std::is_nothrow_move_constructible_v<std::decay_t<T>>,
        void*> = nullptr>
[[nodiscard]] auto move_if_nothrow_move_constructible(
    T&& obj) -> const std::remove_reference_t<T>& {
    return obj;
}
#endif

}  // namespace hash_tables::utility
