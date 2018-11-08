//
// Created by anton.lamtev on 29.09.2018.
//

#include <iostream>

#include "Server.h"


void printHelp();

int main(int argc, char **argv) {
    if (argc != 2) {
        printHelp();
        return -1;
    }
    uint16_t port;
    try {
       port = static_cast<uint16_t>(std::stoul(std::string(argv[1])));
    } catch (const std::exception &e) {
        printHelp();
        return -1;
    }

    auto server = Server(port);
    server.start();

    return 0;
}

constexpr static const char *help = R"(
Arguments:

<port> - port
)";

void printHelp() {
    std::cout << help << std::endl;
}
