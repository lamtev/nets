cmake_minimum_required(VERSION 3.12)
project(Nets)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(tcp/client)
add_subdirectory(tcp/n_clients_n_threads_server)
add_subdirectory(nets_lib)
