// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_NETLINK_INTERFACE_HPP
#define PROTOCOL_ANALYZER_NETLINK_INTERFACE_HPP

#include "Socket.hpp"  // Socket.
#include "NetworkTypes.hpp"  // IpAddress, MacAddress, InterfaceInformation.


namespace analyzer::framework::net
{
#define DEFAULT_NETLINK_TIMEOUT   2
#define DEFAULT_NETLINK_GROUPS   (RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV6_IFADDR | RTMGRP_IPV4_ROUTE | RTMGRP_IPV6_ROUTE)

    /**
     * @class NetlinkSocket   NetlinkInterface.hpp   "include/framework/NetlinkInterface.hpp"
     * @brief This class defined an interface for work with Netlink Interface.
     */
    class NetlinkSocket final : public Socket
    {
    private:
        uint32_t uniquePid = 0;

    public:
        NetlinkSocket (NetlinkSocket &&) = delete;
        NetlinkSocket (const NetlinkSocket &) = delete;
        NetlinkSocket & operator= (NetlinkSocket &&) = delete;
        NetlinkSocket & operator= (const NetlinkSocket &) = delete;

        /**
         * @brief Constructor of NetlinkSocket class.
         *
         * @param [in] timeout - Timeout (in seconds) during which data will be read from the socket. Default: DEFAULT_NETLINK_TIMEOUT.
         */
        explicit NetlinkSocket (uint32_t /*timeout*/ = DEFAULT_NETLINK_TIMEOUT) noexcept;

        /**
         * @brief Method that associates a Netlink socket PID with a Netlink multicast groups.
         *
         * @param [in] groups - Multicast groups for listening. Default: DEFAULT_NETLINK_GROUPS.
         * @return Boolean value that indicates the binding status.
         */
        bool Bind (uint32_t /*groups*/ = DEFAULT_NETLINK_GROUPS) noexcept;

        /**
         * @brief Default destructor of NetlinkSocket class.
         */
        ~NetlinkSocket(void) noexcept;
    };



    /**
     * @enum INTERFACE_TYPES
     * @brief Type of network interface.
     */
    enum INTERFACE_TYPES : uint16_t
    {
        INTERFACE_TYPE_ANY = 0x01,
        INTERFACE_TYPE_ETHERNET = 0x02,
        INTERFACE_TYPE_IEEE80211 = 0x04,
        INTERFACE_TYPE_LOOPBACK = 0x08,
        INTERFACE_TYPE_TUNNEL = 0x10,
        INTERFACE_TYPE_TUNNEL6 = 0x20,
        INTERFACE_TYPE_UNSPECIFIED_TUNNEL = 0x40
    };
#define DEFAULT_INTERFACE_TYPES   (INTERFACE_TYPE_ETHERNET | INTERFACE_TYPE_IEEE80211 | INTERFACE_TYPE_LOOPBACK | INTERFACE_TYPE_TUNNEL | INTERFACE_TYPE_TUNNEL6 | INTERFACE_TYPE_UNSPECIFIED_TUNNEL)

    /**
     * @enum ROUTE_TYPES
     * @brief Type of network route.
     */
    enum ROUTE_TYPES : uint8_t
    {
        ROUTE_TYPE_ANY = 0x01,
        ROUTE_TYPE_UNICAST = 0x02,
        ROUTE_TYPE_BROADCAST = 0x04,
        ROUTE_TYPE_MULTICAST = 0x08,
        ROUTE_TYPE_ANYCAST = 0x10,
        ROUTE_TYPE_LOCAL = 0x20
    };

    /**
     * @class NetlinkRequester   NetlinkInterface.hpp   "include/framework/NetlinkInterface.hpp"
     * @brief This class defined the interface that works with linux kernel socket interface.
     */
    class NetlinkRequester
    {
    private:
        /**
         * @brief Pointer to Netlink socket for interact with kernel space.
         */
        std::unique_ptr<NetlinkSocket> sock = nullptr;
        /**
         * @brief Variable that indicates the sequence number in Netlink header.
         */
        uint32_t sequenceNumber = 0;
        /**
         * @brief Variable that indicates the family of the network interfaces.
         */
        uint8_t interfaceFamily = AF_UNSPEC;

    protected:
        /**
         * @brief Method that parses Netlink-Interface response and fills InterfaceInformation structures.
         *
         * @param [in] data - Netlink-Interface response message on Netlink-Interface request.
         * @param [in] length - Length of the inputted Netlink-Interface response.
         * @param [out] interfaces - List of InterfaceInformation structures for filling.
         * @param [in] types - Type of the network interfaces in INTERFACE_TYPES type.
         * @param [in] onlyRunning - Boolean flag that indicates the working status of received interfaces.
         * @return TRUE - if Netlink-Interface message was successfully parsed, otherwise - FALSE.
         */
        bool NetlinkInterfaceParser (void * /*data*/, uint32_t /*length*/, std::list<InterfaceInformation> & /*interfaces*/, uint16_t /*types*/, bool /*onlyRunning*/) const noexcept;

        /**
         * @brief Method that parses Netlink-Address response and fills InterfaceInformation structures.
         *
         * @param [in] data - Netlink-Address response message on Netlink-Address request.
         * @param [in] length - Length of the inputted Netlink-Address response.
         * @param [out] addresses - List of InterfaceInformation structures for filling.
         * @param [in] notEnrich - Boolean flag that indicates about regarding the addition to the collection of new interfaces.
         * @return TRUE - if Netlink-Address message was successfully parsed, otherwise - FALSE.
         */
        bool NetlinkAddressParser (void * /*data*/, uint32_t /*length*/, std::list<InterfaceInformation> & /*addresses*/, bool /*notEnrich*/) const noexcept;

        /**
         * @brief Method that parses Netlink-Route response and fills RouteInformation structures.
         *
         * @param [in] data - Netlink-Route response message on Netlink-Route request.
         * @param [in] length - Length of the inputted Netlink-Route response.
         * @param [out] routes - List of RouteInformation structures for filling.
         * @param [in] types - Type of the network routes in ROUTE_TYPES type.
         * @return TRUE - if Netlink-Route message was successfully parsed, otherwise - FALSE.
         */
        bool NetlinkRouteParser (void * /*data*/, uint32_t /*length*/, std::list<RouteInformation> & /*routes*/, uint8_t /*types*/) const noexcept;

    public:
        NetlinkRequester (NetlinkRequester &&) = delete;
        NetlinkRequester (const NetlinkRequester &) = delete;
        NetlinkRequester & operator= (NetlinkRequester &&) = delete;
        NetlinkRequester & operator= (const NetlinkRequester &) = delete;

        /**
         * @brief Constructor of NetlinkRequester class.
         *
         * @param [in] family - Family of network interfaces and routes. Default: AF_UNSPEC.
         */
        explicit NetlinkRequester (uint8_t /*family*/ = AF_UNSPEC) noexcept;

        /**
         * @brief Method that returns the list of network interfaces in the system.
         *
         * @param [out] interfaces - List of InterfaceInfo structures for filling.
         * @param [in] types - Type of the network interfaces in INTERFACE_TYPES type. Default: DEFAULT_INTERFACE_TYPES.
         * @param [in] onlyRunning - Boolean flag that indicates the working status of received interfaces. Default: TRUE.
         * @return Boolean value that indicates about the status of the request.
         */
        bool GetNetworkInterfaces (std::list<InterfaceInformation> & /*interfaces*/, uint16_t /*types*/ = DEFAULT_INTERFACE_TYPES, bool /*onlyRunning*/ = true) noexcept;

        /**
         * @brief Method that fills the InterfaceInformation structures by network interfaces addresses.
         *
         * @param [out] addresses - List of InterfaceInformation structures.
         * @param [in] notEnrich - Boolean flag that indicates about regarding the addition to the collection of new interfaces. Default: FALSE.
         * @return Boolean value that indicates about the status of the request.
         */
        bool GetInterfacesAddresses (std::list<InterfaceInformation> & /*addresses*/, bool /*notEnrich*/ = false) noexcept;

        /**
         * @brief Method that fills the RouteInformation structures by network routes.
         *
         * @param [out] routes - List of RouteInformation structures.
         * @param [in] types - Type of the network routes in ROUTE_TYPES type. Default: ROUTE_TYPE_ANY.
         * @return Boolean value that indicates about the status of the request.
         */
        bool GetRoutes (std::list<RouteInformation> & /*routes*/, uint8_t /*types*/ = ROUTE_TYPE_ANY) noexcept;

        /**
         * @brief Default destructor of NetlinkRequester class.
         */
        ~NetlinkRequester(void) = default;
    };

}  // namespace net.

#endif  // PROTOCOL_ANALYZER_NETLINK_INTERFACE_HPP
