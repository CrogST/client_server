cmake_minimum_required(VERSION 3.10)

project(client_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(client client_main.cpp client.cpp)
add_executable(server server_main.cpp server.cpp)
