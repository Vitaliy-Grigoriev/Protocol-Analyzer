// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_LOCKED_DEQUE_HPP
#define PROTOCOL_ANALYZER_LOCKED_DEQUE_HPP

#include <mutex>    // std::mutex, std::scoped_lock, std::lock_guard.
#include <deque>    // std::deque.
#include <utility>  // std::move, std::swap.

// In Common library MUST NOT use any another framework libraries because it is a core library.


namespace analyzer::framework::common::types
{
    /**
     * @class LockedDeque   LockedDeque.hpp   "include/framework/LockedDeque.hpp"
     * @brief This class defined concurrency wrapper over STL deque class to work in parallel threads.
     *
     * @tparam [in] Type - Typename of stored data in STL deque.
     *
     * @note This deque container if type-protected and provides a convenient RAII-style mechanism.
     *
     * @todo Set the global limit for adding new values to the deque.
     */
    template <typename Type>
    class LockedDeque
    {
    private:
        /**
         * @brief Mutex value for thread-safe class working.
         */
        std::mutex mutex = { };
        /**
         * @brief STL deque for stored data;
         */
        std::deque<Type> deque = { };

    public:
        /**
         * @brief Default constructor.
         *
         * @throw std::bad_alloc - If there is not enough system memory for allocate the storage.
         */
        LockedDeque(void) noexcept(std::is_nothrow_default_constructible_v<std::deque<Type>>) = default;

        /**
         * @brief Default destructor.
         */
        ~LockedDeque(void) noexcept = default;

        /**
         * @brief Copy assignment constructor with LockedDeque.
         *
         * @tparam [in] other - Constant lvalue reference of copied LockedDeque.
         *
         * @throw std::bad_alloc - If there is not enough system memory for allocate the storage.
         */
        LockedDeque (const LockedDeque& other) noexcept(std::is_nothrow_copy_constructible_v<std::deque<Type>>)
        {
            try
            {
                std::scoped_lock lock { mutex, other.mutex };
                deque = other.deque;
            }
            catch (const std::system_error& /*err*/) { }
        }

        /**
         * @brief Copy assignment constructor with STL std::deque.
         *
         * @tparam [in] other - Constant lvalue reference of copied STL std::deque.
         *
         * @throw std::bad_alloc - If there is not enough system memory for allocate the storage.
         */
        explicit LockedDeque (const std::deque<Type>& other) noexcept(std::is_nothrow_copy_constructible_v<std::deque<Type>>)
        {
            try
            {
                std::lock_guard<std::mutex> lock { mutex };
                deque = other;
            }
            catch (const std::system_error& /*err*/) { }
        }

        /**
         * @brief Move assignment constructor with LockedDeque.
         *
         * @tparam [in] other - Rvalue reference of moved LockedDeque.
         */
        LockedDeque (LockedDeque&& other) noexcept
        {
            try
            {
                std::scoped_lock lock { mutex, other.mutex };
                deque = std::move(other.deque);
            }
            catch (const std::system_error& /*err*/) { }
        }

        /**
         * @brief Move assignment constructor with STL std::deque.
         *
         * @tparam [in] other - Rvalue reference of moved STL std::deque.
         */
        explicit LockedDeque (std::deque<Type>&& other) noexcept
        {
            try
            {
                std::lock_guard<std::mutex> lock { mutex };
                deque = std::move(other);
            }
            catch (const std::system_error& /*err*/) { }
        }

        /**
         * @brief Copy assignment operator.
         *
         * @tparam [in] other - Constant lvalue reference of copied LockedDeque class.
         * @return Lvalue reference of current LockedDeque class.
         *
         * @throw std::bad_alloc - If there is not enough system memory for allocate the storage.
         */
        LockedDeque& operator= (const LockedDeque& other) noexcept(std::is_nothrow_copy_assignable_v<std::deque<Type>>)
        {
            if (this != &other)
            {
                try
                {
                    std::scoped_lock lock { mutex, other.mutex };
                    deque = other.deque;
                }
                catch (const std::system_error& /*err*/) { }
            }
            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @tparam [in] other - Rvalue reference of moved LockedDeque class.
         * @return Lvalue reference of current LockedDeque class.
         */
        LockedDeque& operator= (LockedDeque&& other) noexcept
        {
            if (this != &other)
            {
                try
                {
                    std::scoped_lock lock { mutex, other.mutex };
                    deque = std::move(other.deque);
                }
                catch (const std::system_error& /*err*/) { }
            }
            return *this;
        }

        /**
         * @brief Method that returns the size of deque.
         *
         * @return Size of deque.
         */
        std::size_t Size(void) noexcept
        {
            try
            {
                std::lock_guard<std::mutex> lock { mutex };
                return deque.size();
            }
            catch (const std::system_error& /*err*/) {
                return 0;
            }
        }

        /**
         * @brief Method that returns the internal state of deque.
         *
         * @return TRUE - if container is empty, otherwise - FALSE.
         */
        bool IsEmpty(void) noexcept
        {
            try
            {
                std::lock_guard<std::mutex> lock { mutex };
                return deque.empty();
            }
            catch (const std::system_error& /*err*/) {
                return false;
            }
        }

        /**
         * @brief Method that push new value to front of deque.
         *
         * @tparam [in] value - New value for insert.
         * @return TRUE - if pushing is successful, otherwise - FALSE.
         */
        bool Push (const Type& value) noexcept
        {
            try
            {
                std::lock_guard<std::mutex> lock { mutex };
                deque.push_front(value);
            }
            catch (const std::exception& /*err*/) {
                return false;
            }
            return true;
        }

        /**
         * @brief Method that pops value from the back of deque.
         *
         * @tparam [out] result - Returned value.
         * @return TRUE - if popping element from back is successful, otherwise - FALSE.
         *
         * @note Use this method for pop the oldest value in the deque.
         */
        bool PopBack (Type& result) noexcept
        {
            try
            {
                std::lock_guard<std::mutex> lock { mutex };
                if (deque.empty() == true) {
                    return false;
                }
                result = deque.back();
                deque.pop_back();
            }
            catch (const std::exception& /*err*/) {
                return false;
            }
            return true;
        }

        /**
         * @brief Method that pops value from the front of deque.
         *
         * @tparam [out] result - Returned value.
         * @return TRUE - if popping element from front is successful, otherwise - FALSE.
         */
        bool PopFront (Type& result) noexcept
        {
            try
            {
                std::lock_guard<std::mutex> lock { mutex };
                if (deque.empty() == true) {
                    return false;
                }
                result = deque.front();
                deque.pop_front();
            }
            catch (const std::system_error& /*err*/) {
                return false;
            }
            return true;
        }

        /**
         * @brief Method that moves all internal values to outside STL std::deque.
         *
         * @tparam [out] result - Returned value.
         * @return TRUE - if at least one element has been moved, otherwise - FALSE.
         */
        bool Move (std::deque<Type>& result) noexcept
        {
            try
            {
                std::lock_guard<std::mutex> lock { mutex };
                if (deque.empty() == true) {
                    return false;
                }
                result = std::move(deque);
            }
            catch (const std::system_error& /*err*/) {
                return false;
            }
            return true;
        }

        /**
         * @brief Method that swaps internal value with outside LockedDeque object.
         *
         * @tparam [in,out] other - Lvalue reference of swapped value.
         * @return TRUE - if swapping is successful, otherwise - FALSE.
         */
        bool Swap (LockedDeque& other) noexcept
        {
            if (this != &other)
            {
                try
                {
                    std::scoped_lock lock { mutex, other.mutex };
                    std::swap(deque, other.deque);
                }
                catch (const std::system_error& /*err*/) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Method that swaps internal value with outside STL std::deque.
         *
         * @tparam [in,out] other - Lvalue reference of swapped value.
         * @return TRUE - if swapping is successful, otherwise - FALSE.
         */
        bool Swap (std::deque<Type>& other) noexcept
        {
            try
            {
                std::lock_guard<std::mutex> lock { mutex };
                std::swap(deque, other);
            }
            catch (const std::system_error& /*err*/) {
                return false;
            }
            return true;
        }

        /**
         * @brief Method that clears the deque.
         *
         * @return TRUE - if clearing is successful, otherwise - FALSE.
         */
        bool Clear(void) noexcept
        {
            try
            {
                std::lock_guard<std::mutex> lock { mutex };
                deque.clear();
            }
            catch (const std::system_error& /*err*/) {
                return false;
            }
            return true;
        }
    };

}  // namespace types.


#endif  // PROTOCOL_ANALYZER_LOCKED_DEQUE_HPP
