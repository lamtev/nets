project(TCP_CALCULATOR_CLIENT)

set(CMAKE_CXX_STANDARD 17)

add_executable(tcp_calculator_client TCPClient.cpp TCPClient.h)

target_include_directories(tcp_calculator_client PUBLIC ${Nets_SOURCE_DIR})

target_link_libraries(tcp_calculator_client nets_lib)

target_link_libraries(tcp_calculator_client calculator_protocol)
