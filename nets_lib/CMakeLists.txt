project(NETS_LIB)

set(CMAKE_C_STANDARD 90)

set(NETS_LIB_SRC
        receivenbytes.h receivenbytes.c
        receiveuptodelimiter.h receiveuptodelimiter.c send.cpp send.h)

add_library(nets_lib STATIC ${NETS_LIB_SRC})
