//
// Created by anton.lamtev on 30.09.2018.
//

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <limits>

#include <CalculatorProtocol.h>

//TODO: Google test
int main(int argc, char **argv) {
    std::vector<OperationType> twoOperandsOps = {
            OperationType::ADDITION,
            OperationType::SUBTRACTION,
            OperationType::MULTIPLICATION,
            OperationType::DIVISION
    };

    for (auto op : twoOperandsOps) {
        for (long num = -1000000; num <= 1000000; ++num) {
            Operation operation = Operation(op, num, std::numeric_limits<long>::max() - num);
            auto bytes = CalculatorProtocol::encode(operation);
            auto decodedOperation = CalculatorProtocol::decode(bytes);
            if (!decodedOperation->equals(operation)) {
                std::cout << "Test failure" << std::endl;
                std::cout << decodedOperation->toString() << " and " << operation.toString() << " are not equal"
                          << std::endl;
                delete bytes;
                delete decodedOperation;
                return -1;
            }
            delete bytes;
            delete decodedOperation;
        }
    }

    std::vector<OperationType> singleOperandOps = {
            OperationType::SQUARE_ROOT,
            OperationType::FACTORIAL,
    };

    for (auto op : singleOperandOps) {
        for (long num = -1000000; num <= 1000000; ++num) {
            Operation operation = Operation(op, num);
            auto bytes = CalculatorProtocol::encode(operation);
            auto decodedOperation = CalculatorProtocol::decode(bytes);
            if (!decodedOperation->equals(operation)) {
                std::cout << "Test failure" << std::endl;
                std::cout << decodedOperation->toString() << " and " << operation.toString() << " are not equal"
                          << std::endl;
                delete bytes;
                delete decodedOperation;
                return -1;
            }
            delete bytes;
            delete decodedOperation;
        }
    }

    std::cout << "All tests passed" << std::endl;

    return 0;
}
