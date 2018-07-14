// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include <vector>
#include <unordered_map>

#include "../../include/framework/Socket.hpp"
#include "../../include/framework/Utilities.hpp"


namespace analyzer::utility
{
#if (defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER >= 0x1000208fL)  // If OPENSSL version more then 1.0.2h.

        std::set<std::string> CheckALPNSupportedProtocols (const std::string& host)
        {
            LOG_TRACE("CheckALPNSupportedProtocols:   Start check...");
            std::unordered_map<std::string, std::vector<uint8_t>> protocols;
            protocols["HTTP/2.0"]    =  { 2, 'h', '2' };
            protocols["HTTP/2.0-14"] =  { 5, 'h', '2', '-', '1', '4' };
            protocols["HTTP/2.0-16"] =  { 5, 'h', '2', '-', '1', '6' };
            protocols["SPDY/1.0"]    =  { 6, 's', 'p', 'd', 'y', '/', '1' };
            protocols["SPDY/2.0"]    =  { 6, 's', 'p', 'd', 'y', '/', '2' };
            protocols["SPDY/3.0"]    =  { 6, 's', 'p', 'd', 'y', '/', '3' };
            protocols["SPDY/4.0"]    =  { 6, 's', 'p', 'd', 'y', '/', '4' };
            protocols["SPDY/3.1"]    =  { 8, 's', 'p', 'd', 'y', '/', '3', '.', '1' };
            protocols["HTTP/1.1"]    =  { 8, 'h', 't', 't', 'p', '/', '1', '.', '1' };

            std::set<std::string> result;
            for (const auto& [ protocol, bytes ] : protocols)
            {
                net::SocketSSL sock;
                if (sock.SetInternalProtocol(bytes.data(), bytes.size()) == false) {
                    return std::set<std::string>();
                }
                if (sock.Connect(host.c_str()) == false) { return std::set<std::string>(); }

                const std::string proto = sock.GetRawSelectedProtocol();
                if (proto.empty() == false) {
                    result.emplace(protocol);
                    LOG_INFO("Next protocol: ", proto, '.');
                }
                sock.Close();
            }
            LOG_TRACE("CheckALPNSupportedProtocols:   End check...");
            return result;
        }

#endif

        std::set<std::string> CheckSupportedTLSProtocols (const std::string& host)
        {
            LOG_TRACE("CheckSupportedTLSProtocols:   Start check...");
            std::unordered_map<std::string, uint16_t> protocols;
            protocols["TLS 1.0"] = SSL_METHOD_TLS1;
            protocols["TLS 1.1"] = SSL_METHOD_TLS11;
            protocols["TLS 1.2"] = SSL_METHOD_TLS12;
            //protocols["TLS 1.3"] == SSL_METHOD_TLS13;

            std::set<std::string> result;
            for (const auto& [ protocol, method ] : protocols)
            {
                net::SocketSSL sock(method);
                if (sock.Connect(host.c_str()) == false) { return std::set<std::string>(); }

                result.emplace(protocol);
                LOG_INFO("Next protocol: ", protocol, '.');
                sock.Close();
            }
            LOG_TRACE("CheckSupportedTLSProtocols:   End check...");
            return result;
        }

}  // namespace utility.
