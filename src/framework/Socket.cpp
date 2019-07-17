// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include <fcntl.h>
#include <csignal>
#include <unistd.h>

#include "../../include/framework/System.hpp"
#include "../../include/framework/Socket.hpp"
#include "../../include/framework/GlobalInfo.hpp"  // storage::GlobalInfo.


namespace analyzer::framework::net
{
    // Constructor.
    Socket::Socket (const int32_t family, const int32_t type, const int32_t protocol, const uint32_t time) noexcept
        : socketFamily(family), socketType(type), ipProtocol(protocol), timeout(time)
    {
        LOG_TRACE("Socket.Socket: Creating socket...");
        fd = socket(socketFamily, socketType, ipProtocol);
        if (fd == INVALID_SOCKET) {
            LOG_ERROR("Socket.Socket: In function 'socket' - ", GET_ERROR(errno));
            return;
        }

        if (DisableSignalSIGPIPE() == false)
        {
            close(fd); fd = INVALID_SOCKET;
            return;
        }
        if (SetSocketToNonBlock() == false)
        {
            close(fd); fd = INVALID_SOCKET;
            return;
        }
        LOG_TRACE("Socket.Socket [", fd, "]: Socket was created.");

        epfd = epoll_create1(0);
        if (epfd == INVALID_SOCKET)
        {
            LOG_ERROR("Socket.Socket [", fd, "]: In function 'epoll_create1' - ", GET_ERROR(errno));
            close(fd); fd = INVALID_SOCKET;
            return;
        }

        event.data.fd = fd;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == SOCKET_ERROR)
        {
            LOG_ERROR("Socket.Socket [", fd, "]: In function 'epoll_ctl' - ", GET_ERROR(errno));
            close(fd); fd = INVALID_SOCKET;
            close(epfd); epfd = INVALID_SOCKET;
            return;
        }

        if (family == AF_NETLINK) {
            exHost = "Netlink";
        }
    }


    // Associates local address with the socket.
    bool Socket::Bind (const uint16_t port)
    {
        if (fd == INVALID_SOCKET) {
            LOG_ERROR("Socket.Bind: Socket is invalid.");
            return false;
        }

        if (socketFamily == AF_INET)
        {
            sockaddr_in service = { };
            service.sin_family = AF_INET;
            service.sin_addr.s_addr = INADDR_ANY;
            service.sin_port = htons(port);

            const int32_t result = bind(fd, reinterpret_cast<sockaddr*>(&service), sizeof(struct sockaddr_in));
            if (result == SOCKET_SUCCESS)
            {
                LOG_INFO("Socket.Bind [", fd, "]: Binding to local port '", port, "' is success.");
                return true;
            }
            LOG_ERROR("Socket.Bind [", fd, "]: Binding to local port '", port, "' failed - ", GET_ERROR(errno));
        }
        else if (socketFamily == AF_INET6)
        {
            sockaddr_in6 service6 = { };
            service6.sin6_family = AF_INET6;
            service6.sin6_addr = IN6ADDR_ANY_INIT;
            service6.sin6_port = htons(port);

            const int32_t result = bind(fd, reinterpret_cast<sockaddr*>(&service6), sizeof(struct sockaddr_in6));
            if (result == SOCKET_SUCCESS)
            {
                LOG_INFO("Socket.Bind [", fd, "]: Binding to local port '", port, "' is success.");
                return true;
            }
            LOG_ERROR("Socket.Bind [", fd, "]: Binding to local port '", port, "' failed - ", GET_ERROR(errno));
        }
        else { LOG_ERROR("Socket.Bind [", fd, "]: Unsupported socket family type - '", socketFamily, "'."); }

        return false;
    }


    // Associates a local address with prepared sockaddr.
    bool Socket::Bind (const struct sockaddr* const addr, const uint32_t size)
    {
        if (fd == INVALID_SOCKET) {
            LOG_ERROR("Socket.Bind: Socket is invalid.");
            return false;
        }

        if (bind(fd, addr, size) == SOCKET_SUCCESS)
        {
            LOG_INFO("Socket.Bind [", fd, "]: Binding to local port is success.");
            return true;
        }

        LOG_ERROR("Socket.Bind [", fd, "]: Binding to local port failed - ", GET_ERROR(errno));
        return false;
    }

    // Connecting to external host.
    bool Socket::Connect (const char* host, const uint16_t port)
    {
        if (fd == INVALID_SOCKET) {
            LOG_ERROR("Socket.Connect: Socket is invalid.");
            return false;
        }

        exHost = host;
        struct addrinfo hints = { };
        struct addrinfo* server = nullptr;
        hints.ai_family = socketFamily;
        hints.ai_socktype = socketType;

        const int32_t status = getaddrinfo(host, std::to_string(port).c_str(), &hints, &server);
        if (status != SOCKET_SUCCESS) {
            LOG_ERROR("Socket.Connect [", fd, "]: In function 'getaddrinfo' - ", gai_strerror(status));
            CloseAfterError();
            return false;
        }

        LOG_TRACE("Socket.Connect [", fd, "]: Connecting to '", host, "'...");
        for (auto&& curr = server; curr != nullptr; curr = curr->ai_next)
        {
            const int32_t result = connect(fd, curr->ai_addr, curr->ai_addrlen);
            if (result != SOCKET_ERROR || errno == EINPROGRESS)
            {
                LOG_INFO("Socket.Connect [", fd, "]: Connecting to '", exHost, "' on port '", port, "' is success.");
                freeaddrinfo(server);
                return true;
            }
            LOG_ERROR("Socket.Connect [", fd, "]: In function 'connect' - ", GET_ERROR(errno));
        }

        LOG_ERROR("Socket.Connect [", fd, "]: Connecting to '", exHost, "' on port '", port, "' failed.");
        freeaddrinfo(server);
        CloseAfterError();
        return false;
    }


    // Sending the message to external host.
    bool Socket::Send (const char* const data, const std::size_t length) noexcept
    {
        if (fd == INVALID_SOCKET) {
            LOG_ERROR("Socket.Send: Socket is invalid.");
            return false;
        }
        LOG_TRACE("Socket.Send [", fd, "]: Sending data to '", exHost, "' by TCP socket...");

        // Check callback functor.
        using functor = callbacks::SocketCallbackFunctorBeforeSend;
        auto callback = storage::GI.GetCallback<functor>(modules::MODULE_SOCKET, callbacks::MODULE_SOCKET_BEFORE_SEND_TCP);
        if (callback != nullptr)
        {
            LOG_TRACE("Socket.Send [", fd, "]: Calling the SocketCallbackFunctorBeforeSend functor...");
            callback->operator()(const_cast<char*>(data), const_cast<std::size_t*>(&length));
        }
        
        std::size_t idx = 0;
        while (idx != length)
        {
            const ssize_t result = send(fd, &data[idx], length - idx, 0);
            if (result == SOCKET_ERROR)
            {
                // The socket is marked non-blocking and the requested operation would block.
                if (errno == EWOULDBLOCK || errno == EAGAIN) {
                    if (IsReadyForSend(1500) == false) { CloseAfterError(); return false; }
                    continue;
                }
                // A signal occurred before any data was transmitted.
                if (errno == EINTR) { continue; }

                LOG_ERROR("Socket.Send [", fd, "]: In function 'send' - ", GET_ERROR(errno));
                CloseAfterError();
                return false;
            }
            idx += static_cast<std::size_t>(result);
        }

        LOG_TRACE("Socket.Send [", fd, "]: Sending data to '", exHost, "' is success: ", idx, " bytes.");
        return (idx == length);
    }


    // Receiving the message from external host.
    int32_t Socket::Recv (char* data, const std::size_t length, const bool noWait)
    {
        using std::chrono::system_clock;

        if (fd == INVALID_SOCKET) {
            LOG_ERROR("Socket.Recv: Socket is invalid.");
            return -1;
        }
        LOG_TRACE("Socket.Recv [", fd, "]: Receiving data from '", exHost, "'...");
        const system_clock::time_point limit = system_clock::now() + GetTimeout();

        std::size_t idx = 0;
        while (idx != length && system_clock::now() < limit)
        {
            const ssize_t result = recv(fd, &data[idx], length - idx, 0);
            if (result == SOCKET_ERROR)
            {
                // The socket is marked non-blocking and the requested operation would block.
                if (errno == EWOULDBLOCK || errno == EAGAIN)
                {
                    if (IsReadyForRecv(1500) == false) {
                        if (idx == 0) { CloseAfterError(); return -1; }
                        break; // In this case no any error because we have any data.
                    }
                    continue;
                }
                // A signal occurred before any data was transmitted.
                if (errno == EINTR) { continue; }

                LOG_ERROR("Socket.Recv [", fd, "]: In function 'recv' - ", GET_ERROR(errno));
                CloseAfterError();
                return -1;
            }
            if (result == 0) { break; }

            idx += static_cast<std::size_t>(result);
            if (noWait == true) { break; }
        }

        // Check callback functor.
        using functor = callbacks::SocketCallbackFunctorAfterReceive;
        auto callback = storage::GI.GetCallback<functor>(modules::MODULE_SOCKET, callbacks::MODULE_SOCKET_AFTER_RECEIVE_TCP);
        if (callback != nullptr)
        {
            LOG_TRACE("Socket.Recv [", fd, "]: Calling the SocketCallbackFunctorAfterReceive functor...");
            callback->operator()(data, idx);
        }

        LOG_TRACE("Socket.Recv [", fd, "]: Receiving data from '", exHost, "' is success: ", idx, " bytes.");
        return static_cast<int32_t>(idx);
    }

    // Receiving the message from external host over TCP until the functor returns false value.
    bool Socket::Recv (char* data, const std::size_t length, std::size_t& obtainLength, CompleteFunctor functor, std::size_t chunkLength)
    {
        using std::chrono::system_clock;

        if (fd == INVALID_SOCKET) {
            LOG_ERROR("Socket.Recv: Socket is invalid.");
            return false;
        }

        // Check chunk condition.
        bool withoutChunk = false;
        if (chunkLength == DEFAULT_NO_CHUNK || chunkLength > length)
        {
            withoutChunk = true;
            chunkLength = length;
        }

        const system_clock::time_point limit = system_clock::now() + GetTimeout();
        bool successFunctor = false;
        std::size_t idx = 0;
        do
        {
            const ssize_t result = recv(fd, &data[idx], chunkLength, 0);
            if (result == SOCKET_ERROR)
            {
                // The socket is marked non-blocking and the requested operation would block.
                if (errno == EWOULDBLOCK || errno == EAGAIN)
                {
                    if (IsReadyForRecv(1500) == false) {
                        if (idx == 0) { CloseAfterError(); return false; }
                        break;  // In this case no any error because we have any data.
                    }
                    continue;
                }
                // A signal occurred before any data was transmitted.
                if (errno == EINTR) { continue; }

                LOG_ERROR("Socket.Recv [", fd, "]: In function 'recv' - ", GET_ERROR(errno));
                CloseAfterError();
                return false;
            }
            if (result == 0) { break; }

            idx += static_cast<std::size_t>(result);
            if (withoutChunk == true) { chunkLength = length - idx; }
            else if (chunkLength > length - idx) { chunkLength = length - idx; }

        } while (idx != length && system_clock::now() < limit && (successFunctor = functor(data, idx)) == false);

        // Check callback functor.
        using func = callbacks::SocketCallbackFunctorAfterReceive;
        auto callback = storage::GI.GetCallback<func>(modules::MODULE_SOCKET, callbacks::MODULE_SOCKET_AFTER_RECEIVE_TCP);
        if (callback != nullptr)
        {
            LOG_TRACE("Socket.Recv [", fd, "]: Calling the SocketCallbackFunctorAfterReceive functor...");
            callback->operator()(data, idx);
        }

        obtainLength = idx;
        LOG_TRACE("Socket.Recv [", fd, "]: Receiving data from '", exHost, "' is success: ", idx, " bytes.");
        return successFunctor;
    }

    // Receiving the message from external host until reach the end.
    int32_t Socket::RecvToEnd (char* data, const std::size_t length) noexcept
    {
        using std::chrono::system_clock;

        if (fd == INVALID_SOCKET) {
            LOG_ERROR("Socket.RecvToEnd: Socket is invalid.");
            return -1;
        }
        LOG_TRACE("Socket.RecvToEnd [", fd, "]: Receiving data from '", exHost, "'...");
        const system_clock::time_point limit = system_clock::now() + GetTimeout();

        std::size_t idx = 0;
        while (idx != length && IsReadyForRecv(1500) == true && system_clock::now() < limit)
        {
            const ssize_t result = recv(fd, &data[idx], length - idx, 0);
            if (result == SOCKET_ERROR)
            {
                // A signal occurred before any data was transmitted.
                if (errno == EINTR) { continue; }

                LOG_ERROR("Socket.RecvToEnd [", fd, "]: In function 'recv' - ", GET_ERROR(errno));
                CloseAfterError();
                return -1;
            }
            if (result == 0) { break; }

            idx += static_cast<std::size_t>(result);
        }

        // Check callback functor.
        using functor = callbacks::SocketCallbackFunctorAfterReceive;
        auto callback = storage::GI.GetCallback<functor>(modules::MODULE_SOCKET, callbacks::MODULE_SOCKET_AFTER_RECEIVE_TCP);
        if (callback != nullptr)
        {
            LOG_TRACE("Socket.RecvToEnd [", fd, "]: Calling the SocketCallbackFunctorAfterReceive functor...");
            callback->operator()(data, idx);
        }

        LOG_TRACE("Socket.RecvToEnd [", fd, "]: Receiving data from '", exHost, "' is success: ", idx, " bytes.");
        return static_cast<int32_t>(idx);
    }

    // Method that sends message to external host over UDP protocol.
    bool Socket::SendTo (const char* host, const uint16_t port, const char* const data, const std::size_t length) noexcept
    {
        if (fd == INVALID_SOCKET) {
            LOG_ERROR("Socket.SendTo: Socket is invalid.");
            return false;
        }

        exHost = host;
        struct addrinfo hints = { };
        struct addrinfo* server = nullptr;
        hints.ai_family = socketFamily;
        hints.ai_socktype = socketType;

        const int32_t status = getaddrinfo(host, std::to_string(port).c_str(), &hints, &server);
        if (status != SOCKET_SUCCESS)
        {
            LOG_ERROR("Socket.SendTo [", fd, "]: In function 'getaddrinfo' - ", gai_strerror(status));
            CloseAfterError();
            return -1;
        }

        // Check callback functor.
        using functor = callbacks::SocketCallbackFunctorBeforeSend;
        auto callback = storage::GI.GetCallback<functor>(modules::MODULE_SOCKET, callbacks::MODULE_SOCKET_BEFORE_SEND_UDP);
        if (callback != nullptr)
        {
            LOG_TRACE("Socket.SendTo [", fd, "]: Calling the SocketCallbackFunctorBeforeSend functor...");
            callback->operator()(const_cast<char*>(data), const_cast<std::size_t*>(&length));
        }

        LOG_TRACE("Socket.SendTo [", fd, "]: Sending data to '", exHost, "' by UDP socket...");
        for (auto&& curr = server; curr != nullptr; curr = curr->ai_next)
        {
            std::size_t idx = 0;
            while (idx != length)
            {
                const ssize_t result = sendto(fd, &data[idx], length - idx, 0, curr->ai_addr, curr->ai_addrlen);
                if (result == SOCKET_ERROR)
                {
                    // The socket is marked non-blocking and the requested operation would block.
                    if (errno == EWOULDBLOCK || errno == EAGAIN) {
                        if (IsReadyForSend(1500) == false) { break; }
                        continue;
                    }
                    // A signal occurred before any data was transmitted.
                    if (errno == EINTR) { continue; }
                    break;  // Error occurred.
                }
                idx += static_cast<std::size_t>(result);
            }

            // Check any errors.
            if (idx == length)
            {
                LOG_INFO("Socket.SendTo [", fd, "]: Sending data to '", exHost, "' on port '", port, "' is success.");
                freeaddrinfo(server);
                return true;
            }
        }

        LOG_ERROR("Socket.SendTo [", fd, "]: In function 'send' - ", GET_ERROR(errno));
        CloseAfterError();
        return false;
    }

    // Method that receives message from external host over UDP protocol.
    int32_t Socket::RecvFrom (const char* host, const uint16_t port, char* data, const std::size_t length) noexcept
    {
        if (fd == INVALID_SOCKET) {
            LOG_ERROR("Socket.RecvFrom: Socket is invalid.");
            return -1;
        }
        return 0;
    }


    // Checks availability socket on read/write.
    uint16_t Socket::CheckSocketState (const int32_t time) const noexcept
    {
        struct epoll_event ev = { };
        ev.events = EPOLLIN | EPOLLOUT;
        if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == SOCKET_ERROR) {
            LOG_ERROR("Socket.CheckSocketState [", fd, "]: In function 'epoll_ctl' - ", GET_ERROR(errno));
            return 0;
        }

        struct epoll_event events = { };
        const int32_t wfd = epoll_wait(epfd, &events, 1, time);
        if (wfd == 1)
        {
            if ((events.events & EPOLLIN) != 0U && (events.events & EPOLLOUT) != 0U) {
                return 3;
            }
            if ((events.events & EPOLLIN) != 0U) { return 1; }
            if ((events.events & EPOLLOUT) != 0U) { return 2; }
            if (((events.events & EPOLLERR) != 0U) || ((events.events & EPOLLHUP) != 0U)) {
                LOG_ERROR("Socket.CheckSocketState [", fd, "]: Function 'epoll_wait' return error event: ", uint32_t(events.events), '.');
            }
            else {
                LOG_ERROR("Socket.CheckSocketState [", fd, "]: Function 'epoll_wait' return unknown event: ", uint32_t(events.events), '.');
            }
        }
        else if (wfd == 0) { LOG_ERROR("Socket.CheckSocketState [", fd, "]: In function 'epoll_wait' - Timeout expired."); }
        else { LOG_ERROR("Socket.CheckSocketState [", fd, "]: In function 'epoll_wait' - ", GET_ERROR(errno)); }
        return 0;
    }


    // Checks availability socket on write.
    bool Socket::IsReadyForSend (const int32_t time) noexcept
    {
        event.events = EPOLLOUT;
        if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event) == SOCKET_ERROR) {
            LOG_ERROR("Socket.IsReadyForSend [", fd, "]: In function 'epoll_ctl (s)' - ", GET_ERROR(errno));
            return false;
        }

        struct epoll_event events = { };
        const int32_t wfd = epoll_wait(epfd, &events, 1, time);
        if (wfd == 1)
        {
            if ((events.events & EPOLLOUT) != 0U) { return true; }
            if (((events.events & EPOLLERR) != 0U) || ((events.events & EPOLLHUP) != 0U)) {
                LOG_ERROR("Socket.IsReadyForSend [", fd, "]: Function 'epoll_wait (s)' return error event: ", uint32_t(events.events), '.');
            }
            else {
                LOG_ERROR("Socket.IsReadyForSend [", fd, "]: Function 'epoll_wait (s)' return unknown event: ", uint32_t(events.events), '.');
            }
        }
        else if (wfd == 0) { LOG_ERROR("Socket.IsReadyForSend [", fd, "]: In function 'epoll_wait (s)' - Timeout expired."); }
        else { LOG_ERROR("Socket.IsReadyForSend [", fd, "]: In function 'epoll_wait (s)' - ", GET_ERROR(errno)); }
        return false;
    }


    // Checks availability socket on read.
    bool Socket::IsReadyForRecv (const int32_t time) noexcept
    {
        event.events = EPOLLIN;
        if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event) == SOCKET_ERROR) {
            LOG_ERROR("Socket.IsReadyForRecv [", fd, "]: In function 'epoll_ctl (r)' - ", GET_ERROR(errno));
            return false;
        }

        struct epoll_event events = { };
        const int32_t wfd = epoll_wait(epfd, &events, 1, time);
        if (wfd == 1)
        {
            if ((events.events & EPOLLIN) != 0U) { return true; }
            if (((events.events & EPOLLERR) != 0U) || ((events.events & EPOLLHUP) != 0U)) {
                LOG_ERROR("Socket.IsReadyForRecv [", fd, "]: Function 'epoll_wait (r)' return error event: ", uint32_t(events.events), '.');
            }
            else {
                LOG_ERROR("Socket.IsReadyForRecv [", fd, "]: Function 'epoll_wait (r)' return unknown event: ", uint32_t(events.events), '.');
            }
        }
        else if (wfd == 0) { LOG_ERROR("Socket.IsReadyForRecv [", fd, "]: In function 'epoll_wait (r)' - Timeout expired."); }
        else { LOG_ERROR("Socket.IsReadyForRecv [", fd, "]: In function 'epoll_wait (r)' - ", GET_ERROR(errno)); }
        return false;
    }


    // Set socket to Non-Blocking state.
    bool Socket::SetSocketToNonBlock(void) noexcept
    {
        const int32_t flags = fcntl(fd, F_GETFL, 0);
        if (flags == INVALID_SOCKET) {
            LOG_ERROR("Socket.SetSocketToNonBlock [", fd, "]: When getting socket options - ", GET_ERROR(errno));
            return false;
        }
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == INVALID_SOCKET) {
            LOG_ERROR("Socket.SetSocketToNonBlock [", fd, "]: When setting socket options - ", GET_ERROR(errno));
            return false;
        }
        return true;
    }

    // Disable SIGPIPE signal for send.
    bool Socket::DisableSignalSIGPIPE(void) const noexcept
    {
        static sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGPIPE);
        const int32_t ret = pthread_sigmask(SIG_BLOCK, &mask, nullptr);
        if (ret != 0) {
            LOG_ERROR("Socket.DisableSignalSIGPIPE [", fd, "]: When block signal 'SIG_BLOCK' - ", GET_ERROR(ret));
            return false;
        }
        return true;
    }

    // Shutdown the connection.
    void Socket::Shutdown (const int32_t how) const
    {
        if (fd != INVALID_SOCKET && socketType == SOCK_STREAM && IsAlive() == true)
        {
            if (shutdown(fd, how) == SOCKET_ERROR) {
                LOG_ERROR("Socket.Shutdown [", fd, "]: In function 'shutdown' - ", GET_ERROR(errno));
                return;
            }
            LOG_INFO("Socket.Shutdown [", fd, "]: Connection shutdown by mode: ", how, '.');
        }
    }

    // Close the connection.
    void Socket::Close(void)
    {
        if (fd != INVALID_SOCKET) {
            close(fd);
            LOG_INFO("Socket.Close [", fd, "]: Connection closed with host: '", exHost, "'.");
            fd = INVALID_SOCKET;
        }
        if (epfd != INVALID_SOCKET) { close(epfd); epfd = INVALID_SOCKET; }
    }

    // Cleaning after error.
    void Socket::CloseAfterError(void)
    {
        Close();
        exHost.clear();
    }

    // Destructor.
    Socket::~Socket(void)
    {
        if (fd != INVALID_SOCKET) { close(fd); fd = INVALID_SOCKET; }
        if (epfd != INVALID_SOCKET) { close(epfd); epfd = INVALID_SOCKET; }
        exHost.clear();
    }




    SocketStatePool::SocketStatePool(void) noexcept
    {
        // Create array for socket events.
        events = system::allocMemoryForArray<struct epoll_event>(MAXIMUM_SOCKET_DESCRIPTORS);
        if (events == nullptr) {
            LOG_FATAL("SocketStatePool.SocketStatePool: In function 'alloc_memory'.");
            std::terminate();
        }

        // Create epoll descriptor.
        epoll_fd = epoll_create1(0);
        if (epoll_fd == INVALID_SOCKET) {
            LOG_FATAL("SocketStatePool.SocketStatePool: In function 'epoll_create1' - ", GET_ERROR(errno));
            std::terminate();
        }

        // Start thread for epoll loop with events pointer.

    }

    SocketStatePool& SocketStatePool::Instance(void) noexcept
    {
        // Since it's a static variable, if the class has already been created, its won't be created again.
        // It's thread-safe since C++11.
        static SocketStatePool instance;
        return instance;
    }



}  // namespace net.
