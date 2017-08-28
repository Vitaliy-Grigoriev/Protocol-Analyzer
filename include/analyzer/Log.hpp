#pragma once
#ifndef PROTOCOL_ANALYZER_LOG_HPP
#define PROTOCOL_ANALYZER_LOG_HPP

#include <mutex>
#include <cstdint>
#include <fstream>
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
 * @note In this code is used GNU extension "Named Variadic Macros".
 *
 * @{
 */

/**
 * @def LOG_TRACE (args...)
 * @brief Marco that output message to the log file with TRACE level attribute.
 * @param [in] args - The sequence of parameters for output to the log file.
 */
#define LOG_TRACE(args...) (analyzer::log::__common_log(analyzer::log::LEVEL::TRACE, args))
/**
 * @def LOG_INFO (args...)
 * @brief Marco that output message to the log file with INFO level attribute.
 * @param [in] args - The sequence of parameters for output to the log file.
 */
#define LOG_INFO(args...) (analyzer::log::__common_log(analyzer::log::LEVEL::INFORMATION, args))
/**
 * @def LOG_WARNING (args...)
 * @brief Marco that output message to the log file with WARNING level attribute.
 * @param [in] args - The sequence of parameters for output to the log file.
 */
#define LOG_WARNING(args...) (analyzer::log::__common_log(analyzer::log::LEVEL::WARNING, args))
/**
 * @def LOG_ERROR (args...)
 * @brief Marco that output message to the log file with ERROR level attribute.
 * @param [in] args - The sequence of parameters for output to the log file.
 */
#define LOG_ERROR(args...) (analyzer::log::__common_log(analyzer::log::LEVEL::ERROR, args))
/**
 * @def LOG_FATAL (args...)
 * @brief Marco that output message to the log file with FATAL level attribute.
 * @param [in] args - The sequence of parameters for output to the log file.
 */
#define LOG_FATAL(args...) (analyzer::log::__common_log(analyzer::log::LEVEL::FATAL, args))

/**@}*/


namespace analyzer::log
{
    /**
     * @enum LEVEL
     * @brief The level of logging.
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
     * @var extern std::mutex log_mutex;
     * @brief A global mutex value for logging.
     */
    extern std::mutex log_mutex;

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
         * @brief Function that return the instance of the system error singleton class.
         * @return The instance of singleton class.
         */
        static StrSysError & Instance(void) noexcept;

        /**
         * @fn std::string StrSysError::operator() (int32_t) noexcept;
         * @brief Operator that return a system error in string format.
         * @param [in] error - The error code number.
         * @return The string that contains a description of the error.
         */
        std::string operator() (int32_t /*error*/) noexcept;
    };

    /**
     * @fn static inline void __output_values (std::ostream &) noexcept;
     * @brief Common function that outputs the data to log file.
     * @param [in] fd - File descriptor for output newline.
     * @note DO NOT USE THIS FUNCTION!
     */
    static inline void __output_values (std::ostream& fd) noexcept
    {
        fd << std::endl;
        fd.flush();
    }

    /**
     * @fn template<typename T> static inline void __output_values (std::ostream &, const T &) noexcept;
     * @brief Common function that outputs the data to log file.
     * @param [in] fd - File descriptor for output.
     * @param [in] value - The current output parameter.
     * @note DO NOT USE THIS FUNCTION!
     */
    template<typename T>
    void __output_values (std::ostream& fd, const T& value) noexcept
    {
        fd << value;
        __output_values(fd);
    }

    /**
     * @fn template<typename T, typename... Args> void __output_values (std::ostream &, const T &, Args &&...) noexcept;
     * @brief Common function that outputs the data to log file.
     * @param [in] fd - File descriptor for output.
     * @param [in] value - The current output parameter.
     * @param [in] params - Any data for logging.
     * @note DO NOT USE THIS FUNCTION!
     */
    template<typename T, typename... Args>
    void __output_values (std::ostream& fd, const T& value, Args&&... args) noexcept
    {
        fd << value;
        __output_values(fd, std::forward<Args>(args)...);
    }

    /**
     * @fn template <typename... Args> void __common_log (const char *, Args &&...);
     * @brief Common function that outputs the data to log file.
     * @param [in] params - Any data for logging.
     * @note DO NOT USE THIS FUNCTION!
     */
    template <typename... Args>
    void __common_log (LEVEL level, Args&&... args) noexcept
    {
#if ( !defined(DEBUG) && !defined(FULLLOG) )
        if (level == LEVEL::TRACE) { return; }
#endif
#if ( !defined(FULLLOG) )
        if (level == LEVEL::INFORMATION) { return; }
#endif

        try { std::lock_guard<std::mutex> lock(log_mutex); }
        catch (const std::system_error& /*err*/) {
            return;
        }

        std::ofstream fd("../log/program.log", std::ios::app);
        if (fd.is_open())
        {
            fd << '[' << common::clockToString(std::chrono::system_clock::now()) << "]  ---  ";
            switch (level)
            {
                case LEVEL::TRACE:
                    fd << "[trace] ";
                    break;
                case LEVEL::INFORMATION:
                    fd << "[info] ";
                    break;
                case LEVEL::WARNING:
                    fd << "[warning] ";
                    break;
                case LEVEL::ERROR:
                    fd << "[error] ";
                    break;
                case LEVEL::FATAL:
                    fd << "[fatal] ";
                    break;
            }
            __output_values(fd, std::forward<Args>(args)...);
            fd.close();
        }
    }


    /**
     * @fn void DbgHexDump (const char *, void *, std::size_t, std::size_t);
     * @brief Function that outputs the data to log file in hex dump format.
     * @param [in] message - The first message in log string.
     * @param [in] data - Data data that must be displayed.
     * @param [in] size - The size of this data.
     * @param [in] line_length - The length of one hex dump string. Default: 16.
     */
    void DbgHexDump (const char * /*message*/, void * /*data*/, std::size_t /*size*/, std::size_t /*line_length*/ = 16);

}  // namespace log.

#endif  // PROTOCOL_ANALYZER_LOG_HPP
