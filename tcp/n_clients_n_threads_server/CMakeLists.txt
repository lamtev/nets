project(TCP_SERVER)

set(CMAKE_CXX_STANDARD 14)
if(UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

add_executable(tcp_server main.cpp include.h)
target_link_libraries(tcp_server nets_lib)
target_include_directories(tcp_server PUBLIC ${Nets_SOURCE_DIR})
