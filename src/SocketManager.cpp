// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../include/SocketManager.hpp"

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
            info->recv_length = static_cast<std::size_t>(recv_bytes);

            info->flagSuccess = true;
            info->work_t.unlock();
            pthread_exit(nullptr);
        }


        std::size_t NonBlockSocketManager::FindFd (const pthread_t fd) const
        {
            for (std::size_t i = 0; i < hosts.size(); ++i) {
                if (hosts[i].first == fd) { return i; }
            }
            return NonBlockSocketManager::fpos;
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
                    delete info;
                    return 0;
            }

            pthread_t thread_id;
            const int32_t result = pthread_create(&thread_id, nullptr, thread_worker, static_cast<void*>(info));
            if (result != 0) {
                log::DbgLog("[error] SocketManager: Error in function 'pthread_create' - ", GET_ERROR(result));
                return 0;
            }

            hosts.push_back(std::make_pair(thread_id, info));
            return thread_id;
        }


        void NonBlockSocketManager::Wait (const pthread_t fd) const
        {
            const int32_t result = pthread_join(fd, nullptr);
            if (result != 0) {
                log::DbgLog("[error] SocketManager: Error in function 'pthread_join' - ", GET_ERROR(result));
            }
        }


        void NonBlockSocketManager::WaitAll () const
        {
            for (auto&& it : hosts) {
                int32_t result = pthread_join(it.first, nullptr);
                if (result != 0) {
                    log::DbgLog("[error] SocketManager: Error in function 'pthread_join' - ", GET_ERROR(result));
                }
                std::cout << it.first << "   " << it.second->flagSuccess << std::endl; //.......DEBUG...ONLY.......
            }
        }


        data_t NonBlockSocketManager::GetData (const pthread_t fd) const
        {
            std::size_t idx = FindFd(fd);
            if (idx != fpos) {
                if (hosts[idx].second->work_t.try_lock()) {
                    hosts[idx].second->work_t.unlock();

                    if (hosts[idx].second->flagSuccess) {
                        return data_t(hosts[idx].second->recv_buffer, hosts[idx].second->recv_length);
                    }
                }
            }
            return data_t(nullptr, 0);
        }


        // Function, which allocate dynamic memory.
        std::unique_ptr<char[]> NonBlockSocketManager::alloc_memory (const size_t size) noexcept
        {
            try {
                return std::make_unique<char[]>(size);
            }
            catch (std::exception& err) {
                log::DbgLog("[error] SocketManager: Error in function 'alloc_memory' - ", err.what());
                return nullptr;
            }
        }


        NonBlockSocketManager::~NonBlockSocketManager() {
            for (auto&& it : hosts) {
                delete it.second;
            }
            hosts.clear();
        }


        data_t::data_t (char* in, const size_t size) : data(in), length(size) { }
        data_t::data_t (std::unique_ptr<char[]>& in, const size_t size) : data(std::move(in)), length(size) { }

    }  // namespace net.
}  // namespace analyzer.
