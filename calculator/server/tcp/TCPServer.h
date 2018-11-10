//
// Created by anton.lamtev on 30.09.2018.
//

#ifndef NETS_SERVER_H
#define NETS_SERVER_H

#include <cstdint>

class TCPServerNet;
class ServerIO;


class TCPServer {
private:
    TCPServerNet *net;
    ServerIO *io;

public:
    explicit TCPServer(uint16_t port);

    ~TCPServer();

    void start();

    void stop();
};

#endif //NETS_SERVER_H
