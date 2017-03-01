#include <mutex>
#include <chrono>
#include <iomanip>
#include <cstdint>
#include <fstream>


#pragma once
#ifndef HTTP2_ANALYZER_LOG_HPP
#define HTTP2_ANALYZER_LOG_HPP

/**
 * @brief Marco that return a system error in string format.
 * @param [in] error - The error code number.
 * @return The string that contains a description of the error.
 */
#define GET_ERROR(error) (*log::StrSysError::Instance())(error)


namespace analyzer {
    /**
     * @namespace log
     * @brief The namespace that contain definition of logging and error classes.
     */
    namespace log {
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
        private:
            /**
             * @var static StrSysError * volatile pInstance;
             * @brief The main instance of singleton StrSysError class.
             */
            static StrSysError * volatile pInstance;

            StrSysError (StrSysError &&) = delete;
            StrSysError (const StrSysError &) = delete;
            StrSysError & operator= (StrSysError &&) = delete;
            StrSysError & operator= (const StrSysError &) = delete;

        protected:
            /**
             * @fn StrSysError::StrSysError(void);
             * @brief Protect constructor.
             */
            StrSysError(void);
            /**
             * @fn StrSysError::~StrSysError(void);
             * @brief Protect destructor.
             */
            ~StrSysError(void) { delete pInstance; }

        public:
            /**
             * @brief Function that return the instance of the system error singleton class.
             * @return The instance of singleton class.
             */
            static StrSysError * Instance(void);

            /**
             * @fn std::string StrSysError::operator() (const int32_t) noexcept;
             * @brief Operator that return a system error in string format.
             * @param [in] error - The error code number.
             * @return The string that contains a description of the error.
             */
            std::string operator() (const int32_t /*error*/) noexcept;
        };

        /**
         * @fn std::string __get_time_string(void) noexcept;
         * @brief Function that return a current time in string format.
         * @return The string that contains a current time.
         */
        std::string __get_time_string(void) noexcept;

        /**
         * @brief Common function that outputs the data to log file.
         * @param [in] fd - File descriptor for output newline.
         * @note DO NOT USE THIS FUNCTION!
         */
        static inline void __output_values (std::ostream& fd) {
            fd << std::endl;
            fd.flush();
        }

        /**
         * @brief Common function that outputs the data to log file.
         * @param [in] fd - File descriptor for output.
         * @param [in] value - The current output parameter.
         * @note DO NOT USE THIS FUNCTION!
         */
        template<typename T>
        void __output_values (std::ostream& fd, const T& value) {
            fd << value;
            __output_values(fd);
        }

        /**
         * @brief Common function that outputs the data to log file.
         * @param [in] fd - File descriptor for output.
         * @param [in] value - The current output parameter.
         * @param [in] params - Any data for logging.
         * @note DO NOT USE THIS FUNCTION!
         */
        template<typename T, typename... Args>
        void __output_values (std::ostream& fd, const T& value, Args&&... params) {
            fd << value;
            __output_values(fd, params...);
        }

        /**
         * @fn void __common_log (const char *, Args &&...);
         * @brief Common function that outputs the data to log file.
         * @param [in] message - The first message in log string.
         * @param [in] params - Any data for logging.
         * @note DO NOT USE THIS FUNCTION!
         */
        template <typename... Args>
        void __common_log (const char* message, Args&&... params)
        {
            std::lock_guard<std::mutex> lock(log_mutex);
            std::ofstream fd("../log/prog.log", std::ios::app);
            if (fd.is_open()) {
                fd << '[' << __get_time_string().c_str() << "]  ---  " << message;
                __output_values(fd, params...);
                fd.close();
            }
        }

        /**
         * @fn void DbgLog (const char *, Args &&...);
         * @brief Function that outputs the data to log file in debug mode.
         * @param [in] message - The first message in log string.
         * @param [in] params - Any data for logging.
         */
        template <typename... Args>
        void DbgLog (const char* message, Args&&... params)
        {
#ifdef DEBUG
            __common_log(message, params...);
#endif
        }

        /**
         * @fn void SysLog (const char *, Args &&...);
         * @brief Function that outputs the data to log file in release mode.
         * @param [in] message - The first message in log string.
         * @param [in] params - Any data for logging.
         */
        template <typename... Args>
        void SysLog (const char* message, Args&&... params)
        {
            __common_log(message, params...);
        }

        /**
         * @fn void DbgHexDump (const char *, void *, std::size_t, std::size_t = 16);
         * @brief Function that outputs the data to log file in hex dump format.
         * @param message - The first message in log string.
         * @param data - Data data that must be displayed.
         * @param size - The size of this data.
         * @param line_length - The length of one hex dump string (default length is 16).
         */
        void DbgHexDump (const char * /*message*/, void * /*data*/, std::size_t /*size*/, std::size_t /*line_length*/ = 16);

    }  // namespace log.
}  // namespace analyzer.

#endif  // HTTP2_ANALYZER_LOG_HPP
