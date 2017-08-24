// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <vector>
#include <unordered_map>

#include "../include/analyzer/Utilities.hpp"


namespace analyzer::utility
{
        std::set<std::string> CheckALPNSupportedProtocols (const std::string& host)
        {
            LOG_TRACE("CheckALPNSupportedProtocols:   Start check...");
            std::unordered_map<std::string, std::vector<unsigned char>> protocols;
            protocols["HTTP/2.0"]    =  { 2, 'h', '2' };
            protocols["HTTP/2.0-14"] =  { 5, 'h', '2', '-', '1', '4' };
            protocols["HTTP/2.0-16"] =  { 5, 'h', '2', '-', '1', '6' };
            protocols["SPDY/1.0"]    =  { 6, 's', 'p', 'd', 'y', '/', '1' };
            protocols["SPDY/2.0"]    =  { 6, 's', 'p', 'd', 'y', '/', '2' };
            protocols["SPDY/3.0"]    =  { 6, 's', 'p', 'd', 'y', '/', '3' };
            protocols["SPDY/3.1"]    =  { 8, 's', 'p', 'd', 'y', '/', '3', '.', '1' };
            protocols["SPDY/4.0"]    =  { 6, 's', 'p', 'd', 'y', '/', '4' };
            protocols["HTTP/1.1"]    =  { 8, 'h', 't', 't', 'p', '/', '1', '.', '1' };

            std::set<std::string> result;
            for (auto&& it : protocols)
            {
                net::SocketSSL sock;
                if (!sock.SetInternalProtocol(it.second.data(), it.second.size())) {
                    return std::set<std::string>();
                }

                sock.Connect(host.c_str());
                if (sock.IsError()) { return std::set<std::string>(); }

                const std::string proto = sock.GetRawSelectedProtocol();
                if (!proto.empty()) {
                    result.emplace(it.first);
                    LOG_INFO("Next protocol: ", proto, '.');
                }
                sock.Close();
            }
            LOG_TRACE("CheckALPNSupportedProtocols:   End check...");
            return result;
        }


        std::set<std::string> CheckSupportedTLSProtocols (const std::string& host)
        {
            LOG_TRACE("CheckSupportedTLSProtocols:   Start check...");
            std::unordered_map<std::string, uint16_t> protocols;
            protocols.emplace(std::make_pair("TLS 1.0", SSL_METHOD_TLS1));
            protocols.emplace(std::make_pair("TLS 1.1", SSL_METHOD_TLS11));
            protocols.emplace(std::make_pair("TLS 1.2", SSL_METHOD_TLS12));
            //protocols.emplace(std::make_pair("TLS 1.3", SSL_METHOD_TLS13));

            std::set<std::string> result;
            for (auto&& it : protocols)
            {
                net::SocketSSL sock(it.second);

                sock.Connect(host.c_str());
                if (sock.IsError()) { return std::set<std::string>(); }

                result.emplace(it.first);
                LOG_INFO("Next protocol: ", it.first, '.');
                sock.Close();
            }
            LOG_TRACE("CheckSupportedTLSProtocols:   End check...");
            return result;
        }

}  // namespace analyzer.
