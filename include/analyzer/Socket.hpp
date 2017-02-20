#include <list>
#include <cerrno>
#include <string>
#include <cstring>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include "Log.hpp"


#pragma once
#ifndef HTTP2_ANALYZER_SOCKET_HPP
#define HTTP2_ANALYZER_SOCKET_HPP


#define NUMBER_OF_CTX    3
#define SSL_METHOD_TLS1  0
#define SSL_METHOD_TLS11 1
#define SSL_METHOD_TLS12 2
#define SSL_METHOD_TLS13 3

#define SOCKET_ERROR   (-1)
#define INVALID_SOCKET (-1)

#define DEFAULT_PORT     80
#define DEFAULT_PORT_SSL 443

#define DEFAULT_TIMEOUT      5    // sec.
#define DEFAULT_TIMEOUT_SSL  7    // sec.
#define DEFAULT_TIME_SIGWAIT (-1) // msec.

enum HTTP_VERSION : uint16_t { ERROR = 0, HTTP1_1 = 1, HTTP2_0 = 2 };


namespace analyzer {
    namespace net {
        using std::chrono::system_clock;

        class Socket {
        private:
            // Is the Socket in open state.
            bool isConnectionAlive = false;
            // Is an error occurred in the socket.
            bool isErrorOccurred = false;
            // Dummy 1.
            uint16_t nonUsed1 = 0;
            // The Socket family.
            int32_t socketFamily;
            // The Socket type (TCP/UDP).
            int32_t socketType;
            // The Socket protocol.
            int32_t ipProtocol;
            // Connection timeout.
            std::chrono::seconds timeout;
            // The Epoll descriptor.
            int32_t epfd = INVALID_SOCKET;
            // The Epoll events.
            struct epoll_event event = { };
            // Dummy 2.
            uint32_t nonUsed2 = 0;


            Socket (Socket &&) = delete;
            Socket (const Socket &) = delete;
            Socket & operator= (Socket &&) = delete;
            Socket & operator= (const Socket &) = delete;
            // Set Socket to Non-Blocking state.
            bool SetSocketToNonBlock();


        protected:
            // The Socket descriptor.
            int32_t fd = INVALID_SOCKET;
            // Name or IPv4/IPv6 of external host.
            std::string exHost = "";

            // Checks availability socket on write.
            bool IsReadyForSend (const int32_t /*time*/ = DEFAULT_TIME_SIGWAIT);
            // Checks availability socket on read.
            bool IsReadyForRecv (const int32_t /*time*/ = DEFAULT_TIME_SIGWAIT);
            // Cleaning after error.
            void CloseAfterError();


        public:
            // Constructor.
            explicit Socket (const int32_t  /*family*/   = AF_INET,
                             const int32_t  /*type*/     = SOCK_STREAM,
                             const int32_t  /*protocol*/ = IPPROTO_TCP,
                             const uint32_t /*timeout*/  = DEFAULT_TIMEOUT);

            // Return Socket descriptor.
            inline int32_t GetFd() const { return fd; }
            // Return Timeout of connection.
            inline std::chrono::seconds GetTimeout() const { return timeout; }
            // Return Socket error state.
            inline bool IsError() const { return isErrorOccurred; }
            // Return Socket connection state.
            inline bool IsAlive() const { return isConnectionAlive; }
            // Checks availability socket on read/write.
            uint16_t CheckSocketState (const int32_t /*time*/ = DEFAULT_TIME_SIGWAIT);

            // Connecting to external host.
            virtual void Connect (const char * /*host*/, const uint16_t /*port*/ = DEFAULT_PORT);
            // Sending the message to external host over TCP.
            virtual int32_t Send (char * /*data*/, std::size_t /*length*/);
            // Receiving the message from external host over TCP.
            virtual int32_t Recv (char * /*data*/, std::size_t /*length*/, const bool /*noWait*/ = false);
            // Receiving the message from external host until reach the end over TCP.
            // 1. The socket is no data to read.
            // 2. Has expired connection timeout.
            // 3. The lack of space in the buffer for reading.
            // 4. The socket has not been informed for reading signal.
            virtual int32_t RecvToEnd (char * /*data*/, std::size_t /*length*/);

            // Shutdown the connection. (SHUT_RD, SHUT_WR, SHUT_RDWR).
            virtual void Shutdown (int32_t /*how*/ = SHUT_RDWR);
            // Close the connection.
            virtual void Close();
            // Destructor.
            virtual ~Socket();
        };



        class SSLContext {
        private:
            SSL_CTX * ctx[NUMBER_OF_CTX] = { };

        public:
            SSLContext() noexcept;
            SSL_CTX * Get (const std::size_t /*method*/) const noexcept;
            ~SSLContext() noexcept;
        };



        class SocketSSL : public Socket {
        private:
            // SSL object.
            SSL * ssl = nullptr;
            // I/O object.
            BIO * bio = nullptr;

            SocketSSL (SocketSSL &&) = delete;
            SocketSSL (const SocketSSL &) = delete;
            SocketSSL & operator= (SocketSSL &&) = delete;
            SocketSSL & operator= (const SocketSSL &) = delete;

            // Returns true when the handshake is complete.
            bool IsHandshakeReady() const;
            // Provide handshake between hosts.
            bool DoHandshakeSSL();
            // Cleaning after error.
            void CleaningAfterError();

        public:
            static SSLContext context;

            // Constructor.
            explicit SocketSSL (const uint16_t /*method*/  = SSL_METHOD_TLS12,
                                const char *   /*ciphers*/ = nullptr,
                                const uint32_t /*timeout*/ = DEFAULT_TIMEOUT_SSL);

            // Connecting to external host.
            void Connect (const char * /*host*/, const uint16_t /*port*/ = DEFAULT_PORT_SSL) override final;
            // Sending the message to external host.
            int32_t Send (char * /*data*/, std::size_t /*length*/) override final;
            // Receiving the message from external host.
            int32_t Recv (char * /*data*/, std::size_t /*length*/, const bool /*noWait*/ = false) override final;
            // Receiving the message from external host until reach the end.
            // 1. The socket is no data to read.
            // 2. Has expired connection timeout.
            // 3. The lack of space in the buffer for reading.
            // 4. The socket has not been informed for reading signal.
            int32_t RecvToEnd (char * /*data*/, std::size_t /*length*/) override final;

            // Get all available clients ciphers.
            std::list<std::string> GetCiphersList() const;
            // Use only security ciphers in connection.
            bool SetOnlySecureCiphers();
            // Use ALPN protocol to change the set of application protocols.
            bool SetHttpProtocols();
            bool SetHttp_1_1_OnlyProtocol();
            bool SetHttp_2_0_OnlyProtocol();
            // Get selected ALPN protocol by server.
            HTTP_VERSION GetSelectedHttpProtocols() const;
            // Get selected cipher name in ssl connection.
            std::string GetSelectedCipherName() const;
            // Shutdown the connection. (SHUT_RD, SHUT_WR, SHUT_RDWR).
            void Shutdown (int32_t /*how*/ = SHUT_RDWR) override final;
            // Close the connection.
            void Close() override final;
            // Destructor.
            ~SocketSSL();
        };

    }  // namespace net.
}  // namespace analyzer.

#endif  // HTTP2_ANALYZER_SOCKET_HPP
