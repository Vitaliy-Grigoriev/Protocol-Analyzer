// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_LOG_HPP
#define PROTOCOL_ANALYZER_LOG_HPP

#include <mutex>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <exception>

#include "Common.hpp"


/**
 * @def GET_ERROR (error);
 * @brief Marco that return a system error in string format.
 * @param [in] error - Error code number.
 * @return String that contains a description of the error.
 */
#define GET_ERROR(error) (analyzer::framework::log::StrSysError::Instance())(error)


/**
 * @defgroup LOG_FUNCTIONS Global logging defines.
 * @brief This group of macros defines all interfaces for logging with several attributes.
 *
 * @note In this code block is used GNU extension "Named Variadic Macros".
 *
 * @{
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvariadic-macros"

/**
 * @def LOG_TRACE (args...);
 * @brief Marco that outputs message to the logfile with TRACE level attribute.
 * @tparam [in] args - The sequence of parameters for output to the logfile.
 */
#define LOG_TRACE(args...) (analyzer::framework::log::Logger::Instance().Push(analyzer::framework::log::LEVEL::TRACE, args))
/**
 * @def LOG_INFO (args...);
 * @brief Marco that outputs message to the logfile with INFO level attribute.
 * @tparam [in] args - The sequence of parameters for output to the logfile.
 */
#define LOG_INFO(args...) (analyzer::framework::log::Logger::Instance().Push(analyzer::framework::log::LEVEL::INFORMATION, args))
/**
 * @def LOG_MAJOR (args...);
 * @brief Marco that outputs message to the logfile with MAJOR level attribute.
 * @tparam [in] args - The sequence of parameters for output to the logfile.
 */
#define LOG_MAJOR(args...) (analyzer::framework::log::Logger::Instance().Push(analyzer::framework::log::LEVEL::MAJOR, args))
/**
 * @def LOG_WARNING (args...);
 * @brief Marco that outputs message to the logfile with WARNING level attribute.
 * @tparam [in] args - The sequence of parameters for output to the logfile.
 */
#define LOG_WARNING(args...) (analyzer::framework::log::Logger::Instance().Push(analyzer::framework::log::LEVEL::WARNING, args))
/**
 * @def LOG_ERROR (args...);
 * @brief Marco that outputs message to the logfile with ERROR level attribute.
 * @tparam [in] args - The sequence of parameters for output to the logfile.
 */
#define LOG_ERROR(args...) (analyzer::framework::log::Logger::Instance().Push(analyzer::framework::log::LEVEL::ERROR, args))
/**
 * @def LOG_FATAL (args...);
 * @brief Marco that outputs message to the logfile with FATAL level attribute.
 * @tparam [in] args - The sequence of parameters for output to the logfile.
 */
#define LOG_FATAL(args...) (analyzer::framework::log::Logger::Instance().Push(analyzer::framework::log::LEVEL::FATAL, args))

#pragma GCC diagnostic pop

/**@}*/


namespace analyzer::framework::log
{
    /**
     * @enum LEVEL
     * @brief Level type of logging message.
     */
    enum LEVEL : uint16_t
    {
        FATAL = 1,
        ERROR = 2,
        WARNING = 3,
        MAJOR = 4,
        INFORMATION = 5,
        TRACE = 6
    };


    /**
     * @class StrSysError Log.hpp "include/framework/Log.hpp"
     * @brief This singleton class defined the interface of receipt an system error.
     *
     * @note This singleton class is thread-safe.
     * @note To use this class, use the macro GET_ERROR (int32_t).
     */
    class StrSysError
    {
    protected:
        /**
         * @fn StrSysError::StrSysError() noexcept;
         * @brief Protection constructor.
         */
        StrSysError(void) noexcept;

        /**
         * @fn StrSysError::~StrSysError();
         * @brief Protection default destructor.
         */
        ~StrSysError(void) = default;

    public:
        StrSysError (StrSysError &&) = delete;
        StrSysError (const StrSysError &) = delete;
        StrSysError & operator= (StrSysError &&) = delete;
        StrSysError & operator= (const StrSysError &) = delete;

        /**
         * @fn static StrSysError & StrSysError::Instance() noexcept;
         * @brief Method that returns the instance of the StrSysError singleton class.
         * @return The instance of singleton class.
         */
        static StrSysError & Instance(void) noexcept;

        /**
         * @fn std::string StrSysError::operator() (int32_t) const noexcept;
         * @brief Operator that returns a system error in string format.
         * @param [in] error - The error code number.
         * @return String that contains a description of the error.
         */
        std::string operator() (int32_t /*error*/) const noexcept;
    };



    /**
     * @class Logger Log.hpp "include/framework/Log.hpp"
     * @brief This singleton class defined the interface for program logging.
     *
     * @note This singleton class is thread-safe, exception-safe and fault-tolerant.
     * @note To use this class, use following macro definitions: LOG_TRACE, LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_FATAL.
     * @note Allow two types of logger engine: logfile-oriented and console-oriented.
     *
     * @todo Support custom directories for log files.
     * @todo Support setting for users extension for log files.
     * @todo Change std::ofstream to std::filesystem.
     * @todo Implement users prefix for volume.
     * @todo Implement output error and fatal messages to another files (std::cerr, error logfile).
     */
    class Logger
    {
    private:
        /**
         * @var std::mutex logMutex;
         * @brief A mutex value for thread-safe program logging.
         */
        std::mutex logMutex;
        /**
         * @var std::ofstream fd;
         * @brief Descriptor of the logfile.
         */
        std::ofstream fd;
        /**
         * @var mutable std::streambuf * defaultIO;
         * @brief Default IO buffer of std::ostream.
         */
        mutable std::streambuf * defaultIO = nullptr;
        /**
         * @var std::ostream & out;
         * @brief Current descriptor of the output engine.
         *
         * @note Default logging engine: Standard buffered log output (std::clog).
         */
        std::ostream & out = std::cout;
        /**
         * @var std::string logFileName;
         * @brief Current name and path of the logfile.
         *
         * @note Default path to logfile: "program_volume1.log".
         */
        std::string logFileName = "program_volume1.log";
        /**
         * @var volatile std::size_t recordsLimit;
         * @brief Number of entries in logfile.
         *
         * @note Default: 50000.
         */
        volatile std::size_t recordsLimit = 50000;
        /**
         * @var volatile std::size_t recordsLimit;
         * @brief Number of entries in logfile in current time.
         */
        volatile std::size_t currentRecords = 0;
        /**
         * @var volatile uint16_t levelType;
         * @brief The logging level type.
         *
         * @note If message has level that less then current level, then this message will be blocked.
         * @note Default: LEVEL::MAJOR.
         */
        volatile LEVEL levelType = LEVEL::MAJOR;
        /**
         * @var bool bufferedMode;
         * @brief Boolean value that indicates about the buffered/unbuffered mode.
         *
         * @note Default: buffered mode.
         */
        bool bufferedMode = true;

    protected:
        /**
         * @fn Logger::Logger() noexcept;
         * @brief Protection constructor.
         */
        Logger(void) noexcept;

        /**
         * @fn Logger::~Logger() noexcept;
         * @brief Protection destructor.
         */
        ~Logger(void) noexcept;

        /**
         * @fn static std::string Logger::GetTimePrefix() noexcept;
         * @brief Method that returns the logger time prefix in string format.
         * @return Logger time prefix in string format.
         *
         * @note Example: [0000-11-22  00:11:22.333]  ---  message.
         */
        static std::string GetTimePrefix(void) noexcept;

        /**
         * @fn bool Logger::CheckVolume (std::string &, bool) const noexcept;
         * @brief Check and add volume to the logfile name.
         * @param [in,out] name - Logfile name.
         * @param [in] onlyCheck - Flag indicating whether to add a volume. Default: false.
         * @return TRUE - if logfile name already has volume name or if volume is added, otherwise - FALSE.
         */
        bool CheckVolume (std::string & /*name*/, bool /*onlyCheck*/ = false) const noexcept;

        /**
         * @fn bool Logger::GetNameWithNextVolume (std::string &) const noexcept;
         * @brief Change volume for the logfile name.
         * @param [in,out] name - Logfile name.
         * @return TRUE - if logfile volume is changed successfully, otherwise - FALSE.
         */
        bool GetNameWithNextVolume (std::string & /*name*/) const noexcept;

        /**
         * @fn bool Logger::ChangeVolume() noexcept;
         * @brief Change volume the logfile.
         * @return TRUE - if logfile name is changed successfully, otherwise - FALSE.
         *
         * @note This method is strong exception safety.
         * @note This method change engine only if work logfile-oriented engine.
         * @note If work console-oriented engine, then change only name of logfile and number of entries in it.
         */
        bool ChangeVolume(void) noexcept;

        /**
         * @fn template <typename Type>
         * void Logger::CommonLogger (const Type &) const noexcept;
         * @brief Common method that outputs the data to logfile.
         * @tparam [in] value - The current output template parameter.
         */
        template <typename Type>
        void CommonLogger (const Type& value) const noexcept
        {
            try {
                // Without std::endl because it calls the flush() method.
                out << value << '\n';
                if (bufferedMode == false) {
                    out.flush();
                }
            }
            catch (const std::ios_base::failure& err) {
                out.rdbuf(defaultIO);
                out << GetTimePrefix();
                out << "[error] Logger.CommonLogger: Exception occurred when push new log data - " << err.what() << '.' << std::endl;
            }
        }

        /**
         * @fn template <typename Type, typename... Args>
         * void Logger::CommonLogger (const Type &, Args &&...) const noexcept;
         * @brief Common method that outputs the data to logfile.
         * @tparam [in] value - The current output template parameter.
         * @tparam [in] args - Any next data for logging.
         */
        template <typename Type, typename... Args>
        void CommonLogger (const Type& value, Args&&... args) const noexcept
        {
            try {
                out << value;
                CommonLogger(std::forward<Args>(args)...);
            }
            catch (const std::ios_base::failure& err) {
                out.rdbuf(defaultIO);
                out << GetTimePrefix();
                out << "[error] Logger.CommonLogger: Exception occurred when push new log data - " << err.what() << '.' << std::endl;
            }
        }

    public:
        Logger (Logger &&) = delete;
        Logger (const Logger &) = delete;
        Logger & operator= (Logger &&) = delete;
        Logger & operator= (const Logger &) = delete;

        /**
         * @fn static Logger & Logger::Instance() noexcept;
         * @brief Method that returns the instance of the program logging singleton class.
         * @return The instance of singleton Logger class.
         */
        static Logger & Instance(void) noexcept;

        /**
         * @fn template <typename... Args>
         * void Logger::Push (volatile LEVEL, Args &&...) noexcept;
         * @brief Method that pushes any logging data to logfile.
         * @param [in] level - The level of the message.
         * @tparam [in] args - Any logging data.
         */
        template <typename... Args>
        void Push (volatile LEVEL level, Args&&... args) noexcept
        {
            if (level > levelType || sizeof...(args) == 0) {
                return;
            }

            try { std::lock_guard<std::mutex> lock { logMutex }; }
            catch (const std::system_error& err) {
                CommonLogger(GetTimePrefix(), "[error] Logger.Push: In function 'lock_guard' - ", err.what(), '.');
                return;
            }

            try {
                out << GetTimePrefix();
                switch (level)
                {
                    case LEVEL::TRACE:
                        out << "[trace] ";
                        break;
                    case LEVEL::INFORMATION:
                        out << "[info] ";
                        break;
                    case LEVEL::MAJOR:
                        out << "[major] ";
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

                CommonLogger(std::forward<Args>(args)...);
                if (fd.is_open() == true && ++currentRecords >= recordsLimit) {
                    ChangeVolume();
                }
            }
            catch (const std::ios_base::failure& err)
            {
                out.rdbuf(defaultIO);
                if (fd.is_open() == true && fd.good() == true) { fd.close(); }
                CommonLogger(GetTimePrefix(), "[error] Logger.Push: Exception occurred when push new log data - ", err.what(), '.');
            }
        }

        /**
         * @fn bool Logger::SetLogFileRecordsLimit (std::size_t) noexcept;
         * @brief Method that controls number of entries in logfile.
         * @param size - Number of entries.
         * @return TRUE - if logfile records limit is changed successfully, otherwise - FALSE.
         *
         * @note If number of records in logfile more then limit, then logging engine switch logfile to the next volume.
         * @note If enable console-oriented engine, then only logfile name and size of current records in it will be changed.
         */
        bool SetLogFileRecordsLimit (std::size_t /*size*/) noexcept;

        /**
         * @fn bool Logger::ChangeLogFileName (std::string) noexcept;
         * @brief Method that switches the output logfile.
         * @param [in] path - Full path to new logfile.
         * @return TRUE - if logfile name is changed successfully, otherwise - FALSE.
         *
         * @note If enable console-oriented engine, then mode will be changed to logfile-oriented engine.
         * @attention It is not mistake when the path is passed by value.
         */
        bool ChangeLogFileName (std::string /*path*/) noexcept;

        /**
         * @fn bool Logger::SwitchLoggingEngine() noexcept;
         * @brief Method that switches the output engine.
         * @return TRUE - if engine is switched successfully, otherwise - FALSE.
         */
        bool SwitchLoggingEngine(void) noexcept;

        /**
         * @fn inline void Logger::SwitchBufferedMode() noexcept;
         * @brief Method that switches the buffered mode.
         *
         * @note Buffered mode set by default.
         */
        inline void SwitchBufferedMode(void) noexcept { bufferedMode = !bufferedMode; }

        /**
         * @fn void Logger::SetLogLevel (const volatile LEVEL) noexcept;
         * @brief Method that change log level type.
         */
        void SetLogLevel (const volatile LEVEL newLevel) noexcept { levelType = newLevel; }

        /**
         * @fn inline std::size_t Logger::GetLogFileRecordsSize() const noexcept;
         * @brief Method that returns current logfile records.
         * @return Current logfile records.
         */
        inline std::size_t GetLogFileRecordsSize(void) const noexcept { return currentRecords; }
    };


    /**
     * @fn void DbgHexDump (const char *, const void *, std::size_t, std::size_t);
     * @brief Function that outputs the data to logfile in hex dump format.
     * @param [in] message - Message in log string.
     * @param [in] data - Data for transfer to hex format.
     * @param [in] size - Size of input data.
     * @param [in] hexLineLength - Length of one hex dump string. Default: 16.
     *
     * @todo Add three flag: is_offset, is_data, is_upper...
     * @todo Add dependency length of the offset from the length of data.
     * @todo Improvement in case when using std::string_view.
     */
    void DbgHexDump (const char * /*message*/, const void * /*data*/, std::size_t /*size*/, std::size_t /*hexLineLength*/ = 16);


}  // namespace log.

#endif  // PROTOCOL_ANALYZER_LOG_HPP
