//
// Created by anton.lamtev on 29.09.2018.
//

#include "Server.h"

int main(int argc, char **argv) {
    auto server = Server(1234);
    server.start();
    return 0;
}
