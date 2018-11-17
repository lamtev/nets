project(CALCULATOR_CLIENT)

if(UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

add_subdirectory(tcp)
add_subdirectory(udp)
