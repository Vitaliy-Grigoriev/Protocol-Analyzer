// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#pragma once
#ifndef PROTOCOL_ANALYZER_NOTIFICATION_HPP
#define PROTOCOL_ANALYZER_NOTIFICATION_HPP

#include <mutex>
#include <condition_variable>


namespace analyzer::task
{
    template <typename Type>
    class NotificationSubject
    {
    private:
        Type internalValue;

    public:
        NotificationSubject(void) = default;
        virtual ~NotificationSubject(void) = default;

        NotificationSubject (NotificationSubject &&) = delete;
        NotificationSubject (const NotificationSubject &) = delete;
        NotificationSubject & operator= (NotificationSubject &&) = delete;
        NotificationSubject & operator= (const NotificationSubject &) = delete;


        explicit NotificationSubject (const Type& value) noexcept {
            internalValue = value;
        }

        void SetValue (const Type& value) noexcept {
            internalValue = value;
        }

        virtual Type WaitAndGetValue(void) noexcept {
            return internalValue;
        }
    };


    template <typename Type>
    class NotificationObserver : public NotificationSubject<Type>
    {
    public:
        NotificationObserver(void) = default;

        Type WaitAndGetValue(void) noexcept final {
            return NotificationSubject<Type>::WaitAndGetValue();
        }
    };

}  // namespace task.


#endif  // PROTOCOL_ANALYZER_NOTIFICATION_HPP
