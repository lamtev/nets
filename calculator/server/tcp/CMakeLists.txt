project(TCP_CALCULATOR_SERVER)

set(CMAKE_CXX_STANDARD 17)

add_executable(tcp_calculator_server
        TCPServerLauncher.cpp
        TCPServer.h TCPServer.cpp
        TCPServerNet.h TCPServerNet.cpp
        )

target_include_directories(tcp_calculator_server PUBLIC ${Nets_SOURCE_DIR})

target_link_libraries(tcp_calculator_server nets_lib)

target_link_libraries(tcp_calculator_server calculator_server_commons)

target_link_libraries(tcp_calculator_server calculator_protocol)
