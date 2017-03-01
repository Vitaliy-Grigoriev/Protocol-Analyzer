#pragma once
#ifndef HTTP2_ANALYZER_API_HPP
#define HTTP2_ANALYZER_API_HPP

/**
 * @namespace analyzer
 * @brief The main namespace that contain definition of analyzer framework.
 */
namespace analyzer {
    #define ANALYZER_VERSION_MAJOR 0
    #define ANALYZER_VERSION_MINOR 0
    #define ANALYZER_VERSION_PATCH 3
}  // namespace analyzer.


#include "Socket.hpp"
#include "SocketManager.hpp"
#include "Http.hpp"
#include "Utilities.hpp"


#endif  // HTTP2_ANALYZER_API_HPP
