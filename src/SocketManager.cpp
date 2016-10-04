#include "../include/SocketManager.hpp"
#include "../include/Log.hpp"

namespace analyzer {
    namespace net {

        NonBlockSocketManager::NonBlockSocketManager() : timeout(DEFAULT_TIMEOUT) { }
        void NonBlockSocketManager::SetTimeout (const uint32_t time) { timeout = time; }


        void* NonBlockSocketManager::thread_worker (void* arg)
        {
            auto info = static_cast<socket_init*>(arg);
            info->work_t.lock();

            info->sock = std::make_unique<Socket>(info->family, info->type, info->protocol, info->timeout);
            if (info->sock->IsError()) {
                pthread_exit(nullptr);
            }

            info->sock->Connect(info->host.c_str(), info->port);
            if (info->sock->IsError()) {
                pthread_exit(nullptr);
            }

            info->sock->Send(info->send_buffer, info->send_length);
            if (info->sock->IsError()) {
                pthread_exit(nullptr);
            }

            info->recv_buffer = alloc_memory(DEFAULT_BUFFER_SIZE);
            if (info->recv_buffer == nullptr) {
                pthread_exit(nullptr);
            }

            int32_t recv_bytes = info->sock->RecvToEnd(info->recv_buffer.get(), DEFAULT_BUFFER_SIZE);
            if (info->sock->IsError()) {
                pthread_exit(nullptr);
            }
            info->recv_length = static_cast<size_t>(recv_bytes);

            info->flagSuccess = true;
            info->work_t.unlock();
            pthread_exit(nullptr);
        }


        size_t NonBlockSocketManager::FindFd (const pthread_t fd) const
        {
            for (size_t i = 0; i < hosts.size(); ++i) {
                if (hosts[i].first == fd) { return i; }
            }
            return fpos;
        }


        pthread_t NonBlockSocketManager::Add (const char* host, char* buffer, const size_t length, const uint16_t port, const uint16_t protocol)
        {
            auto info = new socket_init;
            info->host = host;
            info->port = port;
            info->send_buffer = buffer;
            info->send_length = length;

            switch (protocol) {
                case TCPv4:
                    info->family = AF_INET;
                    info->type = SOCK_STREAM;
                    info->protocol = IPPROTO_TCP;
                    break;
                case TCPv6:
                    info->family = AF_INET6;
                    info->type = SOCK_STREAM;
                    info->protocol = IPPROTO_TCP;
                    break;
                case UDPv4:
                    info->family = AF_INET;
                    info->type = SOCK_DGRAM;
                    info->protocol = IPPROTO_UDP;
                    break;
                case UDPv6:
                    info->family = AF_INET6;
                    info->type = SOCK_DGRAM;
                    info->protocol = IPPROTO_UDP;
                    break;

                default:
                    log::DbgLog("[error] SocketManager: Error in protocol type.");
                    return 0;
            }

            pthread_t thread_id;
            int32_t result = pthread_create(&thread_id, nullptr, thread_worker, static_cast<void*>(info));
            if (result != 0) {
                log::DbgLog("[error] SocketManager: Error in function 'pthread_create' - ", log::get_strerror(result));
                return 0;
            }

            hosts.push_back( std::make_pair(thread_id, info));
            return thread_id;
        }


        void NonBlockSocketManager::Wait (const pthread_t fd) const
        {
            int32_t result = pthread_join(fd, nullptr);
            if (result != 0) {
                log::DbgLog("[error] SocketManager: Error in function 'pthread_join' - ", log::get_strerror(result));
            }
        }


        void NonBlockSocketManager::WaitAll () const
        {
            for (auto it : hosts) {
                int32_t result = pthread_join(it.first, nullptr);
                if (result != 0) {
                    log::DbgLog("[error] SocketManager: Error in function 'pthread_join' - ", log::get_strerror(result));
                }
                std::cout << it.first << "   " << it.second->flagSuccess << std::endl; //...........DEBUG...............
            }
        }


        data_t NonBlockSocketManager::GetData (const pthread_t fd) const
        {
            size_t idx = FindFd(fd);
            if (idx != fpos) {
                if (hosts[idx].second->work_t.try_lock()) {
                    if (hosts[idx].second->flagSuccess) {
                        return std::make_pair(std::move(hosts[idx].second->recv_buffer),
                                              hosts[idx].second->recv_length);
                    }
                }
                hosts[idx].second->work_t.unlock();
            }
            return std::make_pair(nullptr, 0);
        }


        // Function, which allocate dinamic memory.
        std::unique_ptr<char[]> NonBlockSocketManager::alloc_memory (const size_t size) noexcept
        {
            try {
                auto tmp = std::make_unique<char[]>(size);
                return tmp;
            }
            catch (std::exception &err) {
                log::DbgLog("[error] SocketManager: Error in function 'alloc_memory' - ", err.what());
                return nullptr;
            }
        }


        NonBlockSocketManager::~NonBlockSocketManager() {
            for (auto it : hosts) {
                delete it.second;
            }
            hosts.clear();
        }

    } // namespace net.
} // namespace analyzer.
