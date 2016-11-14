cmake_minimum_required(VERSION 3.5.2)
project(http2_analyzer)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_INCLUDE_PATH /usr/include/)

set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_C_COMPILER "/usr/bin/clang")
# set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS_DEBUG "-pthread -lssl -lcrypto -g -std=c++14 -O0 -fsanitize=address -fsanitize=leak -fsanitize=undefined -Weverything -Wno-c++98-compat -Wno-exit-time-destructors -Wno-global-constructors -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-pthread -lssl -lcrypto -std=c++14 -O3 -DRELEASE")

# -fsanitize=thread -fsanitize=undefined
# -fsanitize=address -fsanitize=leak -fsanitize=undefined
# -fsanitize=memory -fno-omit-frame-pointer -fsanitize=undefined

set(SSL_TEST test/test_ssl.cpp)
add_executable(test_ssl ${SSL_TEST})
set(MANAGER_TEST test/test_manager.cpp)
add_executable(test_manager ${MANAGER_TEST})
set(SOCKET_LIB src/Socket.cpp include/Socket.hpp src/SocketManager.cpp include/SocketManager.hpp src/Log.cpp include/Log.hpp src/SocketSSL.cpp)
add_library(Socket ${SOCKET_LIB})

target_link_libraries(test_ssl Socket)
target_link_libraries(test_manager Socket)
