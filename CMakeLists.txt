cmake_minimum_required(VERSION 3.12)
project(Nets C)

set(CMAKE_C_STANDARD 11)

add_subdirectory(tcp/client)
add_subdirectory(tcp/server)
add_subdirectory(nets_lib)
