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
 * \brief Definition of cache_line class.
 */
#pragma once

#if defined(__cpp_lib_hardware_interference_size) || \
    defined(HASH_TABLES_DOCUMENTATION)

#include <cstddef>
#include <new>

namespace hash_tables::utility {

/*!
 * \brief Size of cache line.
 *
 * \note Some implementation of C++ standard library doesn't have
 * `std::hardware_destructive_interference_size` even in the latest release.
 * So alternative definition may be used in some platforms.
 */
inline constexpr std::size_t cache_line =
    std::hardware_destructive_interference_size;

}  // namespace hash_tables::utility

#else

#include <cstddef>

namespace hash_tables::utility {

/*!
 * \brief Size of cache line.
 *
 * \note This alternative definition is used when C++ standard library doesn't
 * have `std::hardware_destructive_interference_size`.
 * The number is valid for x86_64.
 */
inline constexpr std::size_t cache_line = 64;

}  // namespace hash_tables::utility

#endif
