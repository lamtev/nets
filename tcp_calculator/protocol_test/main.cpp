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

    int64_t a = 0x7FFFFFFFFFFFFFFE;
    auto b = new int64_t[1];
    *b = a;

    auto c = reinterpret_cast<int8_t *>(&a);
    std::cout << (uint32_t) c[0] << "|" << (uint32_t) c[1] << "|" << (uint32_t) c[2] << "|" << (uint32_t) c[3] << std::endl;
    auto d = reinterpret_cast<int8_t *>(b);
    std::cout << (uint32_t) d[0] << "|" << (uint32_t) d[1] << "|" << (uint32_t) d[2] << "|" << (uint32_t) d[3] << std::endl;

    auto e = reinterpret_cast<int64_t *>(c);
    auto f = reinterpret_cast<int64_t *>(d);

    std::cout << *e << std::endl;
    std::cout << *f << std::endl;

    delete[] b;

    return 0;
}
