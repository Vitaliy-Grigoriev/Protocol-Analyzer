// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include <map>
#include <iostream>

#include "../include/framework/AnalyzerApi.hpp"


int32_t main (int32_t size, char** data)
{
    /*const std::string host = "habrahabr.ru";
    const auto protos = analyzer::utility::CheckSupportedTLSProtocols(host);
    if (protos.empty() == false)
    {
        std::cout << "Find next protocols on the host (" << host << "): " << std::endl;
        for (auto&& p : protos) {
            std::cout << p << std::endl;
        }
        std::cout << std::endl;
    }*/

    auto sockets = analyzer::system::allocMemoryForArrayOfObjects<analyzer::net::SocketSSL>(3, SSL_METHOD_TLS11);
    if (sockets == nullptr) {
        std::cerr << "[error] Alloc memory fail..." << std::endl;
        return EXIT_FAILURE;
    }

    auto request = [] (analyzer::net::SocketSSL* sock, const std::string& domain) noexcept -> bool
    {
#if (defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER >= 0x1000208fL)  // If OPENSSL version more then 1.0.2h.
        if (sock->SetHttp_1_1_OnlyProtocol() == false) {
            std::cerr << "[error] Set HTTP/1.1 protocol only failed..." << std::endl;
            return EXIT_FAILURE;
        }
#endif

        if (sock->SetOnlySecureCiphers() == false) {
            std::cerr << "[error] Set secure ciphers failed..." << std::endl;
            return EXIT_FAILURE;
        }

        if (sock->Connect(domain.c_str()) == false) {
            std::cerr << "[error] Connection fail..." << std::endl;
            return false;
        }

        const std::string buffer = "GET / HTTP/1.1\r\nHost: " + domain + "\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
        int32_t result = sock->Send(buffer.c_str(), buffer.size());
        if (result == -1) {
            std::cerr << "[error] Send fail..." << std::endl;
            return false;
        }

        char buff_receive[DEFAULT_BUFFER_SIZE] = { };
        result = sock->Recv(buff_receive, DEFAULT_BUFFER_SIZE, true);
        if (result == -1) {
            std::cerr << "[error] Receive fail..." << std::endl;
            return false;
        }
        std::cerr << '[' << domain << "] Received data length: " << result << std::endl;

        sock->Close();
        return true;
    };

    std::cerr << request(sockets[0].get(), "tproger.ru") << std::endl;
    std::cerr << request(sockets[1].get(), "habrahabr.ru") << std::endl;
    std::cerr << request(sockets[2].get(), "geektimes.ru") << std::endl;

    return EXIT_SUCCESS;
}
