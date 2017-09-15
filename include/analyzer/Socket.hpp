#pragma once
#ifndef PROTOCOL_ANALYZER_SOCKET_HPP
#define PROTOCOL_ANALYZER_SOCKET_HPP

#include <list>
#include <atomic>
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

#include "Log.hpp"   // In this header file also defined "Common.hpp".
#include "Http.hpp"


#define TCPv4 1
#define TCPv6 2
#define UDPv4 3
#define UDPv6 4

#define DEFAULT_PORT     80
#define DEFAULT_PORT_TLS 443

#define NUMBER_OF_CTX    3
#define SSL_METHOD_TLS1  0  // 0x0301
#define SSL_METHOD_TLS11 1  // 0x0302
#define SSL_METHOD_TLS12 2  // 0x0303
//#define SSL_METHOD_TLS13 3  // 0x0304

#define SOCKET_ERROR   (-1)
#define INVALID_SOCKET (-1)
#define SOCKET_SUCCESS 0

#define DEFAULT_TIMEOUT      5    // sec.
#define DEFAULT_TIMEOUT_SSL  7    // sec.

#define DEFAULT_TIME_SIGWAIT (-1) // milli sec.
#define MAXIMUM_SOCKET_DESCRIPTORS 1024 // Maximum descriptors for epoll_wait().

#define DEFAULT_RECEIVE_CHUNK 250 // Chunk part for receive massage.


namespace analyzer::net
{
    using std::chrono::system_clock;
    /**
      * @typedef bool (*CompleteFunctor) (const char *, std::size_t) noexcept;
      * @brief The type of the complete functor in Socket.Recv method.
      */
    using CompleteFunctor = bool (*) (const char *, std::size_t) noexcept;


    /**
     * @class SocketStatePool Socket.hpp "include/analyzer/Socket.hpp"
     * @brief This singleton class defined the interface of checking the status of socket descriptors.
     * @note You must delete socket descriptor after you do not need it anymore.
     *
     * This singleton class is thread-safe.
     */
    class SocketStatePool
    {
    private:
        /**
         * @var int32_t epoll_fd;
         * @brief The Epoll descriptor.
         * @note Epoll is thread-safe.
         */
        int32_t epoll_fd = INVALID_SOCKET;

        /**
         * @var struct epoll_event * events;
         * @brief The Epoll event structures.
         * @note The size of this structures defined as MAXIMUM_SOCKET_DESCRIPTORS.
         */
        std::unique_ptr<struct epoll_event[]> events = nullptr;

        /**
         * @var std::atomic<uint16_t> countOfDescriptors;
         * @brief The number of all socket descriptors under observation.
         */
        std::atomic<uint16_t> countOfDescriptors = 0;

    protected:
        /**
         * @fn SocketStatePool(void);
         * @brief Protect constructor.
         */
        SocketStatePool(void) noexcept;

        /**
         * @fn ~SocketStatePool(void) = default;
         * @brief Protect default destructor.
         */
        ~SocketStatePool(void) = default;

    public:
        SocketStatePool (SocketStatePool &&) = delete;
        SocketStatePool (const SocketStatePool &) = delete;
        SocketStatePool & operator= (SocketStatePool &&) = delete;
        SocketStatePool & operator= (const SocketStatePool &) = delete;

        /**
         * @fn static SocketsStatePool & Instance(void);
         * @brief Function that return the instance of the singleton class.
         * @return The instance of singleton class.
         */
        static SocketStatePool & Instance(void) noexcept;

        /**
         * @fn bool AddSocketDescriptor (int32_t, uint32_t);
         * @brief Function that adding new socket descriptor to epoll set.
         * @param [in] fd - Socket descriptor.
         * @param [in] events - One or more EPOLL_EVENTS values.
         * @return Boolean value that indicate the adding status.
         */
        //bool AddSocketDescriptor (int32_t /*fd*/, uint32_t /*events*/);

        /**
         * @fn bool DeleteSocketDescriptor (int32_t);
         * @brief Function that remove socket descriptor from epoll set.
         * @param [in] fd - Socket descriptor.
         * @return Boolean value that indicate the removal status.
         */
        //bool DeleteSocketDescriptor (int32_t /*fd*/);

        /**
         * @fn uint16_t CheckSocketStatus (int32_t, int32_t);
         * @brief Function that check socket status.
         * @param [in] fd - Socket descriptor.
         * @param [in] timeout - Timeout for waiting descriptor status. Default: unlimited.
         * @return Boolean value that indicate the removal status.
         */
        //uint16_t CheckSocketStatus (int32_t /*fd*/, int32_t /*timeout*/ = DEFAULT_TIME_SIGWAIT);
    };


    class Socket
    {
    private:
        // The Socket family.
        int32_t socketFamily;
        // The Socket type (TCP/UDP).
        int32_t socketType;
        // The Socket protocol.
        int32_t ipProtocol;
        // Connection timeout.
        uint32_t timeout;
        // The Epoll descriptor.
        int32_t epfd = INVALID_SOCKET;
        // The Epoll events.
        struct epoll_event event = { };

        // Set Socket to Non-Blocking state.
        bool SetSocketToNonBlock(void);

    protected:
        // The Socket descriptor.
        int32_t fd = INVALID_SOCKET;
        // Name or IPv4/IPv6 of external host.
        std::string exHost;


        // Checks availability socket on write.
        bool IsReadyForSend (int32_t /*time*/ = DEFAULT_TIME_SIGWAIT);
        // Checks availability socket on read.
        bool IsReadyForRecv (int32_t /*time*/ = DEFAULT_TIME_SIGWAIT);
        // Checks availability socket on read/write.
        uint16_t CheckSocketState (int32_t /*time*/ = DEFAULT_TIME_SIGWAIT) const;
        // Close after error. Must not use virtual methods in constructors so it is not a virtual method.
        void CloseAfterError(void);


    public:
        Socket (Socket &&) = delete;
        Socket (const Socket &) = delete;
        Socket & operator= (Socket &&) = delete;
        Socket & operator= (const Socket &) = delete;


        // Constructor.
        explicit Socket (int32_t  /*family*/   = AF_INET,
                         int32_t  /*type*/     = SOCK_STREAM,
                         int32_t  /*protocol*/ = IPPROTO_TCP,
                         uint32_t /*timeout*/  = DEFAULT_TIMEOUT);


        // Return Socket descriptor.
        inline int32_t GetFd(void) const { return fd; }
        // Return Timeout of connection.
        inline std::chrono::seconds GetTimeout(void) const { return std::chrono::seconds(timeout); }
        // Return Socket connection state.
        inline bool IsAlive(void) const { return (CheckSocketState(3000) != 0); }


        // Associates a local address with a socket.
        bool Bind (uint16_t /*port*/);
        // Connecting to external host.
        virtual bool Connect (const char * /*host*/, uint16_t /*port*/ = DEFAULT_PORT);
        // Sending the message to external host over TCP.
        virtual int32_t Send (const char * /*data*/, std::size_t /*length*/);
        // Receiving the message from external host over TCP.
        virtual int32_t Recv (char * /*data*/, std::size_t /*length*/, bool /*noWait*/ = false);
        // Receiving the message from external host over TCP until the functor returns false value.
        virtual int32_t Recv (char * /*data*/, std::size_t /*length*/, CompleteFunctor /*functor*/, std::size_t /*chunkLength*/ = DEFAULT_RECEIVE_CHUNK);
        // Receiving the message from external host until reach the end over TCP.
        // 1. The socket is no data to read.
        // 2. Has expired connection timeout.
        // 3. The lack of space in the buffer for reading.
        // 4. The socket has not been informed for reading signal.
        virtual int32_t RecvToEnd (char * /*data*/, std::size_t /*length*/);

        // Shutdown the connection (SHUT_RD, SHUT_WR, SHUT_RDWR).
        virtual void Shutdown (int32_t /*how*/ = SHUT_RDWR) const;
        // Close the connection.
        virtual void Close(void);
        // Destructor.
        virtual ~Socket(void);
    };



    class SSLContext
    {
    private:
        SSL_CTX * ctx[NUMBER_OF_CTX] = { };

    public:
        SSLContext (SSLContext &&) = delete;
        SSLContext (const SSLContext &) = delete;
        SSLContext & operator= (SSLContext &&) = delete;
        SSLContext & operator= (const SSLContext &) = delete;


        SSLContext(void) noexcept;
        SSL_CTX * Get (std::size_t /*method*/) const noexcept;
        ~SSLContext(void) noexcept;
    };



    class SocketSSL : public Socket
    {
    private:
        // SSL object.
        SSL * ssl = nullptr;
        // I/O object.
        BIO * bio = nullptr;

        static SSLContext context;

        // Returns true when the handshake is complete.
        bool IsHandshakeReady(void) const;
        // Provide handshake between hosts.
        bool DoHandshakeSSL(void);
        // Close after error.
        void SSLCloseAfterError(void);

    public:
        SocketSSL (SocketSSL &&) = delete;
        SocketSSL (const SocketSSL &) = delete;
        SocketSSL & operator= (SocketSSL &&) = delete;
        SocketSSL & operator= (const SocketSSL &) = delete;


        // Constructor.
        explicit SocketSSL (uint16_t     /*method*/  = SSL_METHOD_TLS12,
                            const char * /*ciphers*/ = nullptr,
                            uint32_t     /*timeout*/ = DEFAULT_TIMEOUT_SSL);

        // Connecting to external host.
        bool Connect (const char * /*host*/, uint16_t /*port*/ = DEFAULT_PORT_TLS) final;
        // Sending the message to external host.
        int32_t Send (const char * /*data*/, std::size_t /*length*/) final;
        // Receiving the message from external host.
        int32_t Recv (char * /*data*/, std::size_t /*length*/, bool /*noWait*/ = false) final;
        // Receiving the message from external host until reach the end.
        // 1. The socket is no data to read.
        // 2. Has expired connection timeout.
        // 3. The lack of space in the buffer for reading.
        // 4. The socket has not been informed for reading signal.
        int32_t RecvToEnd (char * /*data*/, std::size_t /*length*/) final;

        // Return the SSL object.
        inline SSL * GetSSL(void) const { return ssl; }
        // Get all available clients ciphers.
        std::list<std::string> GetCiphersList(void) const;
        // Use only security ciphers in connection.
        bool SetOnlySecureCiphers(void);
        // Use ALPN protocol to change the set of application protocols.
        bool SetInternalProtocol (const unsigned char * /*proto*/, std::size_t /*length*/);
        bool SetHttpProtocols(void);
        bool SetHttp_1_1_OnlyProtocol(void);
        bool SetHttp_2_0_OnlyProtocol(void);
        // Get selected ALPN protocol by server in string type.
        std::string GetRawSelectedProtocol(void) const;
        // Get current timeout of the SSL session.
        std::size_t GetSessionTimeout(void) const;
        // Get selected ALPN protocol by server.
        protocols::http::HTTP_VERSION GetSelectedProtocol(void) const;
        // Get selected cipher name in ssl connection.
        std::string GetSelectedCipherName(void) const;
        // Shutdown the connection. (SHUT_RD, SHUT_WR, SHUT_RDWR).
        void Shutdown (int32_t /*how*/ = SHUT_RDWR) const final;
        // Close the connection.
        void Close(void) final;
        // Destructor.
        ~SocketSSL(void) final;
    };

}  // namespace net.

#endif  // PROTOCOL_ANALYZER_SOCKET_HPP
