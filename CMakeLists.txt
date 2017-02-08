cmake_minimum_required(VERSION 3.6)
project(http2_analyzer)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_INCLUDE_PATH /usr/include/)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS_DEBUG "-pthread -lssl -lcrypto -g -O0 -fsanitize=address -fsanitize=leak -fsanitize=undefined -Weverything -Wno-c++98-compat -Wno-exit-time-destructors -Wno-global-constructors -DDEBUG")
#set(CMAKE_CXX_FLAGS_DEBUG "-pthread -lssl -lcrypto -g -O0 -fsanitize=thread -fsanitize=undefined -Weverything -Wno-c++98-compat -Wno-exit-time-destructors -Wno-global-constructors -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-pthread -lssl -lcrypto -O3 -DRELEASE")

# -fsanitize=thread -fsanitize=undefined
# -fsanitize=address -fsanitize=leak -fsanitize=undefined
# -fsanitize=memory -fno-omit-frame-pointer -fsanitize=undefined

set(LOGGING_LIB src/Log.cpp include/Log.hpp)
add_library(Logging ${LOGGING_LIB})
set(SOCKET_LIB src/Socket.cpp src/SocketSSL.cpp include/Socket.hpp src/SocketManager.cpp include/SocketManager.hpp)
add_library(Socket ${SOCKET_LIB} ${LOGGING_LIB})
set(HTTP_LIB src/Http.cpp src/Http.cpp include/Http.hpp)
add_library(Http ${HTTP_LIB} ${LOGGING_LIB})


set(SSL_TEST test/test_ssl.cpp include/Api.hpp)
add_executable(test_ssl ${SSL_TEST})
set(MANAGER_TEST test/test_manager.cpp include/Api.hpp)
add_executable(test_manager ${MANAGER_TEST})
set(HTTP2_NEGOTIATION_TEST test/test_http2_negotiation.cpp include/Api.hpp)
add_executable(test_http2_negotiation ${HTTP2_NEGOTIATION_TEST})


target_link_libraries(test_ssl Socket Http)
target_link_libraries(test_manager Socket Http)
target_link_libraries(test_http2_negotiation Socket Http)