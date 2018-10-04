project(TCP_CALCULATOR_PROTOCOL)

set(CMAKE_C_STANDARD 90)

set(TCP_CALCULATOR_PROTOCOL_SRC Operation.h Message.h Operation.cpp BitsUtils.h Message.cpp)

add_library(tcp_calculator_protocol STATIC ${TCP_CALCULATOR_PROTOCOL_SRC})
