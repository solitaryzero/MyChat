cmake_minimum_required(VERSION 2.6)

# avoid some cmake warnings
IF(POLICY CMP0026)
    CMAKE_POLICY(SET CMP0026 OLD)
ENDIF()

project(MyChat)

add_compile_options(--std=c++14 -fPIC -g)

#pthread
find_package(Threads)

#json11
add_subdirectory(json11)
include_directories(json11)

set(SERVER_SRC_LIST server/server.cpp server/serverTest.cpp server/serverDatabase.cpp socket/TcpChatSocket.cpp)
set(CLIENT_SRC_LIST client/client.cpp client/clientTest.cpp socket/TcpChatSocket.cpp)
add_executable(Server ${SERVER_SRC_LIST})
target_link_libraries(Server json11 ${CMAKE_THREAD_LIBS_INIT})
add_executable(Client ${CLIENT_SRC_LIST})
target_link_libraries(Client json11 ${CMAKE_THREAD_LIBS_INIT})