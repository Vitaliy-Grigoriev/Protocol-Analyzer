#include <map>
#include <ctime>
#include <vector>
#include <memory>
#include <cstdint>
#include <pthread.h>
#include <exception>

#include "../include/Socket.hpp"

#define TCPv4 1
#define TCPv6 2
#define UDPv4 3
#define UDPv6 4
#define DEFAULT_BUFFER_SIZE 1048576 // 1 MB.


#ifndef HTTP2_ANALYZER_SOCKETMANAGER_H
#define HTTP2_ANALYZER_SOCKETMANAGER_H

namespace analyzer {
    namespace net {

        struct socket_init {
            bool flagSuccess = false;
            // Dummy.
            bool nonUsed1 = false;
            uint16_t port = DEFAULT_PORT;
            int32_t nonUsed2 = 0;
            int32_t family;
            int32_t type;
            int32_t protocol;
            uint32_t timeout = DEFAULT_TIMEOUT;
            char * send_buffer;
            std::unique_ptr<char[]> recv_buffer = nullptr;
            size_t send_length;
            size_t recv_length = 0;
            std::mutex work_t = { };
            std::string host = "";
            std::unique_ptr<Socket> sock;
        };

        typedef std::pair<pthread_t, socket_init * > pair_t;
        typedef std::pair<std::unique_ptr<char[]>, size_t> data_t;

        class NonBlockSocketManager {
        private:
            // Dummy.
            int32_t nonUsed = 0;
            // The timeout of connection.
            uint32_t timeout;
            // The set of external host.
            std::vector<pair_t> hosts;

            NonBlockSocketManager (const NonBlockSocketManager &) = delete;
            void operator= (const NonBlockSocketManager &) = delete;

            // Function, which work in thread.
            static void * thread_worker (void * /*arg*/);
            // Function, which allocate dinamic memory.
            static std::unique_ptr<char[]> alloc_memory (const size_t /*size*/) noexcept;
            // Function, which find Socket descriptor.
            size_t FindFd (const pthread_t /*fd*/) const;

        public:
            static const size_t fpos = SIZE_MAX;
            // Constructor.
            NonBlockSocketManager();

            // Adding a new connection to the pool.
            pthread_t Add (const char *   /*host*/,
                           char *         /*buffer*/,
                           const size_t   /*length*/,
                           const uint16_t /*port*/     = DEFAULT_PORT,
                           const uint16_t /*protocol*/ = TCPv4);


            // Wait thread by descriptor.
            void Wait (const pthread_t /*fd*/) const;
            // Wait all threads.
            void WaitAll () const;

            data_t GetData (const pthread_t /*fd*/) const;

            // Set connection timeout.
            void SetTimeout (const uint32_t /*time*/);

            // Destructor.
            ~NonBlockSocketManager();
        };

    } // namespace net.
} // namespace analyzer.


#endif //HTTP2_ANALYZER_SOCKETMANAGER_H
