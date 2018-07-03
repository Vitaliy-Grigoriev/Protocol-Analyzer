// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#ifndef PROTOCOL_ANALYZER_UTILITIES_HPP
#define PROTOCOL_ANALYZER_UTILITIES_HPP

#include <set>
#include <string>
#include <openssl/opensslv.h>  // For using OPENSSL_VERSION_NUMBER define.


namespace analyzer::utility
{
#if (defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER >= 0x1000208fL)  // If OPENSSL version more then 1.0.2h.

    /**
     * @fn std::set<std::string> CheckALPNSupportedProtocols (const std::string &);
     * @brief Function that checks the set of supported ALPN protocols.
     * @param [in] host - The domain name or the IP of remote host.
     * @return The set of the supported ALPN protocols.
     */
    std::set<std::string> CheckALPNSupportedProtocols (const std::string & /*host*/);

#endif

    /**
     * @fn std::set<std::string> CheckSupportedTLSProtocols (const std::string &);
     * @brief Function that checks the set of supported TLS protocols.
     * @param [in] host - The domain name or the IP of remote host.
     * @return The set of the supported TLS protocols.
     */
    std::set<std::string> CheckSupportedTLSProtocols (const std::string & /*host*/);


}  // namespace utility.


#endif  // PROTOCOL_ANALYZER_UTILITIES_HPP
