project(CALCULATOR_PROTOCOL)

set(CMAKE_C_STANDARD 90)

set(CALCULATOR_PROTOCOL_SRC Operation.h Message.h Operation.cpp BitsUtils.h Message.cpp MathResponse.h MathResponse.cpp NumberedMessage.h NumberedMessage.cpp)

add_library(calculator_protocol STATIC ${CALCULATOR_PROTOCOL_SRC})
