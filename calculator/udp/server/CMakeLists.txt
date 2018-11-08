project(UDP_CALCULATOR_SERVER)

set(CMAKE_CXX_STANDARD 17)

add_executable(udp_calculator_server main.cpp Balancer.h Balancer.cpp)

target_link_libraries(udp_calculator_server nets_lib)
target_include_directories(udp_calculator_server PUBLIC ${Nets_SOURCE_DIR})

target_link_libraries(udp_calculator_server calculator_protocol)
target_include_directories(udp_calculator_server PUBLIC ${UDP_CALCULATOR_SOURCE_DIR})
