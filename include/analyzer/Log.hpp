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
#define LOG_TRACE(args...) (analyzer::log::Logger::Instance().Push(analyzer::log::LEVEL::TRACE, args))
/**
 * @def LOG_INFO (args...)
 * @brief Marco that output message to the log file with INFO level attribute.
 * @tparam [in] args - The sequence of parameters for output to the log file.
 */
#define LOG_INFO(args...) (analyzer::log::Logger::Instance().Push(analyzer::log::LEVEL::INFORMATION, args))
/**
 * @def LOG_WARNING (args...)
 * @brief Marco that output message to the log file with WARNING level attribute.
 * @tparam [in] args - The sequence of parameters for output to the log file.
 */
#define LOG_WARNING(args...) (analyzer::log::Logger::Instance().Push(analyzer::log::LEVEL::WARNING, args))
/**
 * @def LOG_ERROR (args...)
 * @brief Marco that output message to the log file with ERROR level attribute.
 * @tparam [in] args - The sequence of parameters for output to the log file.
 */
#define LOG_ERROR(args...) (analyzer::log::Logger::Instance().Push(analyzer::log::LEVEL::ERROR, args))
/**
 * @def LOG_FATAL (args...)
 * @brief Marco that output message to the log file with FATAL level attribute.
 * @tparam [in] args - The sequence of parameters for output to the log file.
 */
#define LOG_FATAL(args...) (analyzer::log::Logger::Instance().Push(analyzer::log::LEVEL::FATAL, args))

/**@}*/


namespace analyzer::log
{
    /**
     * @enum LEVEL
     * @brief The level of logging.
     *
     * @note The level TRACE work only in DEBUG mode.
     * The level INFORMATION work only in FULLLOG mode.
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
     * @note This singleton class is thread-safe.
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
     * @class Logger Log.hpp "include/analyzer/Log.hpp"
     * @brief This singleton class defined the interface for program logging.
     *
     * @note This singleton class is thread-safe and fault-tolerant.
     * To use this class, use following macro definitions: LOG_TRACE, LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_FATAL.
     *
     * @todo Support setting directory for log files.
     * @todo Support setting extension for log files.
     * @todo Implement function SetLogLevel(const uint16_t) for change log level in runtime.
     * @todo Change std::ofstream to std::filesystem.
     * @todo Implement user prefix for volume.
     * @todo Implement output error and fatal messages to another files (std::cerr, error log file).
     */
    class Logger
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
         *
         * @note Default: Standard output (std::cout).
         */
        std::ostream& out = std::cout;
        /**
         * @var std::string logFileName;
         * @brief Current name and path of the log file.
         *
         * @note Default: "../log/program.log".
         */
        std::string logFileName = "../log/program_volume1.log";
        /**
         * @var std::size_t recordsLimit;
         * @brief Number of entries in log file.
         *
         * @note Default: 50000.
         */
        std::size_t recordsLimit = 50000;
        /**
         * @var std::size_t recordsLimit;
         * @brief Number of entries in log file in current time.
         */
        std::size_t currentRecords = 0;

    protected:
        /**
         * @fn Logger::Logger (std::ostream &);
         * @brief Protect constructor.
         */
        Logger(void) noexcept;

        /**
         * @fn Logger::~Logger(void);
         * @brief Protect destructor.
         */
        ~Logger(void) noexcept;

        /**
         * @fn bool Logger::CheckVolume (std::string &) noexcept;
         * @brief Check and add volume to the log file name.
         * @param [in,out] name - File name.
         * @param [in] onlyCheck - Flag indicating whether to add a volume. Default: false.
         * @return True - if log file name already has volume name or if volume is added, otherwise - false.
         */
        bool CheckVolume (std::string & /*name*/, bool /*onlyCheck*/ = false) noexcept;

        /**
         * @fn bool Logger::GetNameWithNextVolume (std::string &) noexcept;
         * @brief Change volume for the log file name.
         * @param [in,out] name - File name.
         * @return True - if log file volume is changed successfully, otherwise - false.
         */
        bool GetNameWithNextVolume (std::string & /*name*/) noexcept;

        /**
         * @fn bool Logger::ChangeVolume(void) noexcept;
         * @brief Change volume the log file.
         * @return True - if log file name is changed successfully, otherwise - false.
         */
        bool ChangeVolume(void) noexcept;

        /**
         * @fn static inline void Logger::__output_values (std::ostream &) noexcept;
         * @brief Common method that outputs the data to log file.
         */
        inline void __output_values(void) const noexcept
        {
            out << std::endl;
        }

        /**
         * @fn template<typename T> static inline void Logger::__output_values (const T &) noexcept;
         * @brief Common method that outputs the data to log file.
         * @tparam [in] value - The current output parameter.
         */
        template<typename T>
        void __output_values (const T& value) const noexcept
        {
            out << value << std::endl;
        }

        /**
         * @fn template<typename T, typename... Args> void Logger::__output_values (const T &, Args &&...) noexcept;
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
        Logger (Logger &&) = delete;
        Logger (const Logger &) = delete;
        Logger & operator= (Logger &&) = delete;
        Logger & operator= (const Logger &) = delete;

        /**
         * @fn static Logging & Logger::Instance(void);
         * @brief Method that returns the instance of the program logging singleton class.
         * @return The instance of singleton class.
         */
        static Logger & Instance(void) noexcept;

        /**
         * @fn template <typename... Args> void Logger::Push (LEVEL level, Args&&... args) noexcept;
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
            catch (const std::system_error& err) {
                out << '[' << common::clockToString(std::chrono::system_clock::now()) << "]  ---  ";
                __output_values("[error] Logger.Push: In function 'lock_guard' - ", err.what(), '.');
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
            if (fd.is_open() == true && ++currentRecords >= recordsLimit) {
                ChangeVolume();
            }
        }

        /**
         * @fn bool Logger::SetLogFileRecordsLimit (std::size_t) noexcept;
         * @brief Method that controls number of entries in log file.
         * @param size - Number of entries.
         * @return True - if log file records limit is changed successfully, otherwise - false.
         *
         * @note If number of records in log file more then limit then logging engine switch log file to the next volume.
         */
        bool SetLogFileRecordsLimit (std::size_t /*size*/) noexcept;

        /**
         * @fn bool Logger::ChangeLogFileName (std::string, std::ios_base::openmode) noexcept;
         * @brief Method that switches the output log file.
         * @param [in] path - Full path to new log file.
         * @return True - if log file name is changed successfully, otherwise - false.
         *
         * @note If the current engine on console mode then only the file name will be changed but the engine stay on the console mode.
         */
        bool ChangeLogFileName (std::string /*path*/) noexcept;

        /**
         * @fn bool Logger::SwitchLoggingEngine(void) noexcept;
         * @brief Method that switches the output engine.
         * @return True - if engine is switched successfully, otherwise - false.
         */
        bool SwitchLoggingEngine(void) noexcept;

        /**
         * @fn inline std::string GetLogFileName(void) const noexcept;
         * @brief Method that returns current log file name.
         * @return Current log file name.
         */
        inline std::string GetLogFileName(void) const noexcept { return logFileName; }
    };


    /**
     * @fn void DbgHexDump (const char *, const void *, std::size_t, std::size_t);
     * @brief Function that outputs the data to log file in hex dump format.
     * @param [in] message - Message in log string.
     * @param [in] data - Data that must be displayed in hex format.
     * @param [in] size - The size of input data.
     * @param [in] hexLineLength - The length of one hex dump string. Default: 16.
     *
     * @todo Add three flag: is_offset, is_data, is_upper...
     * @todo Add dependency length of the offset from the length of data.
     */
    void DbgHexDump (const char * /*message*/, const void * /*data*/, std::size_t /*size*/, std::size_t /*hexLineLength*/ = 16);


}  // namespace log.

#endif  // PROTOCOL_ANALYZER_LOG_HPP
