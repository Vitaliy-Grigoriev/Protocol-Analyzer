// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_NETLINK_INTERFACE_HPP
#define PROTOCOL_ANALYZER_NETLINK_INTERFACE_HPP

#include <list>  // std::list.
#include <linux/rtnetlink.h>  // RTMGRP_*, ifinfomsg, ifaddrmsg, rtmsg, rtattr.

#include "Socket.hpp"  // Socket.
#include "NetworkAddresses.hpp"  // IpAddress, MacAddress.


namespace analyzer::framework::net
{
    struct InterfaceAddresses
    {
        IpAddress ipAddress;
        IpAddress localIpAddress;
        IpAddress anycastIpAddress;
        IpAddress broadcastIpAddress;
    };

    struct InterfaceInfo;

    struct InterfaceRoutesInfo
    {
        IpAddress destinationAddress;
        IpAddress destinationMask;
        IpAddress gateWay;
        IpAddress sourceAddress;

        bool isDefault;
        uint8_t routeScope;
        int32_t routePriority;
        uint32_t interfaceIndex;
        struct InterfaceInfo* interface;

        std::string ToString(void) const noexcept
        {
            std::ostringstream str;

            str << "\tGateWay address: " << gateWay << std::endl;
            str << "\tDestination address: " << destinationAddress << std::endl;
            str << "\tSource address: " << sourceAddress << std::endl;
            str << "\tDestination mask: " << destinationMask << std::endl;
            str << "\tIs default: " << isDefault << std::endl;
            str << "\tPriority: " << routePriority << std::endl << std::endl;

            return str.str();
        }
    };

    struct InterfaceInfo
    {
        std::string interfaceName;
        uint32_t interfaceType;
        uint32_t interfaceIndex;
        uint32_t mtuSize;
        uint8_t interfaceFamily;
        MacAddress macAddress;

        std::list<InterfaceAddresses> ipv4Addresses;
        std::list<InterfaceAddresses> ipv6Addresses;

        InterfaceRoutesInfo * defaultIpv4Route;
        std::list<InterfaceRoutesInfo> ipv4Routes;
        std::list<InterfaceRoutesInfo> ipv6Routes;


        std::string ToString(void) const noexcept
        {
            std::ostringstream str;

            str << "Interface name: " << interfaceName << std::endl;
            str << "Interface index: " << interfaceIndex << std::endl;
            str << "Interface type: " << interfaceType << std::endl;
            str << "MTU size: " << mtuSize << std::endl;
            str << "Interface family: " << uint16_t(interfaceFamily) << std::endl;
            str << "Interface MAC: " << macAddress.ToString() << std::endl;

            str << "IPv4 addresses: " << std::endl;
            for (auto& addr : ipv4Addresses) {
                str << "\tIP address: " << addr.ipAddress << std::endl;
                str << "\tIP local address: " << addr.localIpAddress << std::endl;
                str << "\tIP broadcast address: " << addr.broadcastIpAddress << std::endl;
                str << "\tIP anycast address: " << addr.anycastIpAddress << std::endl << std::endl;
            }
            str << "IPv6 addresses: " << std::endl;
            for (auto& addr : ipv6Addresses) {
                str << "\tIP address: " << addr.ipAddress << std::endl;
                str << "\tIP local address: " << addr.localIpAddress << std::endl;
                str << "\tIP broadcast address: " << addr.broadcastIpAddress << std::endl;
                str << "\tIP anycast address: " << addr.anycastIpAddress << std::endl << std::endl;
            }

            str << "IPv4 routes: " << std::endl;
            for (auto& rtt : ipv4Routes) {
                str << "\tGateWay address: " << rtt.gateWay << std::endl;
                str << "\tDestination address: " << rtt.destinationAddress << std::endl;
                str << "\tSource address: " << rtt.sourceAddress << std::endl;
                str << "\tDestination mask: " << rtt.destinationMask << std::endl;
                str << "\tIs default: " << rtt.isDefault << std::endl;
                str << "\tPriority: " << rtt.routePriority << std::endl << std::endl;
            }
            str << "IPv6 routes: " << std::endl;
            for (auto& rtt : ipv6Routes) {
                str << "\tGateWay address: " << rtt.gateWay << std::endl;
                str << "\tDestination address: " << rtt.destinationAddress << std::endl;
                str << "\tSource address: " << rtt.sourceAddress << std::endl;
                str << "\tDestination mask: " << rtt.destinationMask << std::endl;
                str << "\tIs default: " << rtt.isDefault << std::endl;
                str << "\tPriority: " << rtt.routePriority << std::endl << std::endl;
            }

            return str.str();
        }
    };


#define DEFAULT_NETLINK_TIMEOUT   2
#define DEFAULT_NETLINK_GROUPS   RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV6_IFADDR | RTMGRP_IPV4_ROUTE | RTMGRP_IPV6_ROUTE

    /**
     * @class NetlinkSocket   NetlinkInterface.hpp   "include/framework/NetlinkInterface.hpp"
     * @brief This class defined the interface for work with Netlink Interface.
     */
    class NetlinkSocket final : public Socket
    {
    private:
        uint32_t pid = 0;

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
        ~NetlinkSocket(void) = default;
    };


    InterfaceInfo * GetCorrectInterface (uint32_t /*index*/, uint8_t /*family*/, const std::list<InterfaceInfo> & /*interfaces*/);


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
        bool NetlinkInterfaceParser (void * /*data*/, uint32_t /*length*/, std::list<InterfaceInfo> & /*interfaces*/) const noexcept;

        bool NetlinkAddressParser (void * /*data*/, uint32_t /*length*/, std::list<InterfaceInfo> & /*interfaces*/) const noexcept;

        bool NetlinkRouteParser (void * /*data*/, uint32_t /*length*/, std::list<InterfaceInfo> & /*interfaces*/) const noexcept;

    public:
        NetlinkRequester (NetlinkRequester &&) = delete;
        NetlinkRequester (const NetlinkRequester &) = delete;
        NetlinkRequester & operator= (NetlinkRequester &&) = delete;
        NetlinkRequester & operator= (const NetlinkRequester &) = delete;

        /**
         * @brief Constructor of NetlinkRequester class.
         *
         * @param [in] family - Family of network interface. Default: AF_UNSPEC.
         */
        explicit NetlinkRequester (uint8_t /*family*/ = AF_UNSPEC) noexcept;

        /**
         * @brief Method that returns the list of network interfaces of selected family in the system.
         *
         * @param [in] interfaces - List of InterfaceInfo structures for filling.
         * @return Boolean value that indicates about the status of the request.
         */
        bool GetNetworkInterfaces (std::list<InterfaceInfo> & /*interfaces*/) noexcept;

        /**
         * @brief Method that fills the InterfaceInfo structures by network interface addresses.
         *
         * @param [in] interfaces - List of InterfaceInfo structures which received from 'GetNetworkInterfaces' method.
         * @return Boolean value that indicates about the status of the request.
         */
        bool GetInterfacesAddresses (std::list<InterfaceInfo> & /*interfaces*/) noexcept;

        /**
         * @brief Method that fills the InterfaceInfo structures by network interface routes.
         *
         * @param [in] interfaces - List of InterfaceInfo structures which received from 'GetNetworkInterfaces' method.
         * @return Boolean value that indicates about the status of the request.
         */
        bool GetInterfacesRoutes (std::list<InterfaceInfo> & /*interfaces*/) noexcept;

        /**
         * @brief Default destructor of NetlinkRequester class.
         */
        ~NetlinkRequester(void) = default;
    };

}  // namespace net.

#endif  // PROTOCOL_ANALYZER_NETLINK_INTERFACE_HPP
