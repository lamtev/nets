project(TCP_CALCULATOR_CLIENT)

set(CMAKE_CXX_STANDARD 17)

add_executable(tcp_calculator_client main.cpp main.h)

target_link_libraries(tcp_calculator_client nets_lib)
target_include_directories(tcp_calculator_client PUBLIC ${Nets_SOURCE_DIR})

target_link_libraries(tcp_calculator_client calculator_protocol)
target_include_directories(tcp_calculator_client PUBLIC ${TCP_CALCULATOR_SOURCE_DIR})
