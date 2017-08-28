#pragma once
#ifndef PROTOCOL_ANALYZER_UTILITIES_HPP
#define PROTOCOL_ANALYZER_UTILITIES_HPP

#include <set>

#include "Socket.hpp"


namespace analyzer::utility
{
    /**
     * @fn std::set<std::string> CheckALPNSupportedProtocols (const std::string &);
     * @brief Function that check the set of supported ALPN protocols.
     * @param [in] host - The domain name or the IP of remote host.
     * @return The set of the supported ALPN protocols.
     */
    std::set<std::string> CheckALPNSupportedProtocols (const std::string & /*host*/);

    /**
     * @fn std::set<std::string> CheckSupportedTLSProtocols (const std::string &);
     * @brief Function that check the set of supported TLS protocols.
     * @param [in] host - The domain name or the IP of remote host.
     * @return The set of the supported TLS protocols.
     */
    std::set<std::string> CheckSupportedTLSProtocols (const std::string & /*host*/);


}  // namespace utility.


#endif  // PROTOCOL_ANALYZER_UTILITIES_HPP
