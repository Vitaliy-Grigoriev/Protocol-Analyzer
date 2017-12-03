// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#pragma once
#ifndef PROTOCOL_ANALYZER_LOCKED_DEQUE_HPP
#define PROTOCOL_ANALYZER_LOCKED_DEQUE_HPP

#include <mutex>    // std::mutex, std::scoped_lock, std::lock_guard.
#include <deque>    // std::deque.
#include <utility>  // std::move, std::swap.

// In System library MUST NOT use any another library because it is a core library.


namespace analyzer::common::types
{
    /**
     * @class LockedDeque LockedDeque.hpp "include/analyzer/LockedDeque.hpp"
     * @brief This class defined concurrency wrapper over STL deque class to work in parallel threads.
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
         * @var std::mutex mutex;
         * @brief A mutex value for thread-safe program working.
         */
        std::mutex mutex = { };
        /**
         * @var std::deque<Type> deque;
         * @brief STL deque for stored data;
         */
        std::deque<Type> deque = { };

    public:
        /**
         * @fn LockedDeque::LockedDeque(void) noexcept(true/false);
         * @brief Default constructor.
         *
         * @throw std::bad_alloc - In case when do not system memory to allocate the storage.
         */
        LockedDeque(void) noexcept(std::is_nothrow_default_constructible_v<std::deque<Type>>) = default;

        /**
         * @fn LockedDeque::~LockedDeque(void) noexcept;
         * @brief Default destructor.
         */
        ~LockedDeque(void) noexcept = default;

        /**
         * @fn LockedDeque::LockedDeque (const LockedDeque<Type> &) noexcept(true/false);
         * @brief Copy assignment constructor with LockedDeque<Type>.
         * @tparam [in] other - The const reference of copied LockedDeque<Type>.
         *
         * @throw std::bad_alloc - In case when do not system memory to allocate the storage.
         */
        LockedDeque (const LockedDeque<Type>& other) noexcept(std::is_nothrow_copy_constructible_v<std::deque<Type>>)
        {
            try { std::scoped_lock lock { mutex, other.mutex }; }
            catch (const std::system_error& /*err*/) {
                return;
            }
            deque = other.deque;
        }

        /**
         * @fn explicit LockedDeque::LockedDeque (const std::deque<Type> &) noexcept(true/false);
         * @brief Copy assignment constructor with STL std::deque<Type>.
         * @tparam [in] other - The const reference of copied STL std::deque<Type>.
         *
         * @throw std::bad_alloc - In case when do not system memory to allocate the storage.
         */
        explicit LockedDeque (const std::deque<Type>& other) noexcept(std::is_nothrow_copy_constructible_v<std::deque<Type>>)
        {
            try { std::lock_guard<std::mutex> lock { mutex }; }
            catch (const std::system_error& /*err*/) {
                return;
            }
            deque = other;
        }

        /**
         * @fn LockedDeque::LockedDeque (const LockedDeque<Type> &) noexcept;
         * @brief Move assignment constructor with LockedDeque<Type>.
         * @tparam [in] other - The rvalue reference of moved LockedDeque<Type>.
         */
        LockedDeque (LockedDeque<Type>&& other) noexcept
        {
            try { std::scoped_lock lock { mutex, other.mutex }; }
            catch (const std::system_error& /*err*/) {
                return;
            }
            deque = std::move(other.deque);
        }

        /**
         * @fn explicit LockedDeque::LockedDeque (const std::deque<Type> &) noexcept;
         * @brief Move assignment constructor with STL std::deque<Type>.
         * @tparam [in] other - The rvalue reference of moved STL std::deque<Type>.
         */
        explicit LockedDeque (std::deque<Type>&& other) noexcept
        {
            try { std::lock_guard<std::mutex> lock { mutex }; }
            catch (const std::system_error& /*err*/) {
                return;
            }
            deque = std::move(other);
        }

        /**
         * @fn LockedDeque<Type> & LockedDeque::operator= (const LockedDeque<Type> &) noexcept(true/false);
         * @brief Copy assignment operator.
         * @tparam [in] other - The const reference of copied LockedDeque<Type> class.
         * @return Reference of the current LockedDeque<Type> class.
         *
         * @throw std::bad_alloc - In case when do not system memory to allocate the storage.
         */
        LockedDeque<Type>& operator= (const LockedDeque<Type>& other) noexcept(std::is_nothrow_copy_assignable_v<std::deque<Type>>)
        {
            if (this != &other)
            {
                try { std::scoped_lock lock{mutex, other.mutex}; }
                catch (const std::system_error& /*err*/) {
                    return *this;
                }
                deque = other.deque;
            }
            return *this;
        }

        /**
         * @fn LockedDeque<Type> & LockedDeque::operator= (LockedDeque<Type> &&) noexcept;
         * @brief Move assignment operator.
         * @tparam [in] other - The rvalue reference of moved LockedDeque<Type> class.
         * @return Reference of the current LockedDeque<Type> class.
         */
        LockedDeque<Type>& operator= (LockedDeque<Type>&& other) noexcept
        {
            if (this != &other)
            {
                try { std::scoped_lock lock{mutex, other.mutex}; }
                catch (const std::system_error& /*err*/) {
                    return *this;
                }
                deque = std::move(other.deque);
            }
            return *this;
        }

        /**
         * @fn std::size_t LockedDeque::Size(void) noexcept;
         * @brief Method that returns the size of deque.
         * @return Size of deque.
         */
        std::size_t Size(void) noexcept
        {
            try { std::lock_guard<std::mutex> lock { mutex }; }
            catch (const std::system_error& /*err*/) {
                return 0;
            }
            return deque.size();
        }

        /**
         * @fn bool LockedDeque::IsEmpty(void) noexcept;
         * @brief Method that returns the internal state of deque.
         * @return True - if the container size is 0, otherwise - false.
         */
        bool IsEmpty(void) noexcept
        {
            try { std::lock_guard<std::mutex> lock { mutex }; }
            catch (const std::system_error& /*err*/) {
                return false;
            }
            return deque.empty();
        }

        /**
         * @fn bool LockedDeque::Push (const Type &) noexcept;
         * @brief Method that push new value to front of deque.
         * @tparam [in] value - New value for insert.
         * @return True - if push is successful, otherwise - false.
         */
        bool Push (const Type& value) noexcept
        {
            try {
                std::lock_guard<std::mutex> lock { mutex };
                deque.push_front(value);
            }
            catch (const std::exception& /*err*/) {
                return false;
            }
            return true;
        }

        /**
         * @fn bool LockedDeque::PopBack (Type &) noexcept;
         * @brief Method that pop value from back of deque.
         * @tparam [out] result - Returned value.
         * @return True - if pop back element is successful, otherwise - false.
         *
         * @note Use this method for pop the oldest value in the deque.
         */
        bool PopBack (Type& result) noexcept
        {
            try { std::lock_guard<std::mutex> lock { mutex }; }
            catch (const std::exception& /*err*/) {
                return false;
            }

            if (deque.empty() == true) {
                return false;
            }
            result = deque.back();
            deque.pop_back();
            return true;
        }

        /**
         * @fn bool LockedDeque::PopFront (Type &) noexcept;
         * @brief Method that pop value from front of deque.
         * @tparam [out] result - Returned value.
         * @return True - if pop the front element is successful, otherwise - false.
         */
        bool PopFront (Type& result) noexcept
        {
            try { std::lock_guard<std::mutex> lock { mutex }; }
            catch (const std::system_error& /*err*/) {
                return false;
            }

            if (deque.empty() == true) {
                return false;
            }
            result = deque.front();
            deque.pop_front();
            return true;
        }

        /**
         * @fn bool LockedDeque::Move (std::deque<Type> &) noexcept;
         * @brief Method that moves all internal values to outside STL std::deque<Type>.
         * @tparam [out] result - Returned value.
         * @return True - if at least 1 element has been moved, otherwise - false.
         */
        bool Move (std::deque<Type>& result) noexcept
        {
            try { std::lock_guard<std::mutex> lock { mutex }; }
            catch (const std::system_error& /*err*/) {
                return false;
            }

            if (deque.empty() == true) {
                return false;
            }
            result = std::move(deque);
            return true;
        }

        /**
         * @fn bool LockedDeque::Swap (LockedDeque<Type> &) noexcept;
         * @brief Method that swaps internal value with outside LockedDeque<Type> object.
         * @tparam [in,out] other - Swapped value reference.
         * @return True - if swap is successful, otherwise - false.
         */
        bool Swap (LockedDeque<Type>& other) noexcept
        {
            if (this != &other)
            {
                try { std::scoped_lock lock { mutex, other.mutex }; }
                catch (const std::system_error& /*err*/) {
                    return false;
                }
                std::swap(deque, other.deque);
            }
            return true;
        }

        /**
         * @fn bool LockedDeque::Swap (std::deque<Type> &) noexcept;
         * @brief Method that swaps internal value with outside STL std::deque<Type>.
         * @tparam [in,out] other - Swapped value reference.
         * @return True - if swap is successful, otherwise - false.
         */
        bool Swap (std::deque<Type>& other) noexcept
        {
            try { std::lock_guard<std::mutex> lock { mutex }; }
            catch (const std::system_error& /*err*/) {
                return false;
            }
            std::swap(deque, other);
            return true;
        }

        /**
         * @fn bool LockedDeque::Clear(void) noexcept;
         * @brief Method that clears the deque.
         * @return True - if clear is successful, otherwise - false.
         */
        bool Clear(void) noexcept
        {
            try { std::lock_guard<std::mutex> lock { mutex }; }
            catch (const std::system_error& /*err*/) {
                return false;
            }
            deque.clear();
            return true;
        }
    };

}  // namespace types.


#endif  // PROTOCOL_ANALYZER_LOCKED_DEQUE_HPP
