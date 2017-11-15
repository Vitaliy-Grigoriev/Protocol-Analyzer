// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <mutex>
#include <iostream>

#include "../include/analyzer/Api.hpp"

using namespace analyzer;


int32_t main (int32_t size, char** data)
{
    std::string text = "0123456789";
    common::types::RawDataBuffer buffer1 = { };
    buffer1.AssignData(text.data(), text.size());
    std::cout << buffer1.Size() << std::endl;

    common::types::RawDataBuffer buffer2(buffer1);
    std::cout << buffer1.Size() << std::endl;
    std::cout << buffer2.Size() << std::endl;

    common::types::RawDataBuffer buffer3 = std::move(buffer1);
    std::cout << buffer1.Size() << std::endl;
    std::cout << buffer3.Size() << std::endl;

    std::cout << char(buffer3[5]) << std::endl;
    std::cout << char(buffer3[20]) << std::endl;

    return EXIT_FAILURE;


    auto sockets = system::allocMemoryForArrayOfObjects<net::SocketSSL>(3, SSL_METHOD_TLS11);
    if (sockets == nullptr) {
        std::cout << "[error] Alloc fail..." << std::endl;
        return EXIT_FAILURE;
    }


    auto request = [] (net::SocketSSL* sock, const std::string& domain) noexcept -> bool
    {
        if (sock->Connect(domain.c_str()) == false) {
            std::cout << "[error] Connection fail..." << std::endl;
            return false;
        }

        const std::string buff = "GET / HTTP/1.1\r\nHost: " + domain + "\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
        const int32_t ret = sock->Send(buff.c_str(), buff.size());
        if (ret == -1) {
            std::cout << "[error] Send fail..." << std::endl;
            return false;
        }

        char buff_receive[16384] = { };
        const int32_t result = sock->RecvToEnd(buff_receive, 16384);
        if (result == -1) {
            std::cout << "[error] Recv fail..." << std::endl;
            return false;
        }
        std::cerr << "Received data length: " << result << std::endl << std::endl << std::endl;
        return true;
    };


    std::cerr << request(sockets[0].get(), "tproger.ru") << std::endl;
    std::cerr << request(sockets[1].get(), "habrahabr.ru") << std::endl;
    std::cerr << request(sockets[2].get(), "geektimes.ru") << std::endl;


    std::cerr << "[+] Exit." << std::endl;
    return EXIT_SUCCESS;
}


