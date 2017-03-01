#pragma once
#ifndef HTTP2_ANALYZER_UTILITIES_HPP
#define HTTP2_ANALYZER_UTILITIES_HPP

#include <set>

#include "Socket.hpp"


namespace analyzer {
    /**
     * @namespace utility
     * @brief The namespace that contain definition of various utilities.
     */
    namespace utility {
        /**
         * @fn std::set<std::string> CheckALPNSupportedProtocols (const std::string &);
         * @brief Function that check the set of supported ALPN protocols.
         * @param host - The domain name or the IP of remote host.
         * @return The set of the supported ALPN protocols.
         */
        std::set<std::string> CheckALPNSupportedProtocols (const std::string & /*host*/);



    }  // namespace utility.
}  // namespace analyzer.


#endif  // HTTP2_ANALYZER_UTILITIES_HPP
