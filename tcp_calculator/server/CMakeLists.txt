project(TCP_CALCULATOR_SERVER)

set(CMAKE_CXX_STANDARD 17)

add_executable(tcp_calculator_server main.cpp Server.cpp Server.h ServerIO.cpp ServerIO.h ServerNet.cpp ServerNet.h ClientSession.h ServerNetDelegate.h ServerIODelegate.h ServerNetError.h MathUtils.h)

target_link_libraries(tcp_calculator_server nets_lib)
target_include_directories(tcp_calculator_server PUBLIC ${Nets_SOURCE_DIR})

target_link_libraries(tcp_calculator_server tcp_calculator_protocol)
target_include_directories(tcp_calculator_server PUBLIC ${TCP_CALCULATOR_SOURCE_DIR})
