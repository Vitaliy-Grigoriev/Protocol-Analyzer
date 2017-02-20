cmake_minimum_required(VERSION 3.6)
project(http2_analyzer)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_INCLUDE_PATH /usr/include/)
set(CMAKE_LIBRARY_PATH /usr/lib/)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fsanitize=address -fsanitize=leak -fsanitize=undefined -Weverything -Wno-c++98-compat -Wno-exit-time-destructors -Wno-global-constructors -Wno-documentation -Wno-padded -DDEBUG")
#set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fsanitize=thread -fsanitize=undefined -Weverything -Wno-c++98-compat -Wno-exit-time-destructors -Wno-global-constructors -Wno-documentation -Wno-padded -DDEBUG")
#set(CMAKE_CXX_FLAGS_DEBUG "-g -O1 -fsanitize=memory -fno-omit-frame-pointer -Weverything -Wno-c++98-compat -Wno-exit-time-destructors -Wno-global-constructors -Wno-documentation -Wno-padded -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DRELEASE -Wno-documentation -Wno-padded")


set(LOGGING_LIB      include/analyzer/Log.hpp     src/Log.cpp)
set(HTTP_LIB         include/analyzer/Http.hpp    src/Http.cpp)
set(DIAGNOSTIC_LIB   include/analyzer/Timer.hpp   src/Timer.cpp)
set(SOCKET_LIB       include/analyzer/Socket.hpp include/analyzer/SocketManager.hpp   src/Socket.cpp src/SocketSSL.cpp src/SocketManager.cpp)

add_library(Logging      ${LOGGING_LIB})
add_library(Diagnostic   ${DIAGNOSTIC_LIB})
add_library(Http         ${HTTP_LIB}   ${LOGGING_LIB} ${DIAGNOSTIC_LIB})
add_library(Socket       ${SOCKET_LIB} ${LOGGING_LIB} ${DIAGNOSTIC_LIB})

target_link_libraries(Socket   ssl crypto pthread)


set(SSL_TEST                 test/test_ssl.cpp)
set(TIMING_TEST              test/test_timing.cpp)
set(MANAGER_TEST             test/test_manager.cpp)
set(HTTP2_NEGOTIATION_TEST   test/test_http2_negotiation.cpp)

add_executable(test_ssl                 ${SSL_TEST})
add_executable(test_timing              ${TIMING_TEST})
add_executable(test_manager             ${MANAGER_TEST})
add_executable(test_http2_negotiation   ${HTTP2_NEGOTIATION_TEST})

target_link_libraries(test_timing              Diagnostic)
target_link_libraries(test_ssl                 Socket Http)
target_link_libraries(test_manager             Socket Http)
target_link_libraries(test_http2_negotiation   Socket Http)