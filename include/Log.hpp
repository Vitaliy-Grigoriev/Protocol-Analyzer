#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>

#ifndef HTTP2_ANALYZER_LOG_H
#define HTTP2_ANALYZER_LOG_H


namespace analyzer {
    namespace log {

        class Strerror {
        private:
            std::unordered_map<int32_t, std::string> errors;
            std::mutex error_mutex = { };

            void set_errors () noexcept;

        public:
            Strerror() noexcept;
            std::string operator() (const int32_t /*err*/) noexcept;
            ~Strerror() noexcept;
        };

        extern "C" std::mutex log_mutex;
        extern "C" Strerror get_strerror;


        inline std::string get_time () {
            using std::chrono::system_clock;
            time_t time = system_clock::to_time_t(system_clock::now());
            std::string curr_time = ctime(&time);
            return curr_time.erase(19, 6).erase(0, 11);
        }

        inline void output_values (std::ostream& fd) {
            fd << std::endl;
            fd.flush();
        }

        template <typename T>
        inline void output_values (std::ostream& fd, const T& value) {
            fd << value;
            output_values(fd);
        }

        template <typename T, typename... Args>
        inline void output_values (std::ostream& fd, const T& value, Args&&... param) {
            fd << value;
            output_values(fd, param...);
        }


        template <typename... Args>
        void CommonLog (std::ostream& fd, const char* message, Args&&... param)
        {
            log_mutex.lock();
            fd << '[' << get_time() << "]  ---  " << message;
            output_values(fd, param...);
            log_mutex.unlock();
        }

        template <typename... Args>
        void DbgLog (const char* message, Args&&... param)
        {
#ifdef DEBUG
            std::ofstream fd("../log/prog.log", std::ios::app);
            if (fd.is_open()) {
                CommonLog(fd, message, param...);
                fd.close();
            }
#endif
        }

        template <typename... Args>
        void SysLog (const char* message, Args&&... param)
        {
            std::ofstream fd("../log/prog.log", std::ios::app);
            if (fd.is_open()) {
                CommonLog(fd, message, param...);
                fd.close();
            }
        }

        void DbgHexDump (const char * /*message*/, void * /*data*/, size_t /*size*/, size_t /*line_length*/ = 16);

    }  // namespace log.
}  // namespace analyzer.


#endif  //HTTP2_ANALYZER_LOG_H
