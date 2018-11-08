// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_SYSTEM_NETWORK_CONFIGURATION_HPP
#define PROTOCOL_ANALYZER_SYSTEM_NETWORK_CONFIGURATION_HPP

#include <list>  // std::list.

#include "NetworkTypes.hpp"  // net::InterfaceInformation.


namespace analyzer::framework::system
{
    /**
     * @class SystemNetworkConfiguration   SystemNetworkConfiguration.hpp   "include/framework/SystemNetworkConfiguration.hpp"
     * @brief This class defined an interface for getting information about system interfaces and routes.
     */
    class SystemNetworkConfiguration
    {
    private:
        std::list<net::InterfaceInformation> networkInterfacesInfo = { };
        std::list<net::RouteInformation> networkRoutesInfo = { };

    public:
        SystemNetworkConfiguration (SystemNetworkConfiguration &&) = delete;
        SystemNetworkConfiguration (const SystemNetworkConfiguration &) = delete;
        SystemNetworkConfiguration & operator= (SystemNetworkConfiguration &&) = delete;
        SystemNetworkConfiguration & operator= (const SystemNetworkConfiguration &) = delete;

        /**
         * @brief Constructor of SystemNetworkConfiguration class.
         */
        SystemNetworkConfiguration(void) noexcept = default;

        /**
         * @brief Method that initialize information about system network interfaces and routes.
         *
         * [in] family - Family of network interfaces and routes. Default: AF_UNSPEC.
         *
         * @return TRUE - if initialization succeeds, otherwise - false.
         */
        bool Initialize (uint8_t /*family*/ = AF_UNSPEC) noexcept;

        /**
         * @brief Method that returns information about system interfaces and routes in string format.
         * @return Information about system interfaces and routes.
         */
        std::string ToString(void) const noexcept;

        /**
         * @brief Default destructor of SystemNetworkConfiguration class.
         */
        ~SystemNetworkConfiguration(void) = default;
    };

}  // namespace system.

#endif  // PROTOCOL_ANALYZER_SYSTEM_NETWORK_CONFIGURATION_HPP
