project(TCP_SERVER)

add_executable(tcp_server main.c)
target_link_libraries(tcp_server nets_lib)
target_include_directories(tcp_server PUBLIC ${Nets_SOURCE_DIR})
