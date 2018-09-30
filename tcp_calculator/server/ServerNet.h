//
// Created by anton.lamtev on 30.09.2018.
//

#ifndef NETS_SERVERNET_H
#define NETS_SERVERNET_H

#include <cstdint>
class ServerNetDelegate;

class ServerNet {
 private:
    uint16_t port;
    ServerNetDelegate *delegate;
 public:
    ServerNet(uint16_t port);
    void setDelegate(ServerNetDelegate *delegate);
    void start();
    void stop();
};

enum class ServerNetError {
    SOCKET_CREATE_ERROR,
    SOCKET_BIND_ERROR,
    SOCKET_LISTEN_ERROR,
    SOCKET_ACCEPT_ERROR,
    RECEIVE_ERROR,
    SEND_ERROR,
};

class ServerNetDelegate {
 public:
    virtual void netDidFailWithError(ServerNet *net, ServerNetError error) = 0;
};

#endif //NETS_SERVERNET_H
