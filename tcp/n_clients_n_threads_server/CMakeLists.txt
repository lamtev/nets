project(TCP_SERVER)

set(CMAKE_CXX_STANDARD 14)

add_executable(tcp_server main.cpp TCPClient.h)
target_link_libraries(tcp_server nets_lib)
target_include_directories(tcp_server PUBLIC ${Nets_SOURCE_DIR})
