// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#ifndef PROTOCOL_ANALYZER_SOCKET_HPP
#define PROTOCOL_ANALYZER_SOCKET_HPP

#include <atomic>
#include <string>
#include <vector>
#include <netdb.h>
#include <sys/epoll.h>  // struct epoll_event, epoll_create1, epoll_ctl, epoll_wait, EPOLL_CTL_*, EPOLL*.

#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/opensslv.h>  // For using OPENSSL_VERSION_NUMBER define.

#include "Log.hpp"   // In this header file also defined "Common.hpp".
#include "Http.hpp"
#include "Notification.hpp"


#define TCPv4 1
#define TCPv6 2
#define UDPv4 3
#define UDPv6 4

#define DEFAULT_PORT       80
#define DEFAULT_PORT_TLS   443

#define NUMBER_OF_CTX      4
#define SSL_METHOD_TLS10   0  // 0x0301
#define SSL_METHOD_TLS11   1  // 0x0302
#define SSL_METHOD_TLS12   2  // 0x0303
#if (defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER >= 0x10101000L)
    #define NUMBER_OF_CTX      4
    #define SSL_METHOD_TLS13   3  // 0x0304
#else
    #define NUMBER_OF_CTX    3
#endif

#define SOCKET_ERROR     (-1)
#define INVALID_SOCKET   (-1)
#define SOCKET_SUCCESS   0

#define DEFAULT_TIMEOUT       5    // sec.
#define DEFAULT_TIMEOUT_SSL   7    // sec.

#define DEFAULT_TIME_SIGWAIT   (-1)  // milli sec.
#define MAXIMUM_SOCKET_DESCRIPTORS   1024  // Maximum descriptors for epoll_wait().
#define UNLIMITED_LIVE_TIME 0 // Unlimited live time for socket descriptor in SocketStatePool.

#define DEFAULT_NO_CHUNK   0  // Chunk part for receive massage.


namespace analyzer::framework::net
{
    /**
      * @typedef bool (*CompleteFunctor) (const char * const, std::size_t) noexcept;
      * @brief The type of the complete functor in Socket.Recv method.
      *
      * @note Functor MUST returns TRUE value to finish receiving data, otherwise - FALSE.
      */
    using CompleteFunctor = bool (*) (const char * const, std::size_t) noexcept;


    /**
     * @class SocketStatePool   Socket.hpp   "include/framework/Socket.hpp"
     * @brief This singleton class defined the interface of checking the status of socket descriptors.
     *
     * @note This singleton class is thread-safe.
     */
    class SocketStatePool
    {
    private:
        /**
         * @brief The Epoll descriptor.
         *
         * @note Epoll is thread-safe.
         */
        int32_t epoll_fd = INVALID_SOCKET;

        /**
         * @brief The Epoll event structures.
         *
         * @note The size of this structures defined as MAXIMUM_SOCKET_DESCRIPTORS.
         */
        std::unique_ptr<struct epoll_event[]> events = nullptr;
        /**
         * @brief The number of all socket descriptors under observation in current time.
         */
        std::atomic<uint16_t> countOfDescriptors = 0;

    protected:
        /**
         * @brief Protect constructor.
         */
        SocketStatePool(void) noexcept;

        /**
         * @brief Protect default destructor.
         */
        ~SocketStatePool(void) = default;

    public:
        SocketStatePool (SocketStatePool &&) = delete;
        SocketStatePool (const SocketStatePool &) = delete;
        SocketStatePool & operator= (SocketStatePool &&) = delete;
        SocketStatePool & operator= (const SocketStatePool &) = delete;

        /**
         * @enum SOCKET_STATUS
         * @brief The socket statuses in event notification.
         *
         * @note Definition of the epoll states:
         * @note EPOLLERR   - Error condition happened on the associated file descriptor.
         * @note EPOLLHUP   - Signals an unexpected close of the socket.
         * @note EPOLLRDHUP - Stream socket peer closed connection, or shut down writing half of connection (used only with EPOLLIN).
         * @note EPOLLIN    - Associated file is available for read operations.
         * @note EPOLLOUT   - Associated file is available for write operations.
         */
        enum SOCKET_STATUS : uint16_t
        {
            STATUS_ERROR = 0x1,     // Set if epoll returns event EPOLLERR.
            STATUS_UNKNOWN = 0x2,   // Set if epoll not returns any event for descriptor.
            STATUS_CLOSED = 0x4,    // Set if epoll returns event EPOLLHUP or EPOLLRDHUP.
            STATUS_WRCLOSED = 0x8,  // Set if epoll returns event EPOLLRDHUP|EPOLLIN.
            STATUS_READ = 0x10,     // Set if epoll returns event EPOLLIN.
            STATUS_WRITE = 0x20,    // Set if epoll returns event EPOLLOUT.
            STATUS_ANOTHER = 0x40,  // Set if epoll returns any other event.
            STATUS_DELETE = 0x80    // Set if socket descriptor is not registered or has been deleted.
        };

        /**
         * @enum SOCKET_TYPE
         * @brief The type of the socket descriptor.
         *
         * @note The following types can be used together:
         * @note 1) TEST_ALWAYS with TEST_IMMEDIATELY. In this case default TASK_TYPE will be used.
         */
        enum SOCKET_TYPE : uint16_t
        {
            TEST_ON_REQUEST = 0x1,       // Descriptor will be checked once on request without save result, then notify waiting thread.
            TEST_ONCE_AND_DELETE = 0x2,  // Descriptor will be checked once, then notify waiting thread and after that delete from common set.
            TEST_ALWAYS = 0x4,           // Descriptor will be checked in all event loop with last TASK_TYPE, then save last result without notification.
            TEST_IMMEDIATELY = 0x8,      // Descriptor will be checked once using the default TASK_TYPE without request and notification, then save result.
            // Options section.
            SAVE_LAST_RESULT = 0x10,     // Save last result in internal buffer.   Use only with TEST_ON_REQUEST.
            WITHOUT_NOTIFICATION = 0x20, // No any notification will be sent. Result will be saved.      Use only with TEST_ON_REQUEST.
            NOTIFY_ALWAYS = 0x40         // Notification will be sent always.      Use only with TEST_ALWAYS case.
        };

        /**
         * @enum TASK_TYPE
         * @brief The task type for observed socket descriptor.
         *
         * @note Task type determines in what cases the notification will be sent.
         * @warning If SOCKET_TYPE is WITHOUT_NOTIFICATION, then no any notification will be sent.
         *
         * @attention Notification is ALWAYS sent in the following cases: STATUS_ERROR, STATUS_CLOSED, STATUS_ANOTHER.
         * @attention Status STATUS_WRCLOSED may be sent instead STATUS_READ status if epoll event EPOLLRDHUP is set.
         * @attention Status STATUS_ANOTHER may be sent with any other requested statuses.
         */
        enum TASK_TYPE : uint16_t
        {
            TASK_TYPE_READ = 0x1,   // .
            TASK_TYPE_WRITE = 0x2,  // .
            TASK_TYPE_CLOSE = 0x4,  // .
            TASK_TYPE_ALL = 0x8     // Default task.
        };

        /**
         * @brief Method that returns the instance of the singleton class.
         *
         * @return The instance of singleton SocketStatePool class.
         */
        static SocketStatePool & Instance(void) noexcept;

        /**
         * @brief Method that adds new socket descriptor to common set.
         *
         * @param [in] fd - Socket descriptor.
         * @param [in] type - Type of socket descriptor. Default: TEST_ON_REQUEST.
         * @param [in] liveTime - Time (in seconds) during which the socket descriptor is registered. Default: UNLIMITED_LIVE_TIME.
         * @return Boolean value that indicates the adding status.
         */
        //bool RegisterSocket (int32_t /*fd*/, SOCKET_TYPE /*type*/ = TEST_ON_REQUEST, uint32_t /*liveTime*/ = UNLIMITED_LIVE_TIME) noexcept;

        /**
         * @brief Method that change the type of socket descriptor in common set.
         *
         * @param [in] fd - Socket descriptor.
         * @param [in] type - Type of socket descriptor.
         * @return Boolean value that indicates the changing status.
         *
         * @todo Need to implement adding TEST_ALWAYS with NOTIFY_ALWAYS option or other.
         */
        //bool ChangeSocketType (int32_t /*fd*/, SOCKET_TYPE /*type*/) noexcept;

        /**
         * @brief Method that checks socket status.
         *
         * @param [in] fd - Socket descriptor.
         * @return SOCKET_STATUS value that indicates the status of socket descriptor.
         *
         * @note Method may returns the STATUS_UNKNOWN status if no any recent result.
         * @note Method may returns the STATUS_DELETE status if socket descriptor is not registered or has been deleted.
         */
        //SOCKET_STATUS CheckSocketStatus (int32_t /*fd*/) noexcept;

        /**
         * @brief Method that checks socket status.
         *
         * @param [in] fd - Socket descriptor.
         * @return NotificationObserver pointer that offers an interface for waiting and obtaining the result.
         *
         * @note Method may returns nullptr if an error occurred.
         *
         * @warning Use this method only in following cases:
         * @warning 1) If CheckSocketStatus() method return STATUS_UNKNOWN status.
         * @warning 2) If used TEST_IMMEDIATELY socket type or option WITHOUT_NOTIFICATION is set.
         * @warning 3) If used NOTIFY_ALWAYS socket type option.
         * @warning 4) If ChangeSocketType() method is used.
         */
        //NotificationObserver<SOCKET_STATUS> * SetOrderNotification (int32_t /*fd*/) noexcept;

        /**
         * @brief Method that set request for socket descriptor.
         *
         * @param [in] fd - Socket descriptor.
         * @param [in] task - The task for current descriptor. Default: TASK_TYPE_ALL.
         * @return NotificationObserver pointer that offers an interface for waiting and obtaining the result.
         *
         * @warning Method may returns nullptr if an error occurred.
         *
         * @attention This method replaces previous request.
         * @attention MUST NOT use this method with TEST_ALWAYS socket type. First use the method ChangeSocketType().
         */
        //NotificationObserver<SOCKET_STATUS> * RegisterTask (int32_t /*fd*/, TASK_TYPE /*task*/ = TASK_TYPE_ALL) noexcept;

        /**
         * @brief Method that removes socket descriptor from epoll set.
         *
         * @param [in] fd - Socket descriptor.
         * @return Boolean value that indicates the removal status.
         *
         * @note If parameter 'liveTime' was used when socket descriptor was registered, then no need to call this method.
         * @note Socket class delete descriptor from pool in destructor.
         *
         * @attention Socket descriptor MUST be deleted if it is no longer needed.
         */
        //void DeleteDescriptor (int32_t /*fd*/) noexcept;
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
        bool SetSocketToNonBlock(void) noexcept;
        // Disable SIGPIPE signal for send.
        bool DisableSignalSIGPIPE(void) const noexcept;

    protected:
        // The Socket descriptor.
        int32_t fd = INVALID_SOCKET;
        // Name or IPv4/IPv6 address of external host.
        std::string exHost;


        // Checks availability socket on write.
        bool IsReadyForSend (int32_t /*time*/ = DEFAULT_TIME_SIGWAIT) noexcept;
        // Checks availability socket on read.
        bool IsReadyForRecv (int32_t /*time*/ = DEFAULT_TIME_SIGWAIT) noexcept;
        // Checks availability socket on read/write.
        uint16_t CheckSocketState (int32_t /*time*/ = DEFAULT_TIME_SIGWAIT) const noexcept;
        // Close after error. MUST NOT use virtual methods in constructors so it is not a virtual method.
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
                         uint32_t /*timeout*/  = DEFAULT_TIMEOUT) noexcept;


        // Return Socket descriptor.
        inline int32_t GetFd(void) const noexcept { return fd; }
        // Return Timeout of connection.
        inline std::chrono::seconds GetTimeout(void) const noexcept { return std::chrono::seconds(timeout); }
        // Return Socket connection state.
        inline bool IsAlive(void) const noexcept { return (CheckSocketState(3000) != 0); }


        // Associates a local address with a socket.
        bool Bind (uint16_t /*port*/);
        // Associates a local address with prepared sockaddr.
        bool Bind (const struct sockaddr * /*addr*/, uint32_t /*size*/);
        // Connecting to external host.
        virtual bool Connect (const char * /*host*/, uint16_t /*port*/ = DEFAULT_PORT);

        /**
         * @brief Method that sends a message to external host over TCP protocol.
         *
         * @param [in] data - Pointer to the constant data.
         * @param [in] length - Length of the sending data.
         * @return TRUE - if sending data is successful, otherwise - FALSE.
         */
        virtual bool Send (const char * /*data*/, std::size_t /*length*/) noexcept;

        /**
         * @brief Method that receives a message from external host over TCP protocol.
         *
         * @param [in] data - Pointer to the buffer for incoming data.
         * @param [in] length - Size of the buffer for incoming data.
         * @param [in] noWait - Boolean flag that indicates is it worth it to continue in a loop after the first reading.
         * @return Received data length or error code (less than zero).
         */
        virtual int32_t Recv (char * /*data*/, std::size_t /*length*/, bool /*noWait*/ = false);
        // Receiving the message from external host over TCP protocol until the functor returns false value.
        bool Recv (char * /*data*/, std::size_t /*length*/, std::size_t & /*obtainLength*/, CompleteFunctor /*functor*/, std::size_t /*chunkLength*/ = DEFAULT_NO_CHUNK);
        // Receiving the message from external host until reach the end over TCP protocol.
        // 1. The socket is no data to read.
        // 2. Has expired connection timeout.
        // 3. The lack of space in the buffer for reading.
        // 4. The socket has not been informed for reading signal.
        virtual int32_t RecvToEnd (char * /*data*/, std::size_t /*length*/) noexcept;

        /**
         * @brief Method that sends message to external host over UDP protocol.
         *
         * @param [in] host - Name or IPv4/IPv6 address of external host.
         * @param [in] port - Destination port number.
         * @param [in] data - Pointer to data for sending.
         * @param [in] length - Length of sending data.
         * @return TRUE - if sending data is successful, otherwise - FALSE.
         */
        virtual bool SendTo (const char * /*host*/, uint16_t /*port*/, const char * /*data*/, std::size_t /*length*/) noexcept;

        /**
         * @brief Method that receives message from external host over UDP protocol.
         *
         * @param [in] host - Name or IPv4/IPv6 address of external host.
         * @param [in] port - Destination port number.
         * @param [in] data - Pointer to data buffer for receiving.
         * @param [in] length - Length of data for receiving.
         * @return Number of received bytes or error code (error code is less than zero).
         */
        virtual int32_t RecvFrom (const char * /*host*/, uint16_t /*port*/, char * /*data*/, std::size_t /*length*/) noexcept;

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

        /**
         * @brief Method that allows SSL session resumption by session ID for selected SSL method.
         *
         * @param [in] method - TLS
         * @return TRUE - if SSL session resumption for selected SSL method is enabled, otherwise - FALSE.
         */
        bool AllowSessionResumption (uint16_t /*method*/) noexcept;
    };



    class SocketSSL : public Socket
    {
    private:
        // SSL object.
        SSL * ssl = nullptr;
        // I/O object.
        BIO * bio = nullptr;

    public:
        static SSLContext context;

    private:
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
                            uint32_t     /*timeout*/ = DEFAULT_TIMEOUT_SSL) noexcept;

        // Constructor with previously established session.
        explicit SocketSSL (SSL_SESSION * /*session*/,
                            uint16_t      /*method*/  = SSL_METHOD_TLS12,
                            const char *  /*ciphers*/ = nullptr,
                            uint32_t      /*timeout*/ = DEFAULT_TIMEOUT_SSL) noexcept;

        // Connecting to external host.
        bool Connect (const char * /*host*/, uint16_t /*port*/ = DEFAULT_PORT_TLS) final;
        // Sending the message to external host.
        bool Send (const char * /*data*/, std::size_t /*length*/) noexcept final;
        // Receiving the message from external host.
        int32_t Recv (char * /*data*/, std::size_t /*length*/, bool /*noWait*/ = false) final;
        // Receiving the message from external host until reach the end.
        // 1. The socket is no data to read.
        // 2. Has expired connection timeout.
        // 3. The lack of space in the buffer for reading.
        // 4. The socket has not been informed for reading signal.
        int32_t RecvToEnd (char * /*data*/, std::size_t /*length*/) noexcept final;

        // Return the SSL object.
        inline SSL * GetSSL(void) const noexcept { return ssl; }
        // Return the SSL object.
        inline SSL_SESSION * GetSessionSSL(void) const noexcept { return SSL_get_session(ssl); }
        // Set Server Name Indication to TLS extension.
        bool SetServerNameIndication (const std::string & /*serverName*/) const noexcept;
        // Get all available clients ciphers.
        std::vector<std::string> GetCiphersList(void) const noexcept;
        // Use only security ciphers in connection.
        bool SetOnlySecureCiphers(void) noexcept;

#if (defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER >= 0x1000208fL)  // If OPENSSL version more then 1.0.2h.
        // Use ALPN protocol TLS extension to change the set of application protocols.
        bool SetInternalProtocol (const unsigned char * /*proto*/, std::size_t /*length*/) noexcept;

        // Get selected ALPN protocol by server in string type.
        std::string GetRawSelectedProtocol(void) const noexcept;

        // Get selected ALPN protocol by server.
        protocols::http::HTTP_VERSION GetSelectedProtocol(void) const noexcept;

        bool SetHttpProtocols(void) noexcept;
        bool SetHttp_1_1_OnlyProtocol(void) noexcept;
        bool SetHttp_2_0_OnlyProtocol(void) noexcept;
#endif

        // Get current timeout of the SSL session.
        std::size_t GetSessionTimeout(void) const noexcept;
        // Get selected cipher name in ssl connection.
        std::string GetSelectedCipherName(void) const noexcept;
        // Shutdown the connection. (SHUT_RD, SHUT_WR, SHUT_RDWR).
        void Shutdown (int32_t /*how*/ = SHUT_RDWR) const final;
        // Close the connection.
        void Close(void) final;
        // Destructor.
        ~SocketSSL(void) final;
    };

}  // namespace net.

#endif  // PROTOCOL_ANALYZER_SOCKET_HPP
