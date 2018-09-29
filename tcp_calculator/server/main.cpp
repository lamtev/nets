//
// Created by anton.lamtev on 29.09.2018.
//

#include <iostream>

#include <CalculatorProtocol.h>

int main(int argc, char **argv) {
    auto *operation1 = new Operation(OperationType::MULTIPLICATION, 0x8000000000000000, 0xFFFFFFFFFFFFFFFF);
    auto *operation2 = new Operation(OperationType::SQUARE_ROOT, 123);
    auto bytes = CalculatorProtocol::encode(*operation1);
    auto decoded = CalculatorProtocol::decode(bytes);

    std::cout << operation1->toString() << std::endl;
    std::cout << decoded->toString() << std::endl;
    if (operation1->equals(*decoded)) {
        std::cout << "equals!!!" << std::endl;
    }

    std::cout << operation2->toString() << std::endl;

    delete operation1;
    delete operation2;
    delete decoded;
    delete bytes;

    return 0;
}
