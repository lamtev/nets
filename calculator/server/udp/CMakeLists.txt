project(UDP_CALCULATOR_SERVER)

set(CMAKE_CXX_STANDARD 17)

add_executable(udp_calculator_server main.cpp
        ClientHandler.cpp ClientHandler.h
        UDPServer.cpp UDPServer.h
        UDPServerNet.cpp UDPServerNet.h)

target_include_directories(udp_calculator_server PUBLIC ${Nets_SOURCE_DIR})

target_link_libraries(udp_calculator_server nets_lib)

target_link_libraries(udp_calculator_server calculator_protocol)

target_link_libraries(udp_calculator_server calculator_server_commons)
