// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#ifndef PROTOCOL_ANALYZER_FRAMEWORK_MODULE_TYPES_HPP
#define PROTOCOL_ANALYZER_FRAMEWORK_MODULE_TYPES_HPP

#include <cstdint>  // uint16_t.


namespace analyzer::framework::modules
{
    /**
     * @enum FRAMEWORK_MODULE_TYPES
     * @brief Type of modules in protocol analyzer framework.
     */
    enum FRAMEWORK_MODULE_TYPES : uint16_t
    {
        MODULE_SOCKET = 0,                   // Socket class.
        MODULE_SOCKET_SSL,                   // SocketSSL class.
        MODULE_BINARY_DATA_ENGINE,           // BinaryDataEngine class.
        MODULE_BINARY_STRUCTED_DATA_ENGINE,  // BinaryStructuredDataEngine class.
        FRAMEWORK_MODULE_TYPES_SIZE
    };

}  // namespace modules.


#endif  // PROTOCOL_ANALYZER_FRAMEWORK_MODULE_TYPES_HPP
