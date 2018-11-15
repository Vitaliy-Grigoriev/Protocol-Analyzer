// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_SYSTEM_NETWORK_CONFIGURATION_HPP
#define PROTOCOL_ANALYZER_SYSTEM_NETWORK_CONFIGURATION_HPP

#include <list>  // std::list.
#include <mutex>  // std::mutex.

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
        /**
         * @brief Family of network interfaces and routes.
         */
        uint8_t interfaceFamily = AF_UNSPEC;
        /**
         * @brief Mutex that needs for update network routes.
         */
        std::mutex mutex = { };
        /**
         * @brief List that contain information about all network interfaces.
         */
        std::list<net::InterfaceInformation> networkInterfacesInfo = { };
        /**
         * @brief List that contain information about all network routes.
         */
        std::list<net::RouteInformation> networkRoutesInfo = { };


        /**
         * @brief Method that updates information about system network interfaces and routes.
         *
         * @param [in] family - Family of network interfaces and routes. Default: AF_UNSPEC.
         * @return TRUE - if update succeeds, otherwise - false.
         */
        [[nodiscard]]
        bool Update (uint8_t /*family*/ = AF_UNSPEC) noexcept;

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
         * @brief Method that initializes information about system network interfaces and routes.
         *
         * @param [in] family - Family of network interfaces and routes. Default: AF_UNSPEC.
         * @return TRUE - if initialization succeeds, otherwise - false.
         */
        [[nodiscard]]
        bool Initialize (uint8_t /*family*/ = AF_UNSPEC) noexcept;

        /**
         * @brief Method that returns the best network route for selected IP address.
         *
         * @param [in] ip - IP address in IPv4 or IPv6 representation.
         * @return Pointer to best network route for selected IP address.
         *
         * @todo Add support for IPv6 routing.
         */
        [[nodiscard]]
        const net::RouteInformation * GetBestRouteForIpAddress (const net::IpAddress & /*ip*/) noexcept;

        /**
         * @brief Method that returns the interface by index and network family.
         *
         * @param [in] index - Index of the network interface.
         * @param [in] family - Network family of the network interface.
         * @return Constant pointer to the found network interface or nullptr.
         */
        [[nodiscard]]
        const net::InterfaceInformation * GetInterface (uint32_t /*index*/, uint8_t /*family*/) noexcept;

        /**
         * @brief Method that returns information about system interfaces and routes in string format.
         *
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
