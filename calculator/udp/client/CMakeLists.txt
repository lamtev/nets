project(UDP_CALCULATOR_CLIENT)

set(CMAKE_CXX_STANDARD 17)

add_executable(udp_calculator_client UDPClient.cpp UDPClient.h)

target_link_libraries(udp_calculator_client nets_lib)
target_include_directories(udp_calculator_client PUBLIC ${Nets_SOURCE_DIR})

target_link_libraries(udp_calculator_client calculator_protocol)
target_include_directories(udp_calculator_client PUBLIC ${UDP_CALCULATOR_SOURCE_DIR})
