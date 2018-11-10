project(CALCULATOR_PROTOCOL)

set(CMAKE_CXX_STANDARD 11)

set(CALCULATOR_PROTOCOL_SRC
        Operation.h Operation.cpp
        Message.h  Message.cpp
        BitsUtils.h
        MathResponse.h MathResponse.cpp
        NumberedMessage.h NumberedMessage.cpp)

add_library(calculator_protocol STATIC ${CALCULATOR_PROTOCOL_SRC})
