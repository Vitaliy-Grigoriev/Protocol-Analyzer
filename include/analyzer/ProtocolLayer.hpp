#pragma once
#ifndef HTTP2_ANALYZER_PROTOCOLLAYER_HPP
#define HTTP2_ANALYZER_PROTOCOLLAYER_HPP

namespace analyzer {
    /**
     * @namespace protocol
     * @brief The namespace that contain definitions of network protocols.
     */
    namespace protocol {
        /**
         * @interface ProtocolLayer ProtocolLayer.hpp "include/analyzer/ProtocolLayer"
         * @brief Interface of application layer protocol.
         */
        class ProtocolLayer {
        protected:
            ProtocolLayer (ProtocolLayer &&) = delete;
            ProtocolLayer (const ProtocolLayer &) = delete;
            ProtocolLayer &operator= (ProtocolLayer &&) = delete;
            ProtocolLayer &operator= (const ProtocolLayer &) = delete;

        public:
            ProtocolLayer(void) = default;

            virtual ProtocolLayer & SetSettings (void * /*settings*/) = 0;

            virtual bool IsVerification(void) = 0;

            virtual void ParseHeader(void) = 0;

            virtual ~ProtocolLayer(void);
        };

    }  // namespace protocol.
}  // namespace analyzer.

#endif  // HTTP2_ANALYZER_PROTOCOLLAYER_HPP
