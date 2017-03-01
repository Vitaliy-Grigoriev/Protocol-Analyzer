// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../include/analyzer/Utilities.hpp"

#include <vector>
#include <unordered_map>

namespace analyzer {
    namespace utility {

        std::set<std::string> CheckALPNSupportedProtocols (const std::string& host)
        {
            log::DbgLog("[*] CheckALPNSupportedProtocols:   Start check...");
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
                if (proto.size() > 0) {
                    result.emplace(it.first);
                    log::DbgLog("[+] Next protocol: ", proto, ".");
                }
                sock.Close();
            }
            log::DbgLog("[*] CheckALPNSupportedProtocols:   End check...");
            return result;
        }

    }  // namespace utility.
}  // namespace analyzer.
