project(CALCULATOR_SERVER_COMMONS)

set(CMAKE_CXX_STANDARD 11)

set(CALCULATOR_SERVER_COMMONS_SRC
        ServerNetDelegate.h
        ServerIODelegate.h
        ServerIO.h ServerIO.cpp
        ServerNetError.h
        Client.h ServerUtils.cpp ServerUtils.h)

add_library(calculator_server_commons STATIC ${CALCULATOR_SERVER_COMMONS_SRC})
