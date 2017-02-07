// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../include/Socket.hpp"

namespace analyzer {
    namespace net {

        SocketSSL::SocketSSL (const uint16_t method, const char* ciphers, const uint32_t timeout)
                : Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP, timeout)
        {
            if (Socket::IsError()) { return; }
            if (method >= NUMBER_OF_CTX) {
                log::DbgLog("[error] SocketSSL [", fd,"]: SSL input protocol type is invalid.");
                CleaningAfterError(); return;
            }

            ssl = SSL_new(context.Get(method));
            if (ssl == nullptr) {
                log::DbgLog("[error] SocketSSL [", fd,"]: Error in function 'SSL_new' - ", CheckErrors());
                CleaningAfterError(); return;
            }

            if (ciphers != nullptr) {
                if (SSL_set_cipher_list(ssl, ciphers) == 0) {
                    log::DbgLog("[error] SocketSSL [", fd,"]: Error in function 'SSL_set_cipher_list' - ", CheckErrors());
                    CleaningAfterError(); return;
                }
            }

            // All errors processing OpenSSL library.
            SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

            // Create new I/O object.
            bio = BIO_new_socket(fd, BIO_NOCLOSE);
            if (bio == nullptr) {
                log::DbgLog("[error] SocketSSL [", fd,"]: Error in function 'BIO_new_socket' - ", CheckErrors());
                CleaningAfterError(); return;
            }

            // Assign bio to ssl object.
            SSL_set_bio(ssl, bio, bio);
            // Set non-blocking mode for bio.
            BIO_set_nbio(bio, 1);
            // Set client mode for ssl object.
            SSL_set_connect_state(ssl);
        }


        // Connecting to external host.
        void SocketSSL::Connect (const char* host, const uint16_t port)
        {
            if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
                log::DbgLog("[error] SocketSSL: Socket is invalid.");
                return;
            }

            Socket::Connect(host, port);
            if (IsError()) {
                CleaningAfterError();
                return;
            }

            if (!DoHandshakeSSL()) {
                CleaningAfterError();
            }
        }


        int32_t SocketSSL::Send (char* data, size_t length)
        {
            if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
                log::DbgLog("[error] SocketSSL: Socket is invalid.");
                return -1;
            }
            log::DbgLog("[*] SocketSSL [", fd,"]: Sending data to host '", exHost, "'...");

            size_t idx = 0;
            while (length > 0)
            {
                int32_t result = SSL_write(ssl, &data[idx], static_cast<int32_t>(length));
                size_t err = ERR_get_error();
                if (err != 0) {
                    log::DbgLog("[error] SocketSSL [", fd,"]: Error in function 'SSL_write' - ", ERR_error_string(err, nullptr));
                    CleaningAfterError(); return -1;
                }

                if (result <= 0) {
                    if (BIO_should_retry(bio)) {
                        if (!IsReadyForSend(3000)) { CleaningAfterError(); return -1; }
                        continue;
                    }
                    log::DbgLog("[error] SocketSSL: Error in function 'SSL_write' - ", CheckErrors());
                    CleaningAfterError(); return -1;
                }

                length -= static_cast<size_t>(result);
                idx += static_cast<size_t>(result);
            }

            log::DbgLog("[+] SocketSSL [", fd,"]: Sending data to host '", exHost, "' is successful: ", idx, " bytes.");
            return static_cast<int32_t>(idx);
        }


        int32_t SocketSSL::Recv (char* data, size_t length, const bool noWait)
        {
            if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
                log::DbgLog("[error] SocketSSL: Socket is invalid.");
                return -1;
            }
            log::DbgLog("[*] SocketSSL [", fd,"]: Receiving data from host '", exHost, "'...");
            const system_clock::time_point limit = system_clock::now() + GetTimeout();

            size_t idx = 0;
            while (length > 0 && system_clock::now() < limit)
            {
                int32_t result = SSL_read(ssl, &data[idx], static_cast<int32_t>(length));
                size_t err = ERR_get_error();
                if (err != 0) {
                    log::DbgLog("[error] SocketSSL [", fd,"]: Error in function 'SSL_read' - ", ERR_error_string(err, nullptr));
                    CleaningAfterError(); return -1;
                }

                if (result < 0) {
                    if (BIO_should_retry(bio)) {
                        if (!IsReadyForRecv(3000)) {
                            if (idx == 0) { CleaningAfterError(); return -1; }
                            break;  // In this case no error.
                        }
                        continue;
                    }
                    log::DbgLog("[error] SocketSSL: Error in function 'SSL_read' - ", CheckErrors());
                    CleaningAfterError(); return -1;
                }
                if (result == 0) { break; }

                length -= static_cast<size_t>(result);
                idx += static_cast<size_t>(result);
                if (noWait) { break; }
            }

            log::DbgLog("[+] SocketSSL [", fd,"]: Receiving data from host '", exHost, "' is successful: ", idx, " bytes.");
            return static_cast<int32_t>(idx);
        }


        int32_t SocketSSL::RecvToEnd (char* data, size_t length)
        {
            if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
                log::DbgLog("[error] SocketSSL: Socket is invalid.");
                return -1;
            }
            log::DbgLog("[*] SocketSSL [", fd,"]: Receiving data from host '", exHost, "'...");
            const system_clock::time_point limit = system_clock::now() + GetTimeout();

            size_t idx = 0;
            while (length > 0 &&IsReadyForRecv(3000) && system_clock::now() < limit)
            {
                int32_t result = SSL_read(ssl, &data[idx], static_cast<int32_t>(length));
                size_t err = ERR_get_error();
                if (err != 0) {
                    log::DbgLog("[error] SocketSSL [", fd,"]: Error in function 'SSL_read' - ", ERR_error_string(err, nullptr));
                    CleaningAfterError(); return -1;
                }

                if (result < 0) {
                    if (BIO_should_retry(bio)) { continue; }
                    log::DbgLog("[error] SocketSSL: Error in function 'SSL_read' - ", CheckErrors());
                    CloseAfterError(); return -1;
                }
                if (result == 0) { break; }

                length -= static_cast<size_t>(result);
                idx += static_cast<size_t>(result);
            }

            log::DbgLog("[+] SocketSSL [", fd,"]: Receiving data from host '", exHost, "' is successful: ", idx, " bytes.");
            return static_cast<int32_t>(idx);
        }


        bool SocketSSL::DoHandshakeSSL ()
        {
            if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
                log::DbgLog("[error] SocketSSL: Socket is invalid.");
                return false;
            }
            log::DbgLog("[*] SocketSSL [", fd,"]: Doing handshake...");

            while (true) {
                int32_t result = SSL_do_handshake(ssl);
                size_t err = ERR_get_error();
                if (err != 0) {
                    log::DbgLog("[error] SocketSSL [", fd,"]: Error in function 'SSL_write' - ", ERR_error_string(err, nullptr));
                    break;
                }

                if (result == 1) {
                    log::DbgLog("[+] SocketSSL [", fd,"]: Handshake to host '", exHost, "' is successful.");
                    return true;
                }
                if (result <= 0) {
                    if (BIO_should_retry(bio)) {
                        if (!IsReadyForSend(3000)) { CleaningAfterError(); break; }
                        continue;
                    }
                    log::DbgLog("[error] SocketSSL [", fd,"]: Handshake failed - ", CheckErrors());
                    break;
                }
            }
            return false;
        }


        std::list<std::string> SocketSSL::GetCiphersList () const
        {
            std::list<std::string> result;
            STACK_OF(SSL_CIPHER) *ciphers_list = SSL_get_ciphers(ssl);
            for (int32_t i = 0; i < sk_SSL_CIPHER_num(ciphers_list); ++i) {
                const SSL_CIPHER *cipher = sk_SSL_CIPHER_value(ciphers_list, i);
                result.emplace_back(SSL_CIPHER_get_name(cipher));
            }
            return result;
        }


        bool SocketSSL::SetOnlySecureCiphers ()
        {
            if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
                log::DbgLog("[error] SocketSSL: Socket is invalid.");
                return false;
            }

            std::string secure;
            std::list<std::string> current = GetCiphersList();
            for (auto it : current) {
                if (it.find("SRP") == it.npos && it.find("DH-") == it.npos      &&
                    it.find("RC4") == it.npos && it.find("CAMELLIA") == it.npos &&
                    it.find("MD5") == it.npos && it.find("SEED") == it.npos     &&
                    it.find("DES") == it.npos && it.find("PSK") == it.npos)
                {
                    secure += it + ';';
                }
            }

            if (secure.empty() || SSL_set_cipher_list(ssl, secure.c_str()) == 0) {
                log::DbgLog("[error] SocketSSL [", fd,"]: Setting only secure ciphers is failure.");
                return false;
            }
            log::DbgLog("[+] SocketSSL [", fd,"]: Setting only secure ciphers is successful.");
            return true;
        }


        bool SocketSSL::SetHttp_2_0_OnlyProtocol ()
        {
            if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
                log::DbgLog("[error] SocketSSL: Socket is invalid.");
                return false;
            }

            unsigned char proto[] = { 2, 'h', '2' };
            if (SSL_set_alpn_protos(ssl, &proto[0], sizeof(proto)) == 0) {
                log::DbgLog("[+] SocketSSL [", fd,"]: Change protocol to http/2.0 is successful.");
                return true;
            }
            log::DbgLog("[error] SocketSSL [", fd,"]: Error in function 'SetHttp_2_0_OnlyProtocol' - ", CheckErrors());
            return false;
        }


        bool SocketSSL::SetHttp_1_1_OnlyProtocol ()
        {
            if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
                log::DbgLog("[error] SocketSSL: Socket is invalid.");
                return false;
            }

            unsigned char proto[] = { 8, 'h', 't', 't', 'p', '/', '1', '.', '1' };
            if (SSL_set_alpn_protos(ssl, &proto[0], sizeof(proto)) == 0) {
                log::DbgLog("[+] SocketSSL [", fd,"]: Change protocol to http/1.1 is successful.");
                return true;
            }
            log::DbgLog("[error] SocketSSL [", fd,"]: Error in function 'SetHttp_1_1_OnlyProtocol' - ", CheckErrors());
            return false;
        }


        bool SocketSSL::SetHttpProtocols ()
        {
            if (fd == INVALID_SOCKET || ssl == nullptr || bio == nullptr) {
                log::DbgLog("[error] SocketSSL: Socket is invalid.");
                return false;
            }

            unsigned char proto[] = {
                    2, 'h', '2',
                    8, 'h', 't', 't', 'p', '/', '1', '.', '1'
            };
            if (SSL_set_alpn_protos(ssl, &proto[0], sizeof(proto)) == 0) {
                log::DbgLog("[+] SocketSSL [", fd,"]: Change protocol to http/2.0 and http/1.1 is successful.");
                return true;
            }
            log::DbgLog("[error] SocketSSL [", fd,"]: Error in function 'SetHttpProtocols' - ", CheckErrors());
            return false;
        }


        void SocketSSL::Shutdown (int32_t how)
        { // Encryption alert (21).
            if (ssl != nullptr) {
                int32_t res = SSL_shutdown(ssl);
                if (res == 0) { SSL_shutdown(ssl); }
                else { log::DbgLog("[error] SocketSSL [", fd,"]: Error in function 'SSL_shutdown' - ", CheckErrors()); }
            }
            Socket::Shutdown(how);
        }


        // Close connection.
        void SocketSSL::Close ()
        {
            if (ssl != nullptr) { Shutdown(); SSL_free(ssl); ssl = nullptr; }
            Socket::Close();
        }

        // Returns true when the handshake is complete.
        bool SocketSSL::IsHandshakeReady () const
        {
            return (ssl != nullptr && SSL_is_init_finished(ssl));
        }

        // Cleaning after error.
        void SocketSSL::CleaningAfterError ()
        {
            if (ssl != nullptr) { SSL_free(ssl); ssl = nullptr; }
            Socket::CloseAfterError();
        }

        // Destructor.
        SocketSSL::~SocketSSL () {
            if (ssl != nullptr) { SSL_free(ssl); ssl = nullptr; }
            Socket::Close();
        }



        // Check SSL library error.
        inline std::string CheckErrors ()
        {
            std::size_t err = ERR_get_error();
            if (err != 0) { return ERR_error_string(err, nullptr); }
            return "NO ERROR.";
        }


        SSLContext::SSLContext () noexcept
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
                log::DbgLog("[error] SSLContext: Error in function 'SSL_CTX_new (TLSv1.0)' - ", CheckErrors());
                abort();
            }
            ctx[SSL_METHOD_TLS11] = SSL_CTX_new( TLSv1_1_client_method() );
            if (ctx[SSL_METHOD_TLS11] == nullptr) {
                log::DbgLog("[error] SSLContext: Error in function 'SSL_CTX_new (TLSv1.1)' - ", CheckErrors());
                abort();
            }
            ctx[SSL_METHOD_TLS12] = SSL_CTX_new( TLSv1_2_client_method() );
            if (ctx[SSL_METHOD_TLS12] == nullptr) {
                log::DbgLog("[error] SSLContext: Error in function 'SSL_CTX_new (TLSv1.2)' - ", CheckErrors());
                abort();
            }
            log::DbgLog("[+] SSLContext: Initialize SSL library is successful.");
        }

        SSL_CTX* SSLContext::Get (const std::size_t method) const noexcept
        {
            if (method < NUMBER_OF_CTX) { return ctx[method]; }
            return nullptr;
        }

        SSLContext::~SSLContext () noexcept
        {
            SSL_CTX_free( ctx[SSL_METHOD_TLS1] );
            SSL_CTX_free( ctx[SSL_METHOD_TLS11] );
            SSL_CTX_free( ctx[SSL_METHOD_TLS12] );
        }

        // Initialized context.
        SSLContext SocketSSL::context;

    }  // namespace net.
}  // namespace analyzer.

