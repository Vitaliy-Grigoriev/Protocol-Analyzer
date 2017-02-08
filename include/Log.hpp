#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>


#pragma once
#ifndef HTTP2_ANALYZER_LOG_HPP
#define HTTP2_ANALYZER_LOG_HPP


#define GET_ERROR(error) (*log::StrSysError::Instance())(error)


namespace analyzer {
    namespace log {

        extern "C" std::mutex log_mutex;

        class StrSysError
        {
        private:
            static std::mutex error_mutex;
            static StrSysError * volatile pInstance;

            StrSysError (StrSysError &&) = delete;
            StrSysError (const StrSysError &) = delete;
            StrSysError & operator= (StrSysError &&) = delete;
            StrSysError & operator= (const StrSysError &) = delete;

        protected:
            StrSysError() = default;
            ~StrSysError() { delete pInstance; }

        public:
            static StrSysError * Instance();
            std::string operator() (const int32_t /*error*/) noexcept;
        };


        std::string __get_time_string() noexcept;

        static inline void __output_values (std::ostream& fd) {
            fd << std::endl;
            fd.flush();
        }

        template<typename T>
        void __output_values (std::ostream& fd, const T& value) {
            fd << value;
            __output_values(fd);
        }

        template<typename T, typename... Args>
        void __output_values (std::ostream& fd, const T& value, Args&&... param) {
            fd << value;
            __output_values(fd, param...);
        }

        template <typename... Args>
        void __common_log (const char* message, Args&&... param)
        {
            std::lock_guard<std::mutex> lock(log_mutex);
            std::ofstream fd("../log/prog.log", std::ios::app);
            if (fd.is_open()) {
                fd << '[' << __get_time_string().c_str() << "]  ---  " << message;
                __output_values(fd, param...);
                fd.close();
            }
        }

        template <typename... Args>
        void DbgLog (const char* message, Args&&... param)
        {
#ifdef DEBUG
            __common_log(message, param...);
#endif
        }

        template <typename... Args>
        void SysLog (const char* message, Args&&... param)
        {
            __common_log(message, param...);
        }


        // Output in log file the data in hex dump form with byte shift.
        void DbgHexDump (const char * /*message*/, void * /*data*/, std::size_t /*size*/, std::size_t /*line_length*/ = 16);

    }  // namespace log.
}  // namespace analyzer.

#endif  // HTTP2_ANALYZER_LOG_HPP
