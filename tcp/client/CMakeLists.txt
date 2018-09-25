set(CMAKE_CXX_STANDARD 14)

add_executable(tcp_client main.c)
target_link_libraries(tcp_client nets_lib)
target_include_directories(tcp_client PUBLIC ${Nets_SOURCE_DIR})
