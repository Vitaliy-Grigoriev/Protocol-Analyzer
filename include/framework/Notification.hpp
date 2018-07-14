// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#ifndef PROTOCOL_ANALYZER_NOTIFICATION_HPP
#define PROTOCOL_ANALYZER_NOTIFICATION_HPP

#include <chrono>

#include "Mutex.hpp"


namespace analyzer::task
{
    template <typename Type>
    class NotificationSubject;

    /**
     * @class Notification Notification.hpp "include/analyzer/Notification.hpp"
     * @brief Class that defines a notification template.
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
         * @var Type internalValue;
         * @brief Expected value.
         */
        Type internalValue;
        /**
         * @var system::LocalMutex mutex;
         * @brief The expected event.
         */
        system::LocalMutex mutex = { };

        /**
         * @fn explicit Notification::Notification (const Type &) noexcept;
         * @brief Private constructor of Notification class.
         * @tparam [in] value - Init value.
         */
        explicit Notification (const Type& value) noexcept
                : internalValue(value)
        { }

    public:
        /**
         * @fn Notification::Notification(void);
         * @brief Default constructor.
         */
        Notification(void) = default;
        /**
         * @fn Notification::~Notification(void);
         * @brief Default destructor of Notification class.
         */
        virtual ~Notification(void) = default;

        Notification (Notification &&) = delete;
        Notification (const Notification &) = delete;
        Notification & operator= (Notification &&) = delete;
        Notification & operator= (const Notification &) = delete;


        /**
         * @fn Type Notification::Wait(void) noexcept;
         * @brief Method that returns the expected value when the event signal comes.
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
         * @fn template <typename Duration>
         * bool Notification::WaitUntil (Type &, const std::chrono::time_point<std::chrono::system_clock, Duration> &) noexcept;
         * @brief Method that expects the event signal until some time and then return the expected value.
         * @tparam [out] output - Output value of selected type if the event signal has arrived.
         * @tparam [in] time - The time to which the thread will wait the event signal.
         * @return True - if the event signal has arrived and the expected value can be obtained, otherwise - false.
         */
        template <typename Duration>
        bool WaitUntil (Type& output, const std::chrono::time_point<std::chrono::system_clock, Duration>& time) noexcept
        {
            if (mutex.TryLockUntil(time) == true) {
                output = internalValue;
                mutex.Unlock();
                return true;
            }
            return false;
        }

        /**
         * @fn template <typename Rep, typename Period>
         * bool Notification::WaitFor (Type &, const std::chrono::duration<Rep, Period> &) noexcept;
         * @brief Method that expects the event signal within the specified time and then return the expected value.
         * @tparam [out] output - Output value of selected type if the event signal has arrived.
         * @tparam [in] time - The time to which the thread will wait the event signal.
         * @return True - if the event signal has arrived and the expected value can be obtained, otherwise - false.
         */
        template <typename Rep, typename Period>
        bool WaitFor (Type& output, const std::chrono::duration<Rep, Period>& time) noexcept
        {
            return WaitUntil(output, std::chrono::system_clock::now() + time);
        }
    };


    /**
     * @class NotificationObserver Notification.hpp "include/analyzer/Notification.hpp"
     * @brief Class that defines an interface for observer part for event signal wait.
     * @tparam [in] Type - The internal expected value type.
     *
     * @warning Previously initialize the NotificationInit class for correct work the notification template.
     */
    template <typename Type>
    class NotificationObserver : virtual public Notification<Type>
    {
    public:
        /**
         * @fn NotificationObserver::NotificationObserver(void);
         * @brief Default constructor of NotificationObserver class.
         */
        NotificationObserver(void) = default;
        /**
         * @fn NotificationObserver::~NotificationObserver(void);
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
     * @class NotificationSubject Notification.hpp "include/analyzer/Notification.hpp"
     * @brief Class that defines an interface for subject part for sent event signal.
     * @tparam [in] Type - The internal expected value type.
     *
     * @warning Previously initialize the NotificationInit class for correct work the notification template.
     */
    template <typename Type>
    class NotificationSubject : virtual public Notification<Type>
    {
    public:
        /**
         * @fn NotificationSubject::~NotificationSubject(void);
         * @brief Default destructor of NotificationSubject class.
         */
        ~NotificationSubject(void) = default;

        NotificationSubject (NotificationSubject &&) = delete;
        NotificationSubject (const NotificationSubject &) = delete;
        NotificationSubject & operator= (NotificationSubject &&) = delete;
        NotificationSubject & operator= (const NotificationSubject &) = delete;

        /**
         * @fn NotificationSubject::NotificationSubject(void) noexcept;
         * @brief Constructor of NotificationSubject class.
         */
        NotificationSubject(void) noexcept
        {
            Notification<Type>::mutex.Lock();
        }

        explicit NotificationSubject (const Type& value) noexcept
                : Notification<Type>(value)
        {
            Notification<Type>::mutex.Lock();
        }

        inline void SetValue (const Type& value) noexcept
        {
            Notification<Type>::internalValue = value;
        }

        inline void Notify(void) noexcept
        {
            Notification<Type>::mutex.ResetFlag();
            Notification<Type>::mutex.Unlock();
            while (Notification<Type>::mutex.IsAlreadyLocked() == false) { }
            Notification<Type>::mutex.Lock();
        }

        virtual NotificationObserver<Type>* ToObserver(void) noexcept
        {
            auto base = static_cast<Notification<Type> *>(this);
            return dynamic_cast<NotificationObserver<Type> *>(base);
        }
    };


    /**
     * @class NotificationInit Notification.hpp "include/analyzer/Notification.hpp"
     * @brief Class that defines an interface for init all notification template.
     * @tparam [in] Type - The internal expected value type.
     */
    template <typename Type>
    class NotificationInit final : public NotificationSubject<Type>, public NotificationObserver<Type>
    {
    public:
        /**
         * @fn NotificationInit::NotificationInit(void);
         * @brief Default constructor of NotificationInit class.
         */
        NotificationInit(void) = default;
        /**
         * @fn NotificationInit::~NotificationInit(void);
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
            return static_cast<NotificationObserver<Type> *>(this);
        }

        inline NotificationSubject<Type>* ToSubject(void) noexcept
        {
            return static_cast<NotificationSubject<Type> *>(this);
        }
    };


}  // namespace task.


#endif  // PROTOCOL_ANALYZER_NOTIFICATION_HPP
