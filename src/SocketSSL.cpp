// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../include/analyzer/Socket.hpp"


namespace analyzer::net
{
    /**
     * @fn inline static std::string CheckSSLErrors(void) noexcept;
     * @brief Check SSL library error.
     * @return The SSL library error in string form.
     */
    inline static std::string CheckSSLErrors(void) noexcept
    {
        const std::size_t err = ERR_get_error();
        if (err != 0) { return ERR_error_string(err, nullptr); }
        return std::string("NO ANY ERROR.");
    }


    SocketSSL::SocketSSL (const uint16_t method, const char* ciphers, const uint32_t timeout)
            : Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP, timeout)
    {
        if (method >= NUMBER_OF_CTX) {
            LOG_ERROR("SocketSSL.SocketSSL [", fd,"]: SSL input protocol type is invalid.");
            Socket::CloseAfterError(); return;
        }

        ssl = SSL_new(context.Get(method));
        if (ssl == nullptr) {
            LOG_ERROR("SocketSSL.SocketSSL [", fd,"]: In function 'SSL_new' - ", CheckSSLErrors());
            SSLCloseAfterError(); return;
        }

        if (ciphers != nullptr)
        {
            if (SSL_set_cipher_list(ssl, ciphers) == 0) {
                LOG_ERROR("SocketSSL.SocketSSL [", fd,"]: In function 'SSL_set_cipher_list' - ", CheckSSLErrors());
                SSLCloseAfterError(); return;
            }
        }

        // All errors processing OpenSSL library.
        SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

        // Create new I/O object.
        bio = BIO_new_socket(fd, BIO_NOCLOSE);
        if (bio == nullptr) {
            LOG_ERROR("SocketSSL.SocketSSL [", fd,"]: In function 'BIO_new_socket' - ", CheckSSLErrors());
            SSLCloseAfterError(); return;
        }

        // Assign bio to ssl object.
        SSL_set_bio(ssl, bio, bio);
        // Set non-blocking mode for bio.
        BIO_set_nbio(bio, 1);
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

        if (Socket::Connect(host, port) == false || DoHandshakeSSL() == false)
        {
            SSLCloseAfterError();
            return false;
        }
        return true;
    }


    int32_t SocketSSL::Send (const char* data, size_t length)
    {
        if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
            LOG_ERROR("SocketSSL.Send: Socket is invalid.");
            SSLCloseAfterError(); return -1;
        }
        LOG_TRACE("SocketSSL.Send [", fd,"]: Sending data to '", exHost, "'...");

        std::size_t idx = 0;
        while (length > 0)
        {
            const int32_t result = SSL_write(ssl, &data[idx], static_cast<int32_t>(length));
            const std::size_t err = ERR_get_error();
            if (err != 0) {
                LOG_ERROR("SocketSSL.Send [", fd,"]: In function 'SSL_write' - ", ERR_error_string(err, nullptr));
                SSLCloseAfterError(); return -1;
            }

            if (result <= 0)
            {
                if (BIO_should_retry(bio)) {
                    if (IsReadyForSend(3000) == false) { SSLCloseAfterError(); return -1; }
                    continue;
                }
                LOG_ERROR("SocketSSL.Send [", fd,"]: In function 'SSL_write' - ", CheckSSLErrors());
                SSLCloseAfterError(); return -1;
            }

            length -= static_cast<size_t>(result);
            idx += static_cast<size_t>(result);
        }

        LOG_INFO("SocketSSL.Send [", fd,"]: Sending data to '", exHost, "' is success:  ", idx, " bytes.");
        return static_cast<int32_t>(idx);
    }


    int32_t SocketSSL::Recv (char* data, size_t length, bool noWait)
    {
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
                if (BIO_should_retry(bio))
                {
                    if (IsReadyForRecv(3000) == false) {
                        if (idx == 0) { SSLCloseAfterError(); return -1; }
                        break;  // In this case no error.
                    }
                    continue;
                }
                LOG_ERROR("SocketSSL.Recv [", fd,"]: In function 'SSL_read' - ", CheckSSLErrors());
                SSLCloseAfterError(); return -1;
            }

            length -= static_cast<size_t>(result);
            idx += static_cast<size_t>(result);
            if (noWait == true) { break; }
        }

        LOG_INFO("SocketSSL.Recv [", fd,"]: Receiving data from '", exHost, "' is success:  ", idx, " bytes.");
        return static_cast<int32_t>(idx);
    }


    int32_t SocketSSL::RecvToEnd (char* data, size_t length)
    {
        if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
            LOG_ERROR("SocketSSL.RecvToEnd: Socket is invalid.");
            SSLCloseAfterError(); return -1;
        }
        LOG_TRACE("SocketSSL.RecvToEnd [", fd,"]: Receiving data from '", exHost, "'...");
        const system_clock::time_point limit = system_clock::now() + GetTimeout();

        std::size_t idx = 0;
        while (length > 0 && IsReadyForRecv(3000) == true && system_clock::now() < limit)
        {
            const int32_t result = SSL_read(ssl, &data[idx], static_cast<int32_t>(length));
            const std::size_t err = ERR_get_error();
            if (err != 0) {
                LOG_ERROR("SocketSSL.RecvToEnd [", fd,"]: In function 'SSL_read' - ", ERR_error_string(err, nullptr));
                SSLCloseAfterError(); return -1;
            }

            if (result == 0) { break; }
            if (result < 0) {
                if (BIO_should_retry(bio)) { continue; }
                LOG_ERROR("SocketSSL.RecvToEnd [", fd,"]: In function 'SSL_read' - ", CheckSSLErrors());
                CloseAfterError(); return -1;
            }

            length -= static_cast<size_t>(result);
            idx += static_cast<size_t>(result);
        }

        LOG_INFO("SocketSSL.RecvToEnd [", fd,"]: Receiving data from '", exHost, "' is success:  ", idx, " bytes.");
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
                LOG_INFO("SocketSSL.DoHandshakeSSL [", fd,"]: Handshake to '", exHost, "' is success.");
                return true;
            }
            if (result <= 0)
            {
                if (BIO_should_retry(bio)) {
                    if (IsReadyForSend(3000) == false) { SSLCloseAfterError(); break; }
                    continue;
                }
                LOG_ERROR("SocketSSL.DoHandshakeSSL [", fd,"]: Handshake failed - ", CheckSSLErrors());
                break;
            }
        }
        return false;
    }


    std::list<std::string> SocketSSL::GetCiphersList(void) const
    {
        std::list<std::string> result;
        STACK_OF(SSL_CIPHER)* ciphers_list = SSL_get_ciphers(ssl);
        for (int32_t i = 0; i < sk_SSL_CIPHER_num(ciphers_list); ++i)
        {
            const SSL_CIPHER* cipher = sk_SSL_CIPHER_value(ciphers_list, i);
            result.emplace_back(SSL_CIPHER_get_name(cipher));
        }
        return result;
    }


    bool SocketSSL::SetOnlySecureCiphers(void)
    {
        if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
            LOG_ERROR("SocketSSL.SetOnlySecureCiphers: Socket is invalid.");
            SSLCloseAfterError();
            return false;
        }

        std::string secure;
        const std::list<std::string> current = GetCiphersList();
        for (auto&& it : current)
        {
            if (it.find("SRP") == it.npos && it.find("DH-") == it.npos      &&
                it.find("RC4") == it.npos && it.find("CAMELLIA") == it.npos &&
                it.find("MD5") == it.npos && it.find("SEED") == it.npos     &&
                it.find("DES") == it.npos && it.find("PSK") == it.npos)
            {
                secure += it + ';';
            }
        }

        if (secure.empty() || SSL_set_cipher_list(ssl, secure.c_str()) == 0) {
            LOG_ERROR("SocketSSL.SetOnlySecureCiphers [", fd,"]: Setting only secure ciphers failed.");
            return false;
        }
        LOG_INFO("SocketSSL.SetOnlySecureCiphers [", fd,"]: Setting only secure ciphers is success.");
#if ( defined(DEBUG) )
        std::ostringstream buff;
        const auto result = common::split(secure, ';');
        std::copy(result.begin(), result.end(), std::ostream_iterator<std::string>(buff, "\n|-----------------\n|"));
        LOG_TRACE("SocketSSL.SetOnlySecureCiphers [", fd,"]: Setting following ciphers:\n|-----------------\n|", buff.str());
#endif
        return true;
    }


    bool SocketSSL::SetInternalProtocol (const unsigned char* proto, std::size_t length)
    {
        if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
            LOG_ERROR("SocketSSL.SetInternalProtocol: Socket is invalid.");
            SSLCloseAfterError();
            return false;
        }

        if (SSL_set_alpn_protos(ssl, &proto[0], static_cast<uint32_t>(length)) == 0) {
            LOG_INFO("SocketSSL.SetInternalProtocol [", fd,"]: Set ALPN protocol is success.");
            return true;
        }
        LOG_ERROR("SocketSSL.SetInternalProtocol [", fd,"]: In function 'SetInternalProtocol' - ", CheckSSLErrors());
        return false;
    }


    bool SocketSSL::SetHttp_2_0_OnlyProtocol(void)
    {
        const unsigned char proto[] = { 2, 'h', '2' };
        return SetInternalProtocol(proto, sizeof(proto));
    }


    bool SocketSSL::SetHttp_1_1_OnlyProtocol(void)
    {
        const unsigned char proto[] = { 8, 'h', 't', 't', 'p', '/', '1', '.', '1' };
        return SetInternalProtocol(proto, sizeof(proto));
    }


    bool SocketSSL::SetHttpProtocols(void)
    {
        const unsigned char proto[] = {
                2, 'h', '2',
                8, 'h', 't', 't', 'p', '/', '1', '.', '1'
        };
        return SetInternalProtocol(proto, sizeof(proto));
    }


    void SocketSSL::Shutdown (const int32_t how) const
    { // Encryption alert (21).
        if (ssl != nullptr)
        {
            const int32_t res = SSL_shutdown(ssl);
            if (res == 0) { SSL_shutdown(ssl); }
            else { LOG_ERROR("SocketSSL.Shutdown [", fd,"]: In function 'SSL_shutdown' - ", CheckSSLErrors()); }
        }
        Socket::Shutdown(how);
    }


    // Close connection.
    void SocketSSL::Close(void)
    {
        if (ssl != nullptr) { Shutdown(); SSL_free(ssl); ssl = nullptr; }
        Socket::Close();
    }

    // Returns true when the handshake is complete.
    bool SocketSSL::IsHandshakeReady(void) const
    {
        return (ssl != nullptr && SSL_is_init_finished(ssl) != 0);
    }

    // Get selected ALPN protocol by server in string type.
    std::string SocketSSL::GetRawSelectedProtocol(void) const
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

    // Get current timeout of the SSL session.
    std::size_t SocketSSL::GetSessionTimeout(void) const
    {
        return static_cast<std::size_t>(SSL_get_timeout(SSL_get_session(ssl)));
    }

    // Get selected ALPN protocol by server.
    protocols::http::HTTP_VERSION SocketSSL::GetSelectedProtocol(void) const
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

    // Get selected cipher name in ssl connection.
    std::string SocketSSL::GetSelectedCipherName(void) const
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
            LOG_ERROR("SSLContext: In function 'SSL_CTX_new (TLSv1.0)' - ", CheckSSLErrors());
            abort();
        }
        ctx[SSL_METHOD_TLS11] = SSL_CTX_new( TLSv1_1_client_method() );
        if (ctx[SSL_METHOD_TLS11] == nullptr) {
            LOG_ERROR("SSLContext: In function 'SSL_CTX_new (TLSv1.1)' - ", CheckSSLErrors());
            abort();
        }
        ctx[SSL_METHOD_TLS12] = SSL_CTX_new( TLSv1_2_client_method() );
        if (ctx[SSL_METHOD_TLS12] == nullptr) {
            LOG_ERROR("SSLContext: In function 'SSL_CTX_new (TLSv1.2)' - ", CheckSSLErrors());
            abort();
        }
        LOG_INFO("SSLContext: Initialize SSL library is success.");
    }

    SSL_CTX* SSLContext::Get (std::size_t method) const noexcept
    {
        if (method < NUMBER_OF_CTX) { return ctx[method]; }
        return nullptr;
    }

    SSLContext::~SSLContext(void) noexcept
    {
        SSL_CTX_free( ctx[SSL_METHOD_TLS1] );
        SSL_CTX_free( ctx[SSL_METHOD_TLS11] );
        SSL_CTX_free( ctx[SSL_METHOD_TLS12] );
    }

    // Initialized context.
    SSLContext SocketSSL::context;

}  // namespace net.
