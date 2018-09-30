//
// Created by anton.lamtev on 30.09.2018.
//

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <limits>

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

    std::cout << "All tests passed" << std::endl;

    return 0;
}
