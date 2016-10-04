#include "../include/Socket.hpp"

namespace analyzer {
    namespace net {

        // Constructor.
        Socket::Socket (const int32_t family, const int32_t type, const int32_t protocol, const uint32_t time) :
                socketFamily(family), socketType(type), ipProtocol(protocol), timeout(std::chrono::seconds(time))
        {
            log::DbgLog("[*] Socket: Creating socket...");
            fd = socket(socketFamily, socketType, ipProtocol);
            if (fd == INVALID_SOCKET) {
                log::DbgLog("[error] Socket: Create socket error - ", log::get_strerror(errno));
                isErrorOccurred = true;
                return;
            }

            if (!SetSocketToNonBlock()) {
                CloseAfterError();
                return;
            }
            log::DbgLog("[+] Socket [", fd,"]: Socket was created.");

            epfd = epoll_create1(0);
            if (epfd == INVALID_SOCKET) {
                log::DbgLog("[error] Socket [", fd,"]: Error in function 'epoll_create1' - ", log::get_strerror(errno));
                CloseAfterError();
                return;
            }

            event.data.fd = fd;
            if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == SOCKET_ERROR) {
                log::DbgLog("[error] Socket [", fd,"]: Error in function 'epoll_ctl' - ", log::get_strerror(errno));
                CloseAfterError();
            }
        }


        // Connecting to external host.
        void Socket::Connect (const char* host, const uint16_t port)
        {
            if (fd == INVALID_SOCKET) {
                log::DbgLog("[error] Socket: Socket is invalid.");
                return;
            }

            log::DbgLog("[*] Socket [", fd,"]: Connecting to host '", host, "'...");
            exHost = host;
            struct addrinfo hints = { };
            struct addrinfo *server, *curr;

            hints.ai_family = socketFamily;
            hints.ai_socktype = socketType;

            int32_t status = getaddrinfo(host, std::to_string(port).c_str(), &hints, &server);
            if (status != 0) {
                log::DbgLog("[error] Socket [", fd,"]: Error in function 'getaddrinfo' - ", gai_strerror(status));
                CloseAfterError();
                return;
            }

            for (curr = server; curr != nullptr; curr = curr->ai_next)
            {
                int32_t result = connect(fd, curr->ai_addr, curr->ai_addrlen);
                if (result != SOCKET_ERROR || errno == EINPROGRESS) {
                    log::SysLog("[+] Socket [", fd,"]: Connecting to host '", exHost, "' on port '", port,"' is successful.");
                    isConnectionAlive = true;
                    freeaddrinfo(server);
                    return;
                }
                log::DbgLog("[error] Socket [", fd,"]: Error in function 'connect' - ", log::get_strerror(errno));
            }
            log::SysLog("[error] Socket [", fd,"]: Connecting to host '", exHost, "' on port '", port,"' failed.");
            freeaddrinfo(server);
            CloseAfterError();
        }


        // Sending the message to external host.
        int32_t Socket::Send (char* data, size_t length)
        {
            if (fd == INVALID_SOCKET) {
                log::DbgLog("[error] Socket: Socket is invalid.");
                return -1;
            }
            log::DbgLog("[*] Socket [", fd,"]: Sending data to host '", exHost, "'...");

            size_t idx = 0;
            while (length > 0)
            {
                intmax_t result = send(fd, &data[idx], length, 0);
                if (result == SOCKET_ERROR) {
                    if (errno == EWOULDBLOCK) {
                        if (!IsReadyForSend(3000)) { CloseAfterError(); return -1; }
                        continue;
                    }
                    log::DbgLog("[error] Socket [", fd,"]: Error in function 'send' - ", log::get_strerror(errno));
                    CloseAfterError(); return -1;
                }

                length -= static_cast<size_t>(result);
                idx += static_cast<size_t>(result);
            }

            log::DbgLog("[+] Socket [", fd,"]: Sending data to host '", exHost, "' is successful: ", idx, " bytes.");
            return static_cast<int32_t>(idx);
        }


        // Receiving the message from external host.
        int32_t Socket::Recv (char* data, size_t length, const bool noWait)
        {
            if (fd == INVALID_SOCKET) {
                log::DbgLog("[error] Socket: Socket is invalid.");
                return -1;
            }
            log::DbgLog("[*] Socket [", fd,"]: Receiving data from host '", exHost, "'...");
            const system_clock::time_point limit = system_clock::now() + timeout;

            size_t idx = 0;
            while (length > 0 && system_clock::now() < limit)
            {
                intmax_t result = recv(fd, &data[idx], length, 0);
                if (result == SOCKET_ERROR) {
                    if (errno == EWOULDBLOCK) {
                        if (!IsReadyForRecv(3000)) {
                            if (idx == 0) { CloseAfterError(); return -1; }
                            break; // In this case no error.
                        }
                        continue;
                    }
                    log::DbgLog("[error] Socket [", fd,"]: Error in function 'recv' - ", log::get_strerror(errno));
                    CloseAfterError(); return -1;
                }
                if (result == 0) { break; }

                length -= static_cast<size_t>(result);
                idx += static_cast<size_t>(result);
                if (noWait) { break; }
            }

            log::DbgLog("[+] Socket [", fd,"]: Receiving data from host '", exHost, "' is successful: ", idx, " bytes.");
            return static_cast<int32_t>(idx);
        }


        // Receiving the message from external host until reach the end.
        int32_t Socket::RecvToEnd (char* data, size_t length)
        {
            if (fd == INVALID_SOCKET) {
                log::DbgLog("[error] Socket: Socket is invalid.");
                return -1;
            }
            log::DbgLog("[*] Socket [", fd,"]: Receiving data from host '", exHost, "'...");
            const system_clock::time_point limit = system_clock::now() + timeout;

            size_t idx = 0;
            while (length > 0 && IsReadyForRecv(3000) && system_clock::now() < limit)
            {
                intmax_t result = recv(fd, &data[idx], length, 0);
                if (result == SOCKET_ERROR) {
                    log::DbgLog("[error] Socket [", fd,"]: Error in function 'recv' - ", log::get_strerror(errno));
                    CloseAfterError(); return -1;
                }
                if (result == 0) { break; }

                length -= static_cast<size_t>(result);
                idx += static_cast<size_t>(result);
            }

            log::DbgLog("[+] Socket [", fd,"]: Receiving data to host '", exHost, "' is successful: ", idx, " bytes.");
            return static_cast<int32_t>(idx);
        }


        // Checks availability socket on read/write.
        uint16_t Socket::CheckSocketState (const int32_t time)
        {
            event.events = EPOLLIN | EPOLLOUT;
            if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event) == SOCKET_ERROR) {
                log::DbgLog("[error] Socket [", fd,"]: Error in function 'epoll_ctl' - ", log::get_strerror(errno));
                return 0;
            }

            struct epoll_event events = { };
            int32_t wfd = epoll_wait(epfd, &events, 1, time);
            if (wfd == 1) {
                if (((events.events & EPOLLERR) != 0u) || ((events.events & EPOLLHUP) != 0u)) {
                    log::DbgLog("[error] Socket [", fd,"]: Error in function 'epoll_wait' - ", log::get_strerror(errno));
                }
                else if ((events.events & EPOLLIN) != 0u && (events.events & EPOLLOUT) != 0u) {
                    return 3;
                }
                else if ((events.events & EPOLLIN) != 0u) { return 1; }
                else if ((events.events & EPOLLOUT) != 0u) { return 2; }
                else {
                    log::DbgLog("[error] Socket [", fd,"]: Error in function 'epoll_wait' - ", log::get_strerror(errno));
                }
            }
            else if (wfd == 0) { log::DbgLog("[*] Socket [", fd,"]: In function 'epoll_wait' - Timeout expired."); }
            else {
                log::DbgLog("[error] Socket [", fd,"]: Error in function 'epoll_wait' - ", log::get_strerror(errno));
            }
            return 0;
        }


        // Checks availability socket on write.
        bool Socket::IsReadyForSend (const int32_t time)
        {
            event.events = EPOLLOUT;
            if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event) == SOCKET_ERROR) {
                log::DbgLog("[error] Socket [", fd,"]: Error in function 'epoll_ctl (s)' - ", log::get_strerror(errno));
                return false;
            }

            struct epoll_event events = { };
            int32_t wfd = epoll_wait(epfd, &events, 1, time);
            if (wfd == 1) {
                if (((events.events & EPOLLERR) != 0u) || ((events.events & EPOLLHUP) != 0u)) {
                    log::DbgLog("[error] Socket [", fd,"]: Error in function 'epoll_wait (s)' - ", log::get_strerror(errno));
                }
                else if ((events.events & EPOLLOUT) != 0u) { return true; }
                else {
                    log::DbgLog("[error] Socket [", fd,"]: Error in function 'epoll_wait (s)' - ", log::get_strerror(errno));
                }
            }
            else if (wfd == 0) { log::DbgLog("[*] Socket [", fd,"]: In function 'epoll_wait (s)' - Timeout expired."); }
            else {
                log::DbgLog("[error] Socket [", fd,"]: Error in function 'epoll_wait (s)' - ", log::get_strerror(errno));
            }
            return false;
        }


        // Checks availability socket on read.
        bool Socket::IsReadyForRecv (const int32_t time)
        {
            event.events = EPOLLIN;
            if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event) == SOCKET_ERROR) {
                log::DbgLog("[error] Socket [", fd,"]: Error in function 'epoll_ctl (s)' - ", log::get_strerror(errno));
                return false;
            }

            struct epoll_event events = { };
            int32_t wfd = epoll_wait(epfd, &events, 1, time);
            if (wfd == 1) {
                if (((events.events & EPOLLERR) != 0u) || ((events.events & EPOLLHUP) != 0u)) {
                    log::DbgLog("[error] Socket [", fd,"]: Error in function 'epoll_wait (r)' - ", log::get_strerror(errno));
                }
                else if ((events.events & EPOLLIN) != 0u) { return true; }
                else {
                    log::DbgLog("[error] Socket [", fd,"]: Error in function 'epoll_wait (r)' - ", log::get_strerror(errno));
                }
            }
            else if (wfd == 0) { log::DbgLog("[*] Socket [", fd,"]: In function 'epoll_wait (r)' - Timeout expired."); }
            else {
                log::DbgLog("[error] Socket [", fd,"]: Error in function 'epoll_wait (r)' - ", log::get_strerror(errno));
            }
            return false;
        }


        // Set Socket to Non-Blocking state.
        bool Socket::SetSocketToNonBlock () {
            int32_t flags = fcntl(fd, F_GETFL, 0);
            if (flags == INVALID_SOCKET) {
                log::DbgLog("[error] Socket [", fd,"]: Getting socket options error - ", log::get_strerror(errno));
                return false;
            }
            if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == INVALID_SOCKET) {
                log::DbgLog("[error] Socket [", fd,"]: Setting socket options error - ", log::get_strerror(errno));
                return false;
            }
            return true;
        }


        // Shutdown the connection.
        void Socket::Shutdown (int32_t how)
        {
            if (fd != INVALID_SOCKET && socketType == SOCK_STREAM && isConnectionAlive)
            {
                if (shutdown(fd, how) == SOCKET_ERROR) {
                    log::DbgLog("[error] Socket [", fd,"]: Error in function 'shutdown' - ", log::get_strerror(errno));
                    CloseAfterError();
                }
                else {
                    log::DbgLog("[-] Socket [", fd,"]: Connection closed.");
                }
            }
            isConnectionAlive = false;
        }


        // Close the connection.
        void Socket::Close () {
            if (fd != INVALID_SOCKET) { close(fd); fd = INVALID_SOCKET; }
            if (epfd != INVALID_SOCKET) { close(epfd); epfd = INVALID_SOCKET; }
            isConnectionAlive = false;
        }


        // Cleaning after error.
        void Socket::CloseAfterError () {
            Close();
            isErrorOccurred = true;
        }


        // Destructor.
        Socket::~Socket() {
            if (fd != INVALID_SOCKET) { close(fd); fd = INVALID_SOCKET; }
            if (epfd != INVALID_SOCKET) { close(epfd); epfd = INVALID_SOCKET; }
            isConnectionAlive = false;
            exHost.clear();
        }

    } // namespace net.
} // namespace analyzer.
