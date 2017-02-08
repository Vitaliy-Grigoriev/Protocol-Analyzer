#pragma once
#ifndef HTTP2_ANALYZER_HTTP_HPP
#define HTTP2_ANALYZER_HTTP_HPP


#include "../include/Log.hpp"


namespace analyzer {
    namespace http {

        // Interface of HTTP/x.x classes.
        class __HTTP_Interface {
        protected:
            __HTTP_Interface (__HTTP_Interface &&) = delete;
            __HTTP_Interface (const __HTTP_Interface &) = delete;
            __HTTP_Interface & operator= (__HTTP_Interface &&) = delete;
            __HTTP_Interface & operator= (const __HTTP_Interface &) = delete;

        public:
            // Constructor.
            __HTTP_Interface() = default;
            // Parsing the HTTP header.
            virtual void ParseHeader() = 0;
            // Destructor.
            virtual ~__HTTP_Interface() = default;
        };


        // HTTP/1.1 implementation.
        class HTTP1_1 : public virtual __HTTP_Interface {
        private:


        public:
            // Constructor.
            explicit HTTP1_1 (void * /*data*/, const std::size_t /*length*/);

            // Parsing the HTTP/1.1 header.
            void ParseHeader() override final;

            // Destructor.
            ~HTTP1_1();
        };

    }  // namespace http.
}  // namespace analyzer.


#endif  // HTTP2_ANALYZER_HTTP_HPP
