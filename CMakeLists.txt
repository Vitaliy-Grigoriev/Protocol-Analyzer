cmake_minimum_required(VERSION 3.8)  # For support C++17 standard.
project(ProtocolAnalyzer CXX)

set(EXECUTABLE_OUTPUT_PATH      ${PROJECT_SOURCE_DIR}/bin/)
set(LIBRARY_OUTPUT_PATH         ${PROJECT_SOURCE_DIR}/lib/)
set(CMAKE_SYSTEM_INCLUDE_PATH   /usr/include/)
set(CMAKE_SYSTEM_LIBRARY_PATH   /usr/lib/)

set(INCLUDES   ${PROJECT_SOURCE_DIR}/include/analyzer)
set(SOURCES    ${PROJECT_SOURCE_DIR}/src)
set(TESTS      ${PROJECT_SOURCE_DIR}/test)

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
    set(CMAKE_CXX_FLAGS_DEBUG   "-std=c++17 -O0 -g -DDEBUG   -Weverything -Wthread-safety  ${ADDITIONAL_FLAGS} ${DISABLE_WARNINGS} ${DISABLE_WARNINGS_DEBUG}")
else()  # Other compilers.
    set(CMAKE_CXX_FLAGS_DEBUG   "-std=c++17 -O0 -g -DDEBUG   -Wall -Wextra -Wpedantic  ${DISABLE_WARNINGS}")
endif()


# Build core framework libraries.
set(SYSTEM_LIB       ${INCLUDES}/System.hpp ${INCLUDES}/Mutex.hpp    ${SOURCES}/System.cpp ${SOURCES}/Mutex.cpp)
set(LOGGING_LIB      ${INCLUDES}/Log.hpp                             ${SOURCES}/Log.cpp)
#set(COMMON_LIB       ${INCLUDES}/Common.hpp ${INCLUDES}/LockedDeque.hpp ${INCLUDES}/BinaryDataEngine.hpp ${INCLUDES}/BinaryDataEngineIterator.hpp    ${SOURCES}/Common.cpp ${SOURCES}/BinaryDataEngine.cpp ${SOURCES}/BinaryDataEngineIterator.cpp)
set(COMMON_LIB
        ${INCLUDES}/Common.hpp
        ${INCLUDES}/LockedDeque.hpp
        ${INCLUDES}/BinaryDataEngine.hpp
        ${INCLUDES}/BinaryStructuredDataEngine.hpp

        ${SOURCES}/Common.cpp
        ${SOURCES}/BitStreamEngine.cpp
        ${SOURCES}/ByteStreamEngine.cpp
        ${SOURCES}/BinaryDataEngine.cpp
        ${SOURCES}/BinaryStructuredDataEngine.cpp)

# Build functional framework libraries.
set(TASK_LIB         ${INCLUDES}/Notification.hpp) #${INCLUDES}/Task.hpp ${SOURCES}/Task.cpp   ${SOURCES}/TaskManager.cpp)
set(PARSER_LIB       ${INCLUDES}/Parser.hpp                          ${SOURCES}/Parser.cpp)
set(PROTOCOL_LIB     ${INCLUDES}/Protocol.hpp ${INCLUDES}/Http.hpp   ${SOURCES}/Protocol.cpp  ${SOURCES}/Http.cpp)
set(DIAGNOSTIC_LIB   ${INCLUDES}/Timer.hpp                           ${SOURCES}/Timer.cpp)
set(UTILITIES_LIB    ${INCLUDES}/Utilities.hpp                       ${SOURCES}/Utilities.cpp)
set(SOCKET_LIB       ${INCLUDES}/Socket.hpp                          ${SOURCES}/Socket.cpp  ${SOURCES}/SocketSSL.cpp)

add_library(Common       STATIC   ${COMMON_LIB})
add_library(System       STATIC   ${SYSTEM_LIB})
add_library(Logging      STATIC   ${LOGGING_LIB})
add_library(Task         STATIC   ${TASK_LIB})
add_library(Parser       STATIC   ${PARSER_LIB})
add_library(Diagnostic   STATIC   ${DIAGNOSTIC_LIB})
add_library(Socket       STATIC   ${SOCKET_LIB})
add_library(Protocol     STATIC   ${PROTOCOL_LIB})
add_library(Utilities    STATIC   ${UTILITIES_LIB})

target_link_libraries(Common      System)
target_link_libraries(Logging     Common)
target_link_libraries(Parser      Logging)
target_link_libraries(Protocol    Logging)
target_link_libraries(Socket      ssl crypto pthread Logging System)
target_link_libraries(Task        pthread Logging)
target_link_libraries(Utilities   Socket Protocol)


# Build test suites for framework library.
set(SSL_TEST                 ${TESTS}/test_ssl.cpp                  ${INCLUDES}/AnalyzerApi.hpp)
set(SOCKET_TEST              ${TESTS}/test_socket.cpp               ${INCLUDES}/AnalyzerApi.hpp)
set(COMMON_TEST              ${TESTS}/test_common.cpp               ${INCLUDES}/AnalyzerApi.hpp)
set(RAW_DATA_BUFFER_TEST     ${TESTS}/test_raw_data_buffer.cpp      ${INCLUDES}/AnalyzerApi.hpp)
set(TIMING_TEST              ${TESTS}/test_timing.cpp               ${INCLUDES}/AnalyzerApi.hpp)
#set(MANAGER_TEST             ${TESTS}/test_manager.cpp              ${INCLUDES}/AnalyzerApi.hpp)
set(LOGGING_TEST             ${TESTS}/test_logging.cpp              ${INCLUDES}/AnalyzerApi.hpp)
set(ANYTHING_TEST            ${TESTS}/test_anything.cpp             ${INCLUDES}/AnalyzerApi.hpp)
set(URI_PARSER_TEST          ${TESTS}/test_uri_parser.cpp           ${INCLUDES}/AnalyzerApi.hpp)
set(NOTIFICATION_TEST        ${TESTS}/test_notification.cpp         ${INCLUDES}/AnalyzerApi.hpp)
set(HTTP2_NEGOTIATION_TEST   ${TESTS}/test_http2_negotiation.cpp    ${INCLUDES}/AnalyzerApi.hpp)

add_executable(test_ssl                 ${SSL_TEST})
add_executable(test_socket              ${SOCKET_TEST})
add_executable(test_common              ${COMMON_TEST})
add_executable(test_timing              ${TIMING_TEST})
add_executable(test_raw_data_buffer     ${RAW_DATA_BUFFER_TEST})
#add_executable(test_manager             ${MANAGER_TEST})
add_executable(test_logging             ${LOGGING_TEST})
add_executable(test_anything            ${ANYTHING_TEST})
add_executable(test_uri_parser          ${URI_PARSER_TEST})
add_executable(test_notification        ${NOTIFICATION_TEST})
add_executable(test_http2_negotiation   ${HTTP2_NEGOTIATION_TEST})

#target_link_libraries(test_manager             Task)
target_link_libraries(test_ssl                 Utilities)
target_link_libraries(test_socket              Socket)
target_link_libraries(test_common              Common)
target_link_libraries(test_raw_data_buffer     Common)
target_link_libraries(test_timing              Diagnostic)
target_link_libraries(test_anything            Logging Common Parser)
target_link_libraries(test_logging             Logging)
target_link_libraries(test_uri_parser          Protocol)
target_link_libraries(test_notification        Task System)
target_link_libraries(test_http2_negotiation   Utilities)
