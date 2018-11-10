//
// Created by anton.lamtev on 10/11/2018.
//

#include "UDPServer.h"
#include "UDPServerNet.h"

#include <thread>

#include <calculator/server/commons/ServerIO.h>


UDPServer::UDPServer(uint16_t port): net(new UDPServerNet(port)), io(new ServerIO()) {
    net->setDelegate(io);
    io->setDelegate(net);
}

UDPServer::~UDPServer() {
    delete net;
    delete io;
}

void UDPServer::start() {
    std::thread ioThread([this]() {
        io->start();
    });
    net->start();
    ioThread.join();
}

void UDPServer::stop() {
    net->stop();
    io->stop();
}
