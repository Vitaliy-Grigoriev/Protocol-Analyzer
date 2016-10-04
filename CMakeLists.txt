cmake_minimum_required(VERSION 3.5.2)
project(http2_analyzer)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_INCLUDE_PATH /usr/include/)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS_DEBUG "-pthread -lssl -lcrypto -g -std=c++14 -O0 -fsanitize=address -fsanitize=leak -fsanitize=undefined -Weverything -Wno-c++98-compat -Wno-exit-time-destructors -Wno-global-constructors -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-pthread -lssl -lcrypto -std=c++14 -O3 -DRELEASE")

# -fsanitize=thread -fsanitize=undefined
# -fsanitize=address -fsanitize=leak -fsanitize=undefined
# -fsanitize=memory -fno-omit-frame-pointer -fsanitize=undefined

set(SOURCE_EXE src/main.cpp)
add_executable(http2_analyzer ${SOURCE_EXE})
set(SOURCE_SOCKET_LIB src/Socket.cpp include/Socket.hpp src/SocketManager.cpp include/SocketManager.hpp src/Log.cpp include/Log.hpp src/SocketSSL.cpp)
add_library(Socket ${SOURCE_SOCKET_LIB})

target_link_libraries(http2_analyzer Socket)
