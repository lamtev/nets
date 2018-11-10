//
// Created by anton.lamtev on 10/11/2018.
//

#include "ServerUtils.h"

#include <iostream>


constexpr static const char *help = R"(
Arguments:

<port> - port
)";

void printHelp() {
    std::cout << help << std::endl;
}
