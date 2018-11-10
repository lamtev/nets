//
// Created by anton.lamtev on 30.09.2018.
//

#include "TCPServer.h"
#include "TCPServerNet.h"

#include <calculator/server/commons/ServerIO.h>


TCPServer::TCPServer(uint16_t port) : net(new TCPServerNet(port)), io(new ServerIO()) {
    net->setDelegate(io);
    io->setDelegate(net);
}

TCPServer::~TCPServer() {
    delete net;
    delete io;
}

void TCPServer::start() {
    std::thread ioThread([this]() {
        io->start();
    });
    net->start();
    ioThread.join();
}

void TCPServer::stop() {
    io->stop();
    net->stop();
}
