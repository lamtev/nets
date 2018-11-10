//
// Created by anton.lamtev on 10/11/2018.
//

#ifndef NETS_UDPSERVER_H
#define NETS_UDPSERVER_H

#include <cinttypes>

class ServerIO;
class UDPServerNet;


class UDPServer {
    UDPServerNet *net;
    ServerIO *io;

public:
    explicit UDPServer(uint16_t port);

    ~UDPServer();

    void start();

    void stop();
};


#endif //NETS_UDPSERVER_H
