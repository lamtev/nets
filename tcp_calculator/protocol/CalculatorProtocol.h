//
// Created by anton.lamtev on 29.09.2018.
//

#ifndef NETS_CALCULATORPROTOCOL_H
#define NETS_CALCULATORPROTOCOL_H

#include <cstdint>

class Operation;
class Message;

class CalculatorProtocol {
 public:
    static u_int8_t *encode(const Message &message);
    static Message *decode(u_int8_t *bytes);
};

#endif //NETS_CALCULATORPROTOCOL_H
