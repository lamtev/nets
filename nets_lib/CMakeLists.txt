project(NETS_LIB)

set(NETS_LIB_SRC
        receivenbytes.h receivenbytes.c
        receiveuptodelimiter.h receiveuptodelimiter.c)

add_library(nets_lib STATIC ${NETS_LIB_SRC})
