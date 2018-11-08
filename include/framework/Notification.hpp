// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_NOTIFICATION_HPP
#define PROTOCOL_ANALYZER_NOTIFICATION_HPP

#include "Mutex.hpp"  // system::LocalMutex, system::LockGuard.


namespace analyzer::framework::task
{
    template <typename Type>
    class NotificationSubject;

    /**
     * @class Notification   Notification.hpp   "include/framework/Notification.hpp"
     * @brief Class that defines a notification template.
     *
     * @tparam [in] Type - The internal expected value type.
     *
     * @warning Previously initialize the NotificationInit class for correct work the notification template.
     */
    template <typename Type>
    class Notification
    {
        friend class NotificationSubject<Type>;

    private:
        /**
         * @brief Expected value for NotificationObserver.
         */
        Type internalValue;
        /**
         * @brief The expected event for NotificationObserver.
         */
        system::LocalMutex mutex = { };

        /**
         * @brief Private constructor of Notification class.
         *
         * @tparam [in] value - Init value.
         */
        explicit Notification (const Type& value) noexcept
            : internalValue(value)
        { }

    public:
        /**
         * @brief Default constructor.
         */
        Notification(void) = default;
        /**
         * @brief Default destructor of Notification class.
         */
        virtual ~Notification(void) = default;

        Notification (Notification &&) = delete;
        Notification (const Notification &) = delete;
        Notification & operator= (Notification &&) = delete;
        Notification & operator= (const Notification &) = delete;


        /**
         * @brief Method that returns the expected value when the event signal comes.
         *
         * @return The expected value of selected type.
         *
         * @warning This method blocks the thread causing it until the event signal arrives.
         */
        Type Wait(void) noexcept
        {
            system::LockGuard lock(mutex);
            return internalValue;
        }

        /**
         * @brief Method that expects the event signal until some time and then return the expected value.
         *
         * @tparam [out] output - Output value of selected type if the event signal has arrived.
         * @tparam [in] time - The time to which the thread will wait the event signal.
         * @return TRUE - if the event signal has arrived and the expected value can be obtained, otherwise - FALSE.
         */
        template <typename Duration>
        bool WaitUntil (Type& output, const std::chrono::time_point<std::chrono::system_clock, Duration>& time) noexcept
        {
            if (mutex.TryLockUntil(time) == true)
            {
                output = internalValue;
                mutex.Unlock();
                return true;
            }
            return false;
        }

        /**
         * @brief Method that expects the event signal within the specified time and then return the expected value.
         *
         * @tparam [out] output - Output value of selected type if the event signal has arrived.
         * @tparam [in] time - The time to which the thread will wait the event signal.
         * @return TRUE - if the event signal has arrived and the expected value can be obtained, otherwise - FALSE.
         */
        template <typename Rep, typename Period>
        bool WaitFor (Type& output, const std::chrono::duration<Rep, Period>& time) noexcept
        {
            return WaitUntil(output, std::chrono::system_clock::now() + time);
        }
    };


    /**
     * @class NotificationObserver   Notification.hpp   "include/framework/Notification.hpp"
     * @brief Class that defines an interface for observer part for event signal wait.
     *
     * @tparam [in] Type - The internal expected value type.
     *
     * @warning Previously initialize the NotificationInit class for correct work the notification template.
     */
    template <typename Type>
    class NotificationObserver : virtual public Notification<Type>
    {
    public:
        /**
         * @brief Default constructor of NotificationObserver class.
         */
        NotificationObserver(void) = default;
        /**
         * @brief Default constructor of NotificationObserver class.
         */
        ~NotificationObserver(void) = default;

        NotificationObserver (NotificationObserver &&) = delete;
        NotificationObserver (const NotificationObserver &) = delete;
        NotificationObserver & operator= (NotificationObserver &&) = delete;
        NotificationObserver & operator= (const NotificationObserver &) = delete;

        // Observer class only accessible to methods of base class.
    };


    /**
     * @class NotificationSubject   Notification.hpp   "include/framework/Notification.hpp"
     * @brief Class that defines an interface for subject part for sent event signal.
     *
     * @tparam [in] Type - The internal expected value type.
     *
     * @warning Previously initialize the NotificationInit class for correct work the notification template.
     */
    template <typename Type>
    class NotificationSubject : virtual public Notification<Type>
    {
    public:
        /**
         * @brief Default destructor of NotificationSubject class.
         */
        ~NotificationSubject(void) = default;

        NotificationSubject (NotificationSubject &&) = delete;
        NotificationSubject (const NotificationSubject &) = delete;
        NotificationSubject & operator= (NotificationSubject &&) = delete;
        NotificationSubject & operator= (const NotificationSubject &) = delete;

        /**
         * @brief Constructor of NotificationSubject class.
         */
        NotificationSubject(void) noexcept
        {
            [[maybe_unused]] bool unused = Notification<Type>::mutex.Lock();
        }

        explicit NotificationSubject (const Type& value) noexcept
            : Notification<Type>(value)
        {
            [[maybe_unused]] bool unused = Notification<Type>::mutex.Lock();
        }

        inline void SetValue (const Type& value) noexcept
        {
            Notification<Type>::internalValue = value;
        }

        inline void Notify(void) noexcept
        {
            Notification<Type>::mutex.ResetLockedFlag();
            Notification<Type>::mutex.Unlock();

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wempty-body"
            while (Notification<Type>::mutex.IsAlreadyLocked() == false);
#pragma clang diagnostic pop

            [[maybe_unused]] bool unused = Notification<Type>::mutex.Lock();
        }

        virtual NotificationObserver<Type>* ToObserver(void) noexcept
        {
            auto base = static_cast<Notification<Type>*>(this);
            return dynamic_cast<NotificationObserver<Type>*>(base);
        }
    };


    /**
     * @class NotificationInit   Notification.hpp   "include/framework/Notification.hpp"
     * @brief Class that defines an interface for init all notification template.
     *
     * @tparam [in] Type - The internal expected value type.
     */
    template <typename Type>
    class NotificationInit final : public NotificationSubject<Type>, public NotificationObserver<Type>
    {
    public:
        /**
         * @brief Default constructor of NotificationInit class.
         */
        NotificationInit(void) = default;
        /**
         * @brief Default destructor of NotificationInit class.
         */
        ~NotificationInit(void) = default;

        NotificationInit (NotificationInit &&) = delete;
        NotificationInit (const NotificationInit &) = delete;
        NotificationInit & operator= (NotificationInit &&) = delete;
        NotificationInit & operator= (const NotificationInit &) = delete;


        explicit NotificationInit (const Type& value) noexcept
            : NotificationSubject<Type>(value),
              NotificationObserver<Type>()
        { }

        inline NotificationObserver<Type>* ToObserver(void) noexcept final
        {
            return static_cast<NotificationObserver<Type>*>(this);
        }

        inline NotificationSubject<Type>* ToSubject(void) noexcept
        {
            return static_cast<NotificationSubject<Type>*>(this);
        }
    };


}  // namespace task.


#endif  // PROTOCOL_ANALYZER_NOTIFICATION_HPP
