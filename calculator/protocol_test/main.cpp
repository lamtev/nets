//
// Created by anton.lamtev on 30.09.2018.
//

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <limits>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include <chrono>

#include <Message.h>
#include <Operation.h>

//TODO: Google test
int main(int argc, char **argv) {
    std::vector<OperationType> twoOperandsOps = {
            OperationType::ADDITION,
            OperationType::SUBTRACTION,
            OperationType::MULTIPLICATION,
            OperationType::DIVISION
    };

    std::vector<OperationType> singleOperandOps = {
            OperationType::SQUARE_ROOT,
            OperationType::FACTORIAL,
    };

    for (long i = -std::numeric_limits<long>::min() / 2; i < std::numeric_limits<long>::max() / 2; i+=97) {
        for (const auto &opt : twoOperandsOps) {
            auto op = Operation(opt, i, std::numeric_limits<long>::max() / 2 - abs(i));
            auto message = new Message(MessageType::MATH_REQUEST, op.nBytes(), op.toBytes());
            auto bytes = message->toBytes();

            auto message2 = Message::of(bytes);
            auto op2 = Operation::of(message2->data());

            delete[] message->data();
            delete message;
            delete[] bytes;
            delete message2;

            if (!op2->equals(op)) {
                std::cout << "Operations are not equal" << std::endl;
            }

            delete op2;
        }

        for (const auto &opt : singleOperandOps) {

        }
    }

    return 0;
}
