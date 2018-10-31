// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include "../../include/framework/Socket.hpp"


namespace analyzer::framework::net
{
    /**
     * @fn inline static std::string CheckSSLErrors() noexcept;
     * @brief Check SSL library error.
     * @return The SSL library error in string form.
     */
    inline static std::string CheckSSLErrors(void) noexcept
    {
        const std::size_t err = ERR_get_error();
        if (err != 0) { return ERR_error_string(err, nullptr); }
        return std::string("NO ANY ERROR.");
    }


    SocketSSL::SocketSSL (const uint16_t method, const char* ciphers, const uint32_t timeout) noexcept
            : Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP, timeout)
    {
        if (method >= NUMBER_OF_CTX) {
            LOG_ERROR("SocketSSL.SocketSSL [", fd,"]: SSL input protocol type is invalid.");
            Socket::CloseAfterError();
            return;
        }

        ssl = SSL_new(context.Get(method));
        if (ssl == nullptr) {
            LOG_ERROR("SocketSSL.SocketSSL [", fd,"]: In function 'SSL_new' - ", CheckSSLErrors());
            SSLCloseAfterError();
            return;
        }

        if (ciphers != nullptr && SSL_set_cipher_list(ssl, ciphers) == 0)
        {
            LOG_ERROR("SocketSSL.SocketSSL [", fd,"]: In function 'SSL_set_cipher_list' - ", CheckSSLErrors());
            SSLCloseAfterError();
            return;
        }

        // All errors processing OpenSSL library.
        SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

        // Create new I/O object.
        bio = BIO_new_socket(fd, BIO_NOCLOSE);
        if (bio == nullptr) {
            LOG_ERROR("SocketSSL.SocketSSL [", fd,"]: In function 'BIO_new_socket' - ", CheckSSLErrors());
            SSLCloseAfterError();
            return;
        }

        // Set non-blocking mode for bio.
        BIO_set_nbio(bio, 1);
        // Assign bio to ssl object.
        SSL_set_bio(ssl, bio, bio);
        // Set client mode for ssl object.
        SSL_set_connect_state(ssl);
    }


    // Connecting to external host.
    bool SocketSSL::Connect (const char* host, uint16_t port)
    {
        if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
            LOG_ERROR("SocketSSL: Socket is invalid.");
            SSLCloseAfterError();
            return false;
        }

        SetServerNameIndication(host);
        if (Socket::Connect(host, port) == false || DoHandshakeSSL() == false)
        {
            SSLCloseAfterError();
            return false;
        }

#if defined(DEBUG)
        uint32_t length = 0;
        const unsigned char* sessionID = SSL_SESSION_get_id(GetSessionSSL(), &length);
        if (sessionID != nullptr && length > 0)
        {
            LOG_TRACE("SocketSSL.Connect [", fd,"]: SSL session established. Session ID - ", common::text::getHexString(sessionID, length), '.');
        }
#endif
        return true;
    }


    bool SocketSSL::Send (const char* data, const std::size_t length) noexcept
    {
        if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
            LOG_ERROR("SocketSSL.Send: Socket is invalid.");
            SSLCloseAfterError(); return false;
        }
        LOG_TRACE("SocketSSL.Send [", fd,"]: Sending data to '", exHost, "'...");

        std::size_t idx = 0;
        while (idx != length)
        {
            const int32_t result = SSL_write(ssl, &data[idx], static_cast<int32_t>(length));
            const std::size_t err = ERR_get_error();
            if (err != 0) {
                LOG_ERROR("SocketSSL.Send [", fd,"]: In function 'SSL_write' - ", ERR_error_string(err, nullptr));
                SSLCloseAfterError(); return false;
            }

            if (result <= 0)
            {
                if (BIO_should_retry(bio) != 0) {
                    if (IsReadyForSend(1500) == false) { SSLCloseAfterError(); return false; }
                    continue;
                }
                LOG_ERROR("SocketSSL.Send [", fd,"]: In function 'SSL_write' - ", CheckSSLErrors());
                SSLCloseAfterError(); return false;
            }
            idx += static_cast<std::size_t>(result);
        }

        LOG_TRACE("SocketSSL.Send [", fd,"]: Sending data to '", exHost, "' is success:  ", idx, " bytes.");
        return (idx == length);
    }


    int32_t SocketSSL::Recv (char* data, std::size_t length, const bool noWait)
    {
        using std::chrono::system_clock;

        if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
            LOG_ERROR("SocketSSL: Socket is invalid.");
            SSLCloseAfterError(); return -1;
        }
        LOG_TRACE("SocketSSL.Recv [", fd,"]: Receiving data from '", exHost, "'...");
        const system_clock::time_point limit = system_clock::now() + GetTimeout();

        std::size_t idx = 0;
        while (length > 0 && system_clock::now() < limit)
        {
            const int32_t result = SSL_read(ssl, &data[idx], static_cast<int32_t>(length));
            const std::size_t err = ERR_get_error();
            if (err != 0) {
                LOG_ERROR("SocketSSL.Recv [", fd,"]: In function 'SSL_read' - ", ERR_error_string(err, nullptr));
                SSLCloseAfterError(); return -1;
            }

            if (result == 0) { break; }
            if (result < 0)
            {
                if (BIO_should_retry(bio) != 0)
                {
                    if (IsReadyForRecv(1500) == false) {
                        if (idx == 0) { SSLCloseAfterError(); return -1; }
                        break;  // In this case no error.
                    }
                    continue;
                }
                LOG_ERROR("SocketSSL.Recv [", fd,"]: In function 'SSL_read' - ", CheckSSLErrors());
                SSLCloseAfterError(); return -1;
            }

            length -= static_cast<std::size_t>(result);
            idx += static_cast<std::size_t>(result);
            if (noWait == true) { break; }
        }

        LOG_TRACE("SocketSSL.Recv [", fd,"]: Receiving data from '", exHost, "' is success:  ", idx, " bytes.");
        return static_cast<int32_t>(idx);
    }


    int32_t SocketSSL::RecvToEnd (char* data, std::size_t length) noexcept
    {
        using std::chrono::system_clock;

        if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
            LOG_ERROR("SocketSSL.RecvToEnd: Socket is invalid.");
            SSLCloseAfterError(); return -1;
        }
        LOG_TRACE("SocketSSL.RecvToEnd [", fd,"]: Receiving data from '", exHost, "'...");
        const system_clock::time_point limit = system_clock::now() + GetTimeout();

        std::size_t idx = 0;
        while (length > 0 && IsReadyForRecv(1500) == true && system_clock::now() < limit)
        {
            const int32_t result = SSL_read(ssl, &data[idx], static_cast<int32_t>(length));
            const std::size_t err = ERR_get_error();
            if (err != 0) {
                LOG_ERROR("SocketSSL.RecvToEnd [", fd,"]: In function 'SSL_read' - ", ERR_error_string(err, nullptr));
                SSLCloseAfterError(); return -1;
            }

            if (result == 0) { break; }
            if (result < 0) {
                if (BIO_should_retry(bio) != 0) { continue; }
                LOG_ERROR("SocketSSL.RecvToEnd [", fd,"]: In function 'SSL_read' - ", CheckSSLErrors());
                CloseAfterError(); return -1;
            }

            length -= static_cast<std::size_t>(result);
            idx += static_cast<std::size_t>(result);
        }

        LOG_TRACE("SocketSSL.RecvToEnd [", fd,"]: Receiving data from '", exHost, "' is success:  ", idx, " bytes.");
        return static_cast<int32_t>(idx);
    }


    bool SocketSSL::DoHandshakeSSL(void)
    {
        if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
            LOG_ERROR("SocketSSL.DoHandshakeSSL: Socket is invalid.");
            SSLCloseAfterError();
            return false;
        }
        LOG_TRACE("SocketSSL.DoHandshakeSSL [", fd,"]: Doing handshake...");

        while (true)
        {
            const int32_t result = SSL_do_handshake(ssl);
            const std::size_t err = ERR_get_error();
            if (err != 0) {
                LOG_ERROR("SocketSSL.DoHandshakeSSL [", fd,"]: In function 'SSL_write' - ", ERR_error_string(err, nullptr));
                break;
            }

            if (result == 1) {
                LOG_TRACE("SocketSSL.DoHandshakeSSL [", fd,"]: Handshake to '", exHost, "' is success.");
                return true;
            }
            if (result <= 0)
            {
                if (BIO_should_retry(bio) != 0) {
                    if (IsReadyForSend(1500) == false) { SSLCloseAfterError(); break; }
                    continue;
                }
                LOG_ERROR("SocketSSL.DoHandshakeSSL [", fd,"]: Handshake failed - ", CheckSSLErrors());
                break;
            }
        }
        return false;
    }


    // Set Server Name Indication to TLS extension.
    bool SocketSSL::SetServerNameIndication (const std::string& serverName) const noexcept
    {
        if (SSL_set_tlsext_host_name(ssl, const_cast<char*>(serverName.c_str())) == 0) {
            LOG_ERROR("SocketSSL.SetServerNameIndication [", fd,"]: In function 'SSL_set_tlsext_host_name' - ", CheckSSLErrors());
            return false;
        }
        LOG_TRACE("SocketSSL.SetServerNameIndication [", fd,"]: Setting SNI extension is success.");
        return true;
    }


    std::vector<std::string> SocketSSL::GetCiphersList(void) const noexcept
    {
        std::vector<std::string> result;
        STACK_OF(SSL_CIPHER)* ciphers_list = SSL_get_ciphers(ssl);
        const int32_t size = sk_SSL_CIPHER_num(ciphers_list);

        result.reserve(static_cast<uint32_t>(size));
        for (int32_t idx = 0; idx < size; ++idx)
        {
            const SSL_CIPHER* cipher = sk_SSL_CIPHER_value(ciphers_list, idx);
            result.emplace_back(SSL_CIPHER_get_name(cipher));
        }
        return result;
    }


    bool SocketSSL::SetOnlySecureCiphers(void) noexcept
    {
        if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
            LOG_ERROR("SocketSSL.SetOnlySecureCiphers: Socket is invalid.");
            SSLCloseAfterError();
            return false;
        }

        std::string secure;
        const std::vector<std::string> current = GetCiphersList();
        for (auto&& it : current)
        {
            if (it.find("SRP") == std::string::npos && it.find("DH-") == std::string::npos      &&
                it.find("RC4") == std::string::npos && it.find("CAMELLIA") == std::string::npos &&
                it.find("MD5") == std::string::npos && it.find("SEED") == std::string::npos     &&
                it.find("DES") == std::string::npos && it.find("PSK") == std::string::npos      &&
                it.find("RSA") == std::string::npos /* RSA - ROBOT attack. */)
            {
                secure += it + ';';
            }
        }

        if (secure.empty() == true || SSL_set_cipher_list(ssl, secure.c_str()) == 0) {
            LOG_ERROR("SocketSSL.SetOnlySecureCiphers [", fd,"]: Setting only secure ciphers failed.");
            return false;
        }
        LOG_TRACE("SocketSSL.SetOnlySecureCiphers [", fd,"]: Setting only secure ciphers is success.");
#if defined(DEBUG)
        std::ostringstream buff;
        const auto result = common::text::splitInPlace(secure, ';');
        std::copy(result.begin(), result.end(), std::ostream_iterator<std::string_view>(buff, "\n|-----------------\n|"));
        LOG_TRACE("SocketSSL.SetOnlySecureCiphers [", fd,"]: Setting following ciphers:\n|-----------------\n|", buff.str());
#endif
        return true;
    }


    void SocketSSL::Shutdown (int32_t how) const
    { // Encryption alert (21).
        if (ssl != nullptr)
        {
            switch (how)
            {
                case SHUT_RDWR: SSL_set_shutdown(ssl, SSL_SENT_SHUTDOWN | SSL_RECEIVED_SHUTDOWN);
                    break;
                case SHUT_RD: SSL_set_shutdown(ssl, SSL_RECEIVED_SHUTDOWN);
                    break;
                case SHUT_WR: SSL_set_shutdown(ssl, SSL_SENT_SHUTDOWN);
                    break;
                default:
                    LOG_ERROR("SocketSSL.Shutdown [", fd,"]: Unknown mode for shutdown the SSL connection.");
                    SSL_set_shutdown(ssl, SSL_SENT_SHUTDOWN | SSL_RECEIVED_SHUTDOWN);
                    how = SHUT_RDWR;
            }
            const int32_t res = SSL_shutdown(ssl);
            if (res == 0) { SSL_shutdown(ssl); }
            else if (res < 0) { LOG_ERROR("SocketSSL.Shutdown [", fd,"]: In function 'SSL_shutdown' - ", CheckSSLErrors()); }
        }
        Socket::Shutdown(how);
    }


    // Close connection.
    void SocketSSL::Close(void)
    {
        if (ssl != nullptr) { SSL_free(ssl); ssl = nullptr; }
        Socket::Close();
    }

    // Returns true when the handshake is complete.
    bool SocketSSL::IsHandshakeReady(void) const
    {
        return (ssl != nullptr && SSL_is_init_finished(ssl) != 0);
    }


#if (defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER >= 0x1000208fL)  // If OPENSSL version more then 1.0.2h.

    // Use ALPN protocol TLS extension to change the set of application protocols.
    bool SocketSSL::SetInternalProtocol (const unsigned char* proto, std::size_t length) noexcept
    {
        if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
            LOG_ERROR("SocketSSL.SetInternalProtocol: Socket is invalid.");
            SSLCloseAfterError();
            return false;
        }

        if (SSL_set_alpn_protos(ssl, &proto[0], static_cast<uint32_t>(length)) == 0) {
            LOG_TRACE("SocketSSL.SetInternalProtocol [", fd,"]: Setting ALPN protocol extension is success.");
            return true;
        }
        LOG_ERROR("SocketSSL.SetInternalProtocol [", fd,"]: In function 'SetInternalProtocol' - ", CheckSSLErrors());
        return false;
    }


    // Get selected ALPN protocol by server in string type.
    std::string SocketSSL::GetRawSelectedProtocol(void) const noexcept
    {
        if (IsHandshakeReady() == false) { return std::string(); }
        uint32_t length = 0;
        const unsigned char* proto = nullptr;

        SSL_get0_alpn_selected(ssl, &proto, &length);
        if (length > 0) {
            return std::string(reinterpret_cast<const char*>(proto), length);
        }
        return std::string();
    }


    // Get selected ALPN protocol by server.
    protocols::http::HTTP_VERSION SocketSSL::GetSelectedProtocol(void) const noexcept
    {
        using protocols::http::HTTP_VERSION;
        if (IsHandshakeReady() == false) { return HTTP_VERSION::UNKNOWN; }

        const std::string proto = GetRawSelectedProtocol();
        if (proto.empty() == false)
        {
            if (proto == "h2") { return HTTP_VERSION::HTTP2_0; }
            return HTTP_VERSION::HTTP1_1;
        }
        return HTTP_VERSION::UNKNOWN;
    }


    bool SocketSSL::SetHttp_2_0_OnlyProtocol(void) noexcept
    {
        const unsigned char proto[] = { 2, 'h', '2' };
        return SetInternalProtocol(proto, sizeof(proto));
    }


    bool SocketSSL::SetHttp_1_1_OnlyProtocol(void) noexcept
    {
        const unsigned char proto[] = { 8, 'h', 't', 't', 'p', '/', '1', '.', '1' };
        return SetInternalProtocol(proto, sizeof(proto));
    }


    bool SocketSSL::SetHttpProtocols(void) noexcept
    {
        const unsigned char proto[] = {
                2, 'h', '2',
                8, 'h', 't', 't', 'p', '/', '1', '.', '1'
        };
        return SetInternalProtocol(proto, sizeof(proto));
    }

#endif


    // Get current timeout of the SSL session.
    std::size_t SocketSSL::GetSessionTimeout(void) const noexcept
    {
        return static_cast<std::size_t>(SSL_get_timeout(SSL_get_session(ssl)));
    }


    // Get selected cipher name in ssl connection.
    std::string SocketSSL::GetSelectedCipherName(void) const noexcept
    {
        if (IsHandshakeReady() == false) { return std::string(); }

        const SSL_CIPHER* cipher = SSL_get_current_cipher(ssl);
        return std::string(SSL_CIPHER_get_name(cipher));
    }

    // Cleaning after error.
    void SocketSSL::SSLCloseAfterError(void)
    {
        if (ssl != nullptr) { SSL_free(ssl); ssl = nullptr; }
        Socket::CloseAfterError();
    }

    // Destructor.
    SocketSSL::~SocketSSL(void)
    {
        if (ssl != nullptr) { SSL_free(ssl); ssl = nullptr; }
        Socket::Close();
    }




    SSLContext::SSLContext(void) noexcept
    {
        // Initialize readable error messages.
        ERR_load_BIO_strings();
        SSL_load_error_strings();
        ERR_load_crypto_strings();
        // Initialize SSL library.
        SSL_library_init();
        // Initialize SSL algorithms, ciphers and digests.
        OpenSSL_add_all_ciphers();
        OpenSSL_add_all_digests();
        OpenSSL_add_all_algorithms();

        ctx[SSL_METHOD_TLS1] = SSL_CTX_new( TLSv1_client_method() );
        if (ctx[SSL_METHOD_TLS1] == nullptr) {
            LOG_FATAL("SSLContext: In function 'SSL_CTX_new (TLSv1.0)' - ", CheckSSLErrors());
            std::terminate();
        }
        ctx[SSL_METHOD_TLS11] = SSL_CTX_new( TLSv1_1_client_method() );
        if (ctx[SSL_METHOD_TLS11] == nullptr) {
            LOG_FATAL("SSLContext: In function 'SSL_CTX_new (TLSv1.1)' - ", CheckSSLErrors());
            std::terminate();
        }
        ctx[SSL_METHOD_TLS12] = SSL_CTX_new( TLSv1_2_client_method() );
        if (ctx[SSL_METHOD_TLS12] == nullptr) {
            LOG_FATAL("SSLContext: In function 'SSL_CTX_new (TLSv1.2)' - ", CheckSSLErrors());
            std::terminate();
        }
        LOG_INFO("SSLContext: Initialize SSL library is success.");
    }

    SSL_CTX* SSLContext::Get (const std::size_t method) const noexcept
    {
        if (method < NUMBER_OF_CTX) { return ctx[method]; }
        return nullptr;
    }

    SSLContext::~SSLContext(void) noexcept
    {
        SSL_CTX_free( ctx[SSL_METHOD_TLS1] );
        SSL_CTX_free( ctx[SSL_METHOD_TLS11] );
        SSL_CTX_free( ctx[SSL_METHOD_TLS12] );
        //SSL_CTX_free( ctx[SSL_METHOD_TLS13] );
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"

    // Initialized context.
    SSLContext SocketSSL::context;

#pragma clang diagnostic pop


}  // namespace net.
