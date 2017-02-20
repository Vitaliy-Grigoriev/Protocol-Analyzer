#include <map>
#include <ctime>
#include <vector>
#include <pthread.h>
#include <exception>

#include "Socket.hpp"


#pragma once
#ifndef HTTP2_ANALYZER_SOCKETMANAGER_HPP
#define HTTP2_ANALYZER_SOCKETMANAGER_HPP


#define TCPv4 1
#define TCPv6 2
#define UDPv4 3
#define UDPv6 4
#define DEFAULT_BUFFER_SIZE 1048576 // 1 MB.


namespace analyzer {
    namespace net {

        struct socket_init
        {
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
            std::size_t send_length;
            std::size_t recv_length = 0;
            std::mutex work_t = { };
            std::string host = "";
            std::unique_ptr<Socket> sock = nullptr;
        };

        struct data_t
        {
            std::unique_ptr<char[]> data = nullptr;
            std::size_t length = 0;

            data_t (char * /*in*/, const std::size_t /*size*/);
            data_t (std::unique_ptr<char[]> & /*in*/, const std::size_t /*size*/);
        };

        typedef std::pair<pthread_t, socket_init * > pair_t;


        class NonBlockSocketManager
        {
        private:
            // Dummy.
            int32_t nonUsed = 0;
            // The timeout of connection.
            uint32_t timeout;
            // The set of external host.
            std::vector<pair_t> hosts;

            NonBlockSocketManager (NonBlockSocketManager &&) = delete;
            NonBlockSocketManager (const NonBlockSocketManager &) = delete;
            NonBlockSocketManager & operator= (NonBlockSocketManager &&) = delete;
            NonBlockSocketManager & operator= (const NonBlockSocketManager &) = delete;

            // Function, which work in thread.
            static void * thread_worker (void * /*arg*/);
            // Function, which allocate dynamic memory.
            static std::unique_ptr<char[]> alloc_memory (const std::size_t /*size*/) noexcept;
            // Function, which find Socket descriptor.
            std::size_t FindFd (const pthread_t /*fd*/) const;

        public:
            static const std::size_t fpos = SIZE_MAX;
            // Constructor.
            NonBlockSocketManager();

            // Adding a new connection to the pool.
            pthread_t Add (const char *      /*host*/,
                           char *            /*buffer*/,
                           const std::size_t /*length*/,
                           const uint16_t    /*port*/     = DEFAULT_PORT,
                           const uint16_t    /*protocol*/ = TCPv4);


            // Wait thread by descriptor.
            void Wait (const pthread_t /*fd*/) const;
            // Wait all threads.
            void WaitAll() const;

            data_t GetData (const pthread_t /*fd*/) const;

            // Set connection timeout.
            void SetTimeout (const uint32_t /*time*/);

            // Destructor.
            ~NonBlockSocketManager();
        };

    } // namespace net.
} // namespace analyzer.


#endif  // HTTP2_ANALYZER_SOCKETMANAGER_HPP
