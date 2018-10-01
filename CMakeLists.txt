cmake_minimum_required(VERSION   3.8)  # For support C++17 standard.
project(ProtocolAnalyzer   CXX)

# Select output binary paths.
set(EXECUTABLE_OUTPUT_PATH   ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH      ${PROJECT_SOURCE_DIR}/lib)
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

# Select CPP standard.
set(CMAKE_CXX_STANDARD   17)
set(CMAKE_CXX_STANDARD_REQUIRED   ON)

# Choose compiler.
if (DEFINED  ENV{COMPILER})  # If the custom compiler is defined.
    message("The compiler is set to '$ENV{COMPILER}'.")
    set(CMAKE_CXX_COMPILER   "$ENV{COMPILER}")
else()  # If the custom compiler is not defined.
    message("The compiler is set to 'g++' by default.")
    set(CMAKE_CXX_COMPILER   "/usr/bin/g++")
endif()

# Choose build type.
if (DEFINED   ENV{BUILD_TYPE})  # If the custom build type is defined.
    message("The build type is set to '$ENV{BUILD_TYPE}'.")
    set(CMAKE_BUILD_TYPE   "$ENV{BUILD_TYPE}")
else()  # If the custom build type is not defined.
    message("The build type is set to 'Release' by default.")
    set(CMAKE_BUILD_TYPE   "Release")
endif()


set(DISABLE_WARNINGS_RELEASE   "-Wno-variadic-macros")
set(DISABLE_WARNINGS_DEBUG   "-Wno-unused-result -Wno-deprecated-declarations -Wno-unused-macros -Wno-unused-variable -Wno-padded -Wno-documentation")
set(DISABLE_WARNINGS   "-Wno-unknown-pragmas -Wno-unused-parameter -Wno-c++98-compat -Wno-missing-prototypes -Wno-variadic-macros -Wno-exit-time-destructors -Wno-zero-as-null-pointer-constant")
set(ADDITIONAL_FLAGS   "-fsanitize=address  -fsanitize=leak  -fsanitize=undefined")
#set(ADDITIONAL_FLAGS   "-fsanitize=thread")
#set(ADDITIONAL_FLAGS   "-fsanitize=dataflow")
#set(ADDITIONAL_FLAGS   "-fsanitize=memory  -fno-omit-frame-pointer")

# Choose compile flags.
set(CMAKE_CXX_FLAGS_RELEASE   "-std=c++17 -O2 -DRELEASE -DNDEBUG   -Wall -Wextra -Wpedantic  ${DISABLE_WARNINGS_RELEASE}")
string(FIND  ${CMAKE_CXX_COMPILER}  "clang"  POSITION)
if (${POSITION}  GREATER  "-1")  # Build flags for Clang compiler only.
    set(CMAKE_CXX_FLAGS_DEBUG   "-std=c++17 -O0 -g -DDEBUG   -Weverything -Wthread-safety  ${DISABLE_WARNINGS} ${ADDITIONAL_FLAGS} ${DISABLE_WARNINGS_DEBUG}")
else()  # Other compilers.
    set(CMAKE_CXX_FLAGS_DEBUG   "-std=c++17 -O0 -g -DDEBUG   -Wall -Wextra -Wpedantic  ${DISABLE_WARNINGS}")
endif()

# Output compile flags.
message("Release flags:   ${CMAKE_CXX_FLAGS_RELEASE}.")
message("Debug flags:   ${CMAKE_CXX_FLAGS_DEBUG}.")


# Build Analyzer Framework libraries.
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



# Build test suites for Analyzer Framework library.
set(SSL_TEST                  ${TESTS}/test_ssl.cpp                  ${FRAMEWORK_INCLUDES_PATH}/AnalyzerApi.hpp)
set(SOCKET_TEST               ${TESTS}/test_socket.cpp               ${FRAMEWORK_INCLUDES_PATH}/AnalyzerApi.hpp)
set(COMMON_TEST               ${TESTS}/test_common.cpp               ${FRAMEWORK_INCLUDES_PATH}/AnalyzerApi.hpp)
set(BINARY_DATA_ENGINE_TEST   ${TESTS}/test_binary_data_engine.cpp   ${FRAMEWORK_INCLUDES_PATH}/AnalyzerApi.hpp)
set(TIMING_TEST               ${TESTS}/test_timing.cpp               ${FRAMEWORK_INCLUDES_PATH}/AnalyzerApi.hpp)
set(LOGGING_TEST              ${TESTS}/test_logging.cpp              ${FRAMEWORK_INCLUDES_PATH}/AnalyzerApi.hpp)
set(ANYTHING_TEST             ${TESTS}/test_anything.cpp             ${FRAMEWORK_INCLUDES_PATH}/AnalyzerApi.hpp)
set(CALLBACKS_TEST            ${TESTS}/test_callbacks.cpp            ${FRAMEWORK_INCLUDES_PATH}/AnalyzerApi.hpp)
set(URI_PARSER_TEST           ${TESTS}/test_uri_parser.cpp           ${FRAMEWORK_INCLUDES_PATH}/AnalyzerApi.hpp)
set(NOTIFICATION_TEST         ${TESTS}/test_notification.cpp         ${FRAMEWORK_INCLUDES_PATH}/AnalyzerApi.hpp)
set(HTTP2_NEGOTIATION_TEST    ${TESTS}/test_http2_negotiation.cpp    ${FRAMEWORK_INCLUDES_PATH}/AnalyzerApi.hpp)

add_executable(test_ssl                  ${SSL_TEST})
add_executable(test_socket               ${SOCKET_TEST})
add_executable(test_common               ${COMMON_TEST})
add_executable(test_timing               ${TIMING_TEST})
add_executable(test_binary_data_engine   ${BINARY_DATA_ENGINE_TEST})
add_executable(test_logging              ${LOGGING_TEST})
add_executable(test_anything             ${ANYTHING_TEST})
add_executable(test_callbacks            ${CALLBACKS_TEST})
add_executable(test_uri_parser           ${URI_PARSER_TEST})
add_executable(test_notification         ${NOTIFICATION_TEST})
add_executable(test_http2_negotiation    ${HTTP2_NEGOTIATION_TEST})

set_target_properties(
        test_ssl
        test_socket
        test_common
        test_timing
        test_binary_data_engine
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
target_link_libraries(test_common              AnalyzerFramework)
target_link_libraries(test_binary_data_engine  AnalyzerFramework)
target_link_libraries(test_timing              AnalyzerFramework)
target_link_libraries(test_anything            AnalyzerFramework)
target_link_libraries(test_logging             AnalyzerFramework)
target_link_libraries(test_callbacks           AnalyzerFramework)
target_link_libraries(test_uri_parser          AnalyzerFramework)
target_link_libraries(test_notification        AnalyzerFramework)
target_link_libraries(test_http2_negotiation   AnalyzerFramework)
