// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include <iostream>

#include "../include/framework/GlobalInfo.hpp"
#include "../include/framework/AnalyzerApi.hpp"

using namespace analyzer::framework;


/**
 * @class SocketCallbackFunctorBeforeSendImpl   test_callbacks.cpp   "test/test_callbacks.cpp"
 * @brief This class-functor implemented of SocketCallbackFunctorBeforeSend functor type.
 */
class SocketCallbackFunctorBeforeSendImpl final : public callbacks::SocketCallbackFunctorBeforeSend
{
public:
    SocketCallbackFunctorBeforeSendImpl(void) = default;
    ~SocketCallbackFunctorBeforeSendImpl(void) = default;

    /**
     * @fn void SocketCallbackFunctorBeforeSendImpl::operator() (char *, std::size_t *) const noexcept override;
     * @brief Function call operator.
     * @param [in] data - Pointer to the raw data.
     * @param [in] length - Pointer to the length of the inputted raw data.
     */
    void operator() (char* data, std::size_t* length) const noexcept final
    {
        std::cout << data << std::endl;
    }
};


/**
 * @class SocketCallbackFunctorAfterReceiveImpl   test_callbacks.cpp   "test/test_callbacks.cpp"
 * @brief This class-functor implemented of SocketCallbackFunctorAfterReceive functor type.
 */
class SocketCallbackFunctorAfterReceiveImpl final : public callbacks::SocketCallbackFunctorAfterReceive
{
public:
    SocketCallbackFunctorAfterReceiveImpl(void) = default;
    ~SocketCallbackFunctorAfterReceiveImpl(void) = default;

    /**
     * @fn void SocketCallbackFunctorAfterReceiveImpl::operator() (const char *, const std::size_t) const noexcept;
     * @brief Function call operator of SocketCallbackFunctorAfterReceiveImpl class-functor.
     * @param [in] data - Pointer to the constant raw data.
     * @param [in] length - Length of the inputted raw data.
     */
    void operator() (const char* data, const std::size_t length) const noexcept
    {
        std::cout << "Functor: Receive data length: " << length << std::endl;
    }
};


int32_t main (int32_t size, char** data)
{
    log::Logger::Instance().SwitchLoggingEngine();
    log::Logger::Instance().SetLogLevel(log::LEVEL::TRACE);

    storage::GI.SetCallback(new SocketCallbackFunctorBeforeSendImpl(), modules::MODULE_SOCKET, callbacks::MODULE_SOCKET_BEFORE_SEND_TCP);
    storage::GI.SetCallback(new SocketCallbackFunctorAfterReceiveImpl(), modules::MODULE_SOCKET, callbacks::MODULE_SOCKET_AFTER_RECEIVE_TCP);

    auto sock = system::allocMemoryForObject<net::Socket>();
    if (sock->Connect("clang.llvm.org") == false) {
        std::cout << "[error] Connection fail..." << std::endl;
        return EXIT_FAILURE;
    }

    const char buff[] = "GET / HTTP/1.1\r\nHost: clang.llvm.org\r\nConnection: keep-alive\r\nAccept: */*\r\nDNT: 1\r\n\r\n";
    sock->Send(buff, sizeof(buff));

    char buffReceive[4096] = { };
    sock->Recv(buffReceive, sizeof(buffReceive));
    sock->Close();
    return EXIT_SUCCESS;
}
