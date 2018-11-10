//
// Created by anton.lamtev on 30/10/2018.
//

#include "UDPServer.h"

#include <iostream>

#include <calculator/server/commons/ServerUtils.h>


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
    UDPServer server(port);
    server.start();

    return 0;
}
