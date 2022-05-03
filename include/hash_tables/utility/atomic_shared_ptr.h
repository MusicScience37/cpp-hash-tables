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
 * \brief Definition of atomic_shared_ptr class.
 */
#pragma once

#include <atomic>
#include <memory>

namespace hash_tables::utility {

#if defined(__cpp_lib_atomic_shared_ptr) && !defined(HASH_TABLES_DOCUMENTATION)

/*!
 * \brief Class of atomic `std::shared_ptr`.
 *
 * \tparam T Type of the object in `std::shared_ptr`.
 */
template <typename T>
using atomic_shared_ptr = std::atomic<std::shared_ptr<T>>;

#else

/*!
 * \brief Class of atomic `std::shared_ptr`.
 *
 * \note When C++ standard library doesn't have
 * `std::atomic<std::shared_ptr<T>>`, this class is used instead.
 * For function APIs, I referred
 * [here](https://en.cppreference.com/w/cpp/memory/shared_ptr/atomic2).
 *
 * \tparam T Type of the object in `std::shared_ptr`.
 */
template <typename T>
class atomic_shared_ptr {
public:
    //! Type of values.
    using value_type = std::shared_ptr<T>;

    /*!
     * \brief Constructor.
     */
    atomic_shared_ptr() noexcept = default;

    /*!
     * \brief Constructor.
     *
     * \param[in] value Initial value.
     */
    explicit atomic_shared_ptr(std::shared_ptr<T> value) noexcept
        : value_(std::move(value)) {}

    atomic_shared_ptr(const atomic_shared_ptr&) = delete;
    atomic_shared_ptr(atomic_shared_ptr&&) = delete;
    auto operator=(const atomic_shared_ptr&) = delete;
    auto operator=(atomic_shared_ptr&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~atomic_shared_ptr() noexcept = default;

    /*!
     * \brief Check whether this object is lock free.
     *
     * \retval true This object is lock free.
     * \retval false This object is not lock free.
     */
    [[nodiscard]] auto is_lock_free() const noexcept {
        return std::atomic_is_lock_free(&value_);
    }

    /*!
     * \brief Store a value.
     *
     * \param[in] desired Desired value.
     * \param[in] order Memory order.
     */
    void store(std::shared_ptr<T> desired,
        std::memory_order order = std::memory_order_seq_cst) noexcept {
        std::atomic_store_explicit(&value_, std::move(desired), order);
    }

    /*!
     * \brief Load a value.
     *
     * \param[in] order Memory order.
     * \return Loaded value.
     */
    [[nodiscard]] auto load(
        std::memory_order order = std::memory_order_seq_cst) const noexcept
        -> std::shared_ptr<T> {
        return std::atomic_load_explicit(&value_, order);
    }

    /*!
     * \brief Exchange with a value.
     *
     * \param[in] desired Desired value.
     * \param[in] order Memory order.
     * \return Previous value.
     */
    [[nodiscard]] auto exchange(std::shared_ptr<T> desired,
        std::memory_order order = std::memory_order_seq_cst) noexcept
        -> std::shared_ptr<T> {
        return std::atomic_exchange_explicit(
            &value_, std::move(desired), order);
    }

    /*!
     * \brief Compare value with the expected value, and exchange to the desired
     * value if expectation is satisfied, otherwise get the current value.
     *
     * \param[in,out] expected Expected value. Current value is assigned if
     * different with the actual value.
     * \param[in] desired Desired value.
     * \param[in] order Memory order.
     * \retval true Value was exchaged.
     * \retval false Value was not exchaged.
     */
    auto compare_exchange_weak(std::shared_ptr<T>& expected,
        std::shared_ptr<T> desired,
        std::memory_order order = std::memory_order_seq_cst) noexcept -> bool {
        return compare_exchange_weak(
            expected, std::move(desired), order, order);
    }

    /*!
     * \brief Compare value with the expected value, and exchange to the desired
     * value if expectation is satisfied, otherwise get the current value.
     *
     * \param[in,out] expected Expected value. Current value is assigned if
     * different with the actual value.
     * \param[in] desired Desired value.
     * \param[in] success Memory order for successfull exchange.
     * \param[in] failure Memory order for failure.
     * \retval true Value was exchaged.
     * \retval false Value was not exchaged.
     */
    auto compare_exchange_weak(std::shared_ptr<T>& expected,
        std::shared_ptr<T> desired, std::memory_order success,
        std::memory_order failure) noexcept -> bool {
        return std::atomic_compare_exchange_weak_explicit(
            &value_, &expected, std::move(desired), success, failure);
    }

    /*!
     * \brief Compare value with the expected value, and exchange to the desired
     * value if expectation is satisfied, otherwise get the current value.
     *
     * \param[in,out] expected Expected value. Current value is assigned if
     * different with the actual value.
     * \param[in] desired Desired value.
     * \param[in] order Memory order.
     * \retval true Value was exchaged.
     * \retval false Value was not exchaged.
     */
    auto compare_exchange_strong(std::shared_ptr<T>& expected,
        std::shared_ptr<T> desired,
        std::memory_order order = std::memory_order_seq_cst) noexcept -> bool {
        return compare_exchange_strong(
            expected, std::move(desired), order, order);
    }

    /*!
     * \brief Compare value with the expected value, and exchange to the desired
     * value if expectation is satisfied, otherwise get the current value.
     *
     * \param[in,out] expected Expected value. Current value is assigned if
     * different with the actual value.
     * \param[in] desired Desired value.
     * \param[in] success Memory order for successfull exchange.
     * \param[in] failure Memory order for failure.
     * \retval true Value was exchaged.
     * \retval false Value was not exchaged.
     */
    auto compare_exchange_strong(std::shared_ptr<T>& expected,
        std::shared_ptr<T> desired, std::memory_order success,
        std::memory_order failure) noexcept -> bool {
        return std::atomic_compare_exchange_strong_explicit(
            &value_, &expected, std::move(desired), success, failure);
    }

private:
    //! Value.
    std::shared_ptr<T> value_{};
};

#endif

}  // namespace hash_tables::utility
