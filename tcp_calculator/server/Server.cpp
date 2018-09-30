//
// Created by anton.lamtev on 30.09.2018.
//

#include "Server.h"

Server::Server(uint16_t port) : net(new ServerNet(port)), io(new ServerIO()) {
    net->setDelegate(io);
}

Server::~Server() {
    delete net;
    delete io;
}

void Server::start() {
    net->start();
    io->start();
}

void Server::stop() {
    io->stop();
    net->stop();
}
