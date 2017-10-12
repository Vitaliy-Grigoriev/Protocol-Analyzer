#pragma once
#ifndef PROTOCOL_ANALYZER_LOG_HPP
#define PROTOCOL_ANALYZER_LOG_HPP

#include <mutex>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <exception>

#include "Common.hpp"


/**
 * @def GET_ERROR(error)
 * @brief Marco that return a system error in string format.
 * @param [in] error - The error code number.
 * @return The string that contains a description of the error.
 */
#define GET_ERROR(error) (analyzer::log::StrSysError::Instance())(error)


/**
 * @defgroup LOG_FUNCTIONS Global logging defines.
 * @brief This defines present all interfaces for output log with several attributes.
 *
 * @note In this code is used GNU extension "Named Variadic Macros".
 *
 * @{
 */

/**
 * @def LOG_TRACE (args...)
 * @brief Marco that output message to the log file with TRACE level attribute.
 * @tparam [in] args - The sequence of parameters for output to the log file.
 */
#define LOG_TRACE(args...) (analyzer::log::Logging::Instance().Push(analyzer::log::LEVEL::TRACE, args))
/**
 * @def LOG_INFO (args...)
 * @brief Marco that output message to the log file with INFO level attribute.
 * @tparam [in] args - The sequence of parameters for output to the log file.
 */
#define LOG_INFO(args...) (analyzer::log::Logging::Instance().Push(analyzer::log::LEVEL::INFORMATION, args))
/**
 * @def LOG_WARNING (args...)
 * @brief Marco that output message to the log file with WARNING level attribute.
 * @tparam [in] args - The sequence of parameters for output to the log file.
 */
#define LOG_WARNING(args...) (analyzer::log::Logging::Instance().Push(analyzer::log::LEVEL::WARNING, args))
/**
 * @def LOG_ERROR (args...)
 * @brief Marco that output message to the log file with ERROR level attribute.
 * @tparam [in] args - The sequence of parameters for output to the log file.
 */
#define LOG_ERROR(args...) (analyzer::log::Logging::Instance().Push(analyzer::log::LEVEL::ERROR, args))
/**
 * @def LOG_FATAL (args...)
 * @brief Marco that output message to the log file with FATAL level attribute.
 * @tparam [in] args - The sequence of parameters for output to the log file.
 */
#define LOG_FATAL(args...) (analyzer::log::Logging::Instance().Push(analyzer::log::LEVEL::FATAL, args))

/**@}*/


namespace analyzer::log
{
    /**
     * @enum LEVEL
     * @brief The level of logging.
     *
     * @note The level TRACE work only in debug mode.
     */
    enum LEVEL : uint16_t
    {
        TRACE = 0,
        INFORMATION = 1,
        WARNING = 2,
        ERROR = 3,
        FATAL = 4
    };


    /**
     * @class StrSysError Log.hpp "include/analyzer/Log.hpp"
     * @brief This singleton class defined the interface of receipt an system error.
     *
     * This singleton class is thread-safe.
     * To use this class, use the macro GET_ERROR(const int32_t ErrorCode).
     */
    class StrSysError
    {
    protected:
        /**
         * @fn StrSysError::StrSysError(void);
         * @brief Protect constructor.
         */
        StrSysError(void) noexcept;

        /**
         * @fn StrSysError::~StrSysError(void) = default;
         * @brief Protect default destructor.
         */
        ~StrSysError(void) = default;

    public:
        StrSysError (StrSysError &&) = delete;
        StrSysError (const StrSysError &) = delete;
        StrSysError & operator= (StrSysError &&) = delete;
        StrSysError & operator= (const StrSysError &) = delete;

        /**
         * @fn static StrSysError & StrSysError::Instance(void);
         * @brief Method that returns the instance of the system error singleton class.
         * @return The instance of singleton class.
         */
        static StrSysError & Instance(void) noexcept;

        /**
         * @fn std::string StrSysError::operator() (int32_t) const noexcept;
         * @brief Operator that returns a system error in string format.
         * @param [in] error - The error code number.
         * @return The string that contains a description of the error.
         */
        std::string operator() (int32_t /*error*/) const noexcept;
    };


    /**
     * @class Logging Log.hpp "include/analyzer/Log.hpp"
     * @brief This singleton class defined the interface for program logging.
     *
     * This singleton class is thread-safe.
     * To use this class, use following macro definitions: LOG_TRACE, LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_FATAL.
     */
    class Logging
    {
    private:
        /**
         * @var std::mutex log_mutex;
         * @brief A mutex value for thread-safe program logging.
         */
        std::mutex log_mutex;
        /**
         * @var std::ofstream fd;
         * @brief Descriptor of the log file.
         */
        std::ofstream fd;
        /**
         * @var std::ostream & out;
         * @brief Current descriptor of the output engine.
         */
        std::ostream& out = std::cerr;
        /**
         * @var std::string logFileName;
         * @brief Current name and path of the log file.
         *
         * @note Default: "../log/program.log".
         */
        std::string logFileName = "../log/program.log";

    protected:
        /**
         * @fn Logging::Logging (std::ostream &);
         * @brief Protect constructor.
         */
        Logging(void) noexcept;

        /**
         * @fn Logging::~Logging(void);
         * @brief Protect destructor.
         */
        ~Logging(void) noexcept;

        /**
         * @fn static inline void __output_values (std::ostream &) noexcept;
         * @brief Common method that outputs the data to log file.
         */
        inline void __output_values(void) const noexcept
        {
            out << std::endl;
        }

        /**
         * @fn template<typename T> static inline void __output_values (const T &) noexcept;
         * @brief Common method that outputs the data to log file.
         * @tparam [in] value - The current output parameter.
         */
        template<typename T>
        void __output_values (const T& value) const noexcept
        {
            out << value << std::endl;
        }

        /**
         * @fn template<typename T, typename... Args> void __output_values (const T &, Args &&...) noexcept;
         * @brief Common method that outputs the data to log file.
         * @tparam [in] value - The current output parameter.
         * @tparam [in] params - Any data for logging.
         */
        template<typename T, typename... Args>
        void __output_values (const T& value, Args&&... args) const noexcept
        {
            out << value;
            __output_values(std::forward<Args>(args)...);
        }

    public:
        Logging (Logging &&) = delete;
        Logging (const Logging &) = delete;
        Logging & operator= (Logging &&) = delete;
        Logging & operator= (const Logging &) = delete;

        /**
         * @fn static Logging & Logging::Instance(void);
         * @brief Method that returns the instance of the program logging singleton class.
         * @return The instance of singleton class.
         */
        static Logging & Instance(void) noexcept;

        /**
         * @fn template <typename... Args> void Logging::Push (LEVEL level, Args&&... args) noexcept;
         * @brief Method that pushes any logging data to log file.
         * @param [in] level - The level of the message.
         * @tparam [in] args - Any logging data.
         */
        template <typename... Args>
        void Push (LEVEL level, Args&&... args) noexcept
        {
#if ( !defined(DEBUG) )
            if (level == LEVEL::TRACE) { return; }
#endif
#if ( !defined(FULLLOG) )
            if (level == LEVEL::INFORMATION) { return; }
#endif
            try { std::lock_guard<std::mutex> lock(log_mutex); }
            catch (const std::system_error& /*err*/) {
                out << '[' << common::clockToString(std::chrono::system_clock::now()) << "]  ---  ";
                __output_values("[error] Logging.Push: In function 'lock_guard' - could not lock a mutex.");
                return;
            }

            out << '[' << common::clockToString(std::chrono::system_clock::now()) << "]  ---  ";
            switch (level)
            {
                case LEVEL::TRACE:
                    out << "[trace] ";
                    break;
                case LEVEL::INFORMATION:
                    out << "[info] ";
                    break;
                case LEVEL::WARNING:
                    out << "[warning] ";
                    break;
                case LEVEL::ERROR:
                    out << "[error] ";
                    break;
                case LEVEL::FATAL:
                    out << "[fatal] ";
                    break;
            }
            __output_values(std::forward<Args>(args)...);
        }

        /**
         *
         */
        //void SetLogFileSizeLimit (uint32_t /*size*/) noexcept;

        /**
         * @fn bool ChangeLogFileName (const std::string &, std::ios_base::openmode) noexcept;
         * @brief Method that switches the output log file.
         * @param [in] path - Full path to new log file.
         * @param [in] mode - Mode for open the log file. Default: Append (std::ios_base::ate).
         * @return True - if log file change is successful, otherwise - false.
         *
         * @note If the current engine on console mode then only the file name will be changed but the engine stay on the console mode.
         */
        bool ChangeLogFileName (const std::string & /*path*/, std::ios_base::openmode /*mode*/ = std::ios_base::ate) noexcept;

        /**
         * @fn bool SwitchLoggingEngine(void) noexcept;
         * @brief Method that switches the output engine.
         * @return True - if engine change is successful, otherwise - false.
         */
        bool SwitchLoggingEngine(void) noexcept;
    };


    /**
     * @fn void DbgHexDump (const char *, const void *, std::size_t, std::size_t);
     * @brief Function that outputs the data to log file in hex dump format.
     * @param [in] message - Message in log string.
     * @param [in] data - Data that must be displayed in hex format.
     * @param [in] size - The size of input data.
     * @param [in] hexLineLength - The length of one hex dump string. Default: 16.
     */
    void DbgHexDump (const char * /*message*/, const void * /*data*/, std::size_t /*size*/, std::size_t /*hexLineLength*/ = 16);

}  // namespace log.

#endif  // PROTOCOL_ANALYZER_LOG_HPP
