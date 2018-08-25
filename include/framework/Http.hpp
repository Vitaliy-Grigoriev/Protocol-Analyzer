#ifndef PROTOCOL_ANALYZER_HTTP_HPP
#define PROTOCOL_ANALYZER_HTTP_HPP

#include "Protocol.hpp"


namespace analyzer::framework::net::protocols::http
{
    /**
     * @enum HTTP_VERSION
     * @brief The versions of the HTTP protocols.
     */
    enum HTTP_VERSION : uint16_t
    {
        UNKNOWN = 0,
        HTTP1_1 = 1,
        HTTP2_0 = 2
    };


    /**
     * @class HttpHeader Http.hpp "include/framework/Http.hpp"
     * @brief HTTP/1.1 and HTTP/2.0 headers class.
     */
    class HttpHeader {


    };


    /**
     * @class http   Http.hpp   "include/framework/Http.hpp"
     * @brief HTTP/1.1 and HTTP/2.0 implementations.
     */
    class http : public Protocol {
    private:


    public:
        http (http &&) = delete;
        http (const http &) = delete;
        http & operator= (http &&) = delete;
        http & operator= (const http &) = delete;

        /**
         * @fn http (void *, const std::size_t, HTTP_VERSION = HTTP1_1);
         * @brief Constructor of HTTP protocol class.
         * @param [in] data - The data from the socket.
         * @param [in] length - The size of the data in bytes.
         * @param [in] HTTP_VERSION - The version of the considered protocol.
         */
        http (const void * /*data*/, std::size_t /*length*/, HTTP_VERSION /*version*/ = HTTP1_1);

        /**
         * @fn explicit http (const std::string &, HTTP_VERSION);
         * @brief Constructor of HTTP protocol class.
         * @param [in] data - The data from the socket.
         * @param [in] HTTP_VERSION - The version of the considered protocol.
         */
        explicit http (const std::string & /*data*/, HTTP_VERSION /*version*/ = HTTP1_1);

        /**
         * @fn bool ParseHeader() final;
         * @brief Parsing the HTTP/1.1 and HTTP/2.0 header.
         */
        bool ParseHeader(void) final;

        /**
         * @fn ~http();
         * @brief Destructor of http class.
         */
        ~http(void);
    };

}  // namespace http.


#endif  // PROTOCOL_ANALYZER_HTTP_HPP
