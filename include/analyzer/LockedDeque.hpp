// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#pragma once
#ifndef PROTOCOL_ANALYZER_LOCKEDDEQUE_HPP
#define PROTOCOL_ANALYZER_LOCKEDDEQUE_HPP

#include <mutex>
#include <deque>
#include <utility>

// In System library must not use any another library because it is a core library.


namespace analyzer::common::types
{
    /**
     * @class LockedDeque LockedDeque.hpp "include/analyzer/LockedDeque.hpp"
     * @brief This class defined concurrency wrapper over deque to work in parallel threads.
     * @tparam [in] Type - Typename of stored data in STL deque.
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
         * @fn LockedDeque::LockedDeque(void);
         * @brief Default constructor.
         */
        LockedDeque(void) = default;

        /**
         * @fn LockedDeque::~LockedDeque(void);
         * @brief Default destructor.
         */
        ~LockedDeque(void) = default;

        /**
         * @fn LockedDeque::LockedDeque (const LockedDeque &) noexcept;
         * @brief Copy constructor.
         * @param [in] other - The const reference of copied deque.
         */
        LockedDeque (const LockedDeque& other) noexcept
        {
            try { std::scoped_lock<std::mutex, std::mutex> lock { mutex, other.mutex }; }
            catch (const std::system_error& /*err*/) {
                return;
            }
            deque = other.deque;
        }

        /**
         * @fn LockedDeque::LockedDeque (const LockedDeque &) noexcept;
         * @brief Move assignment constructor.
         * @param [in] other - The lvalue reference of moved deque.
         */
        LockedDeque (LockedDeque&& other) noexcept
        {
            try { std::scoped_lock<std::mutex, std::mutex> lock { mutex, other.mutex }; }
            catch (const std::system_error& /*err*/) {
                return;
            }
            deque = std::move(other.deque);
        }

        /**
         * @fn LockedDeque & LockedDeque::operator= (const LockedDeque &) noexcept;
         * @brief Copy operator.
         * @param [in] other - The const reference of copied deque.
         * @return Reference of current class.
         */
        LockedDeque& operator= (const LockedDeque& other) noexcept
        {
            try { std::scoped_lock<std::mutex, std::mutex> lock { mutex, other.mutex }; }
            catch (const std::system_error& /*err*/) {
                return *this;
            }
            deque = other.deque;
            return *this;
        }

        /**
         * @fn LockedDeque & LockedDeque::operator= (LockedDeque &&) noexcept;
         * @brief Move assignment operator.
         * @param [in] other - The lvalue reference of moved deque.
         * @return Reference of current class.
         */
        LockedDeque& operator= (LockedDeque&& other) noexcept
        {
            try { std::scoped_lock<std::mutex, std::mutex> lock { mutex, other.mutex }; }
            catch (const std::system_error& /*err*/) {
                return *this;
            }
            deque = std::move(other.deque);
            return *this;
        }

        /**
         * @fn std::size_t LockedDeque::Size(void) const noexcept
         * @brief Method that returns the size of deque.
         * @return Size of deque.
         */
        std::size_t Size(void) const noexcept
        {
            try { std::lock_guard<std::mutex> lock { mutex }; }
            catch (const std::system_error& /*err*/) {
                return 0;
            }
            return deque.size();
        }

        /**
         * @fn bool LockedDeque::IsEmpty(void) const noexcept;
         * @brief Method that returns the internal state of deque.
         * @return State of deque.
         */
        bool IsEmpty(void) const noexcept
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
         * @param [in] value - New value for insert.
         * @return True - if push is successful, otherwise - false.
         */
        bool Push (const Type& value) noexcept
        {
            try { std::lock_guard<std::mutex> lock { mutex }; }
            catch (const std::system_error& /*err*/) {
                return false;
            }
            deque.emplace_front(value);
            return true;
        }

        /**
         * @fn bool LockedDeque::PopBack (Type &) noexcept;
         * @brief Method that pop value from back of deque.
         * @param [out] result - Returned value.
         * @return True - if pop is successful, otherwise - false.
         */
        bool PopBack (Type& result) noexcept
        {
            try { std::lock_guard<std::mutex> lock { mutex }; }
            catch (const std::system_error& /*err*/) {
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
         * @param [out] result - Returned value.
         * @return True - if pop is successful, otherwise - false.
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
            result = deque.begin();
            deque.pop_front();
            return true;
        }

        /**
         * @fn bool LockedDeque::Move (std::deque<Type> &) noexcept;
         * @brief Method that moves all internal values to outside STL deque.
         * @param [out] result - Returned value.
         * @return True - if move is successful, otherwise - false.
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


#endif  // PROTOCOL_ANALYZER_LOCKEDDEQUE_HPP
