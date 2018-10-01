//
// Created by anton.lamtev on 30.09.2018.
//

#ifndef NETS_SERVER_H
#define NETS_SERVER_H

#include <cstdint>

#include "ServerIO.h"
#include "ServerNet.h"

class Server {
 private:
    ServerNet *net;
    ServerIO *io;

 public:
    Server(uint16_t port);
    ~Server();
    void start();
    void stop();
};

#endif //NETS_SERVER_H
