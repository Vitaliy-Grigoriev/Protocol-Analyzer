cmake_minimum_required(VERSION   3.8)  # For support C++17 standard.
project(ProtocolAnalyzer   CXX)

# Select CPP standard.
set(CMAKE_CXX_STANDARD   17)
set(CMAKE_CXX_STANDARD_REQUIRED   ON)

# Select output binary paths.
set(EXECUTABLE_OUTPUT_PATH   ${PROJECT_SOURCE_DIR}/bin)
# Select input system paths.
set(CMAKE_SYSTEM_INCLUDE_PATH   /usr/include)
set(CMAKE_SYSTEM_LIBRARY_PATH   /usr/lib)

# Select Analyzer Framework Module paths to source and include files.
set(FRAMEWORK_INCLUDES_PATH   ${PROJECT_SOURCE_DIR}/include/framework)
set(FRAMEWORK_SOURCES_PATH    ${PROJECT_SOURCE_DIR}/src/framework)
# Select Analyzer Scanner Module paths to source and include files.
set(SCANNER_INCLUDES_PATH ${PROJECT_SOURCE_DIR}/include/scanner)
set(SCANNER_SOURCES_PATH    ${PROJECT_SOURCE_DIR}/src/scanner)
# Select path to tests sources.
set(TESTS   ${PROJECT_SOURCE_DIR}/test)

# Info about compiler.
message("Compiler ID is '${CMAKE_CXX_COMPILER_ID}'.")

# Choose build type.
if (DEFINED   ENV{BUILD_TYPE})  # If the custom build type is defined.
    set(CMAKE_BUILD_TYPE   "$ENV{BUILD_TYPE}")
    message("Build type is set to '${CMAKE_BUILD_TYPE}'.")
else()  # If the custom build type is not defined.
    set(CMAKE_BUILD_TYPE   "Release")
    message("Build type is set to '${CMAKE_BUILD_TYPE}' by default.")
endif()


set(DISABLE_WARNINGS   "-Wno-unknown-pragmas -Wno-unused-parameter -Wno-zero-as-null-pointer-constant -Wno-deprecated-declarations")
set(DISABLE_WARNINGS_GCC   "-Wno-variadic-macros -Wno-unknown-pragmas")
set(DISABLE_WARNINGS_CLANG   "-Wno-return-std-move-in-c++11 -Wno-c++98-compat -Wno-exit-time-destructors -Wno-missing-prototypes -Wno-documentation")
set(DISABLE_WARNINGS_INTEL   "-diag-disable=161,1478")
set(DISABLE_WARNINGS_DEBUG   "-Wno-unused-result -Wno-unused-variable")
set(DISABLE_WARNINGS_DEBUG_CLANG   "-Wno-unused-macros -Wno-padded -Wno-weak-vtables -Wno-cast-align")

set(ADDITIONAL_FLAGS   "-fsanitize=address  -fsanitize=leak  -fsanitize=undefined")
#set(ADDITIONAL_FLAGS   "-fsanitize=thread")
#set(ADDITIONAL_FLAGS   "-fsanitize=dataflow")
#set(ADDITIONAL_FLAGS   "-fsanitize=memory  -fno-omit-frame-pointer")

# Choose compile flags.
set(CMAKE_CXX_FLAGS_DEBUG     "-std=c++17 -O0 -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE   "-std=c++17 -O2 -DRELEASE -DNDEBUG")
if (CMAKE_CXX_COMPILER_ID   MATCHES   "GNU")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}  -static-libstdc++ -static-libgcc   -Wall -Wextra -Wpedantic   ${DISABLE_WARNINGS} ${DISABLE_WARNINGS_GCC} ${DISABLE_WARNINGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE} ${DISABLE_WARNINGS} ${DISABLE_WARNINGS_GCC}")
elseif (CMAKE_CXX_COMPILER_ID   MATCHES   "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -fstandalone-debug   -Weverything -Wthread-safety   ${ADDITIONAL_FLAGS} ${DISABLE_WARNINGS} ${DISABLE_WARNINGS_CLANG} ${DISABLE_WARNINGS_DEBUG} ${DISABLE_WARNINGS_DEBUG_CLANG}")
    set(CMAKE_CXX_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE} ${DISABLE_WARNINGS} ${DISABLE_WARNINGS_CLANG}")
elseif (CMAKE_CXX_COMPILER_ID   MATCHES   "Intel")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -w3 -Wall -Wcheck   ${DISABLE_WARNINGS_INTEL}")
    set(CMAKE_CXX_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -xSSE4.2 -axAVX,CORE-AVX2   ${DISABLE_WARNINGS_INTEL}")
else()
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -Wall")
endif()

# Output compile flags.
message("Debug flags:   ${CMAKE_CXX_FLAGS_DEBUG}.")
message("Release flags:   ${CMAKE_CXX_FLAGS_RELEASE}.")


# Select Analyzer Framework source files.
file(GLOB   FSOURCES   "${FRAMEWORK_SOURCES_PATH}/*.cpp")
# Select Analyzer Framework include files.
file(GLOB   FHEADERS   "${FRAMEWORK_INCLUDES_PATH}/*.hpp")
# Ignore several source files.
list(FILTER   FSOURCES   EXCLUDE   REGEX   ".*BinaryDataEngineIterator.cpp$")
list(FILTER   FSOURCES   EXCLUDE   REGEX   ".*Task.cpp$")
list(FILTER   FSOURCES   EXCLUDE   REGEX   ".*TaskManager.cpp$")
list(FILTER   FHEADERS   EXCLUDE   REGEX   ".*BinaryDataEngineIterator.hpp$")
list(FILTER   FHEADERS   EXCLUDE   REGEX   ".*Task.hpp$")

# Build Analyzer Framework library.
add_library(AnalyzerFramework   STATIC   ${FSOURCES} ${FHEADERS})
# Link external libraries.
target_link_libraries(AnalyzerFramework   pthread ssl crypto)


# Add include path of the Analyzer Framework library.
include_directories(${FRAMEWORK_INCLUDES_PATH})
# Select Analyzer Scanner source files.
file(GLOB   SSOURCES   "${SCANNER_SOURCES_PATH}/*.cpp")
# Select Analyzer Scanner include files.
file(GLOB   SHEADERS   "${SCANNER_INCLUDES_PATH}/*.hpp")

# Build Analyzer Scanner Executable.
add_executable(NetProtoFuzz   ${SSOURCES} ${SHEADERS})
# Link Analyzer Framework library.
target_link_libraries(NetProtoFuzz   AnalyzerFramework)


# Build test suites for Analyzer Framework library.
set(SSL_TEST                  ${TESTS}/test_ssl.cpp)
set(SOCKET_TEST               ${TESTS}/test_socket.cpp)
set(NETLINK_TEST              ${TESTS}/test_netlink.cpp)
set(COMMON_TEST               ${TESTS}/test_common.cpp)
set(BINARY_DATA_ENGINE_TEST   ${TESTS}/test_binary_data_engine.cpp)
set(BINARY_STRUCTURED_DATA_ENGINE_TEST   ${TESTS}/test_binary_structured_data_engine.cpp)
set(TIMING_TEST               ${TESTS}/test_timing.cpp)
set(LOGGING_TEST              ${TESTS}/test_logging.cpp)
set(ANYTHING_TEST             ${TESTS}/test_anything.cpp)
set(CALLBACKS_TEST            ${TESTS}/test_callbacks.cpp)
set(URI_PARSER_TEST           ${TESTS}/test_uri_parser.cpp)
set(NOTIFICATION_TEST         ${TESTS}/test_notification.cpp)
set(HTTP2_NEGOTIATION_TEST    ${TESTS}/test_http2_negotiation.cpp)

add_executable(test_ssl                  ${SSL_TEST})
add_executable(test_socket               ${SOCKET_TEST})
add_executable(test_netlink              ${NETLINK_TEST})
add_executable(test_common               ${COMMON_TEST})
add_executable(test_timing               ${TIMING_TEST})
add_executable(test_binary_data_engine   ${BINARY_DATA_ENGINE_TEST})
add_executable(test_binary_structured_data_engine   ${BINARY_STRUCTURED_DATA_ENGINE_TEST})
add_executable(test_logging              ${LOGGING_TEST})
add_executable(test_anything             ${ANYTHING_TEST})
add_executable(test_callbacks            ${CALLBACKS_TEST})
add_executable(test_uri_parser           ${URI_PARSER_TEST})
add_executable(test_notification         ${NOTIFICATION_TEST})
add_executable(test_http2_negotiation    ${HTTP2_NEGOTIATION_TEST})

set_target_properties(
        test_ssl
        test_socket
        test_netlink
        test_common
        test_timing
        test_binary_data_engine
        test_binary_structured_data_engine
        test_logging
        test_anything
        test_callbacks
        test_uri_parser
        test_notification
        test_http2_negotiation
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY   ${EXECUTABLE_OUTPUT_PATH}/test_binaries
)

target_link_libraries(test_ssl                 AnalyzerFramework)
target_link_libraries(test_socket              AnalyzerFramework)
target_link_libraries(test_netlink             AnalyzerFramework)
target_link_libraries(test_common              AnalyzerFramework)
target_link_libraries(test_binary_data_engine  AnalyzerFramework)
target_link_libraries(test_binary_structured_data_engine  AnalyzerFramework)
target_link_libraries(test_timing              AnalyzerFramework)
target_link_libraries(test_anything            AnalyzerFramework)
target_link_libraries(test_logging             AnalyzerFramework)
target_link_libraries(test_callbacks           AnalyzerFramework)
target_link_libraries(test_uri_parser          AnalyzerFramework)
target_link_libraries(test_notification        AnalyzerFramework)
target_link_libraries(test_http2_negotiation   AnalyzerFramework)
