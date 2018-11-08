//
// Created by anton.lamtev on 30.09.2018.
//

#ifndef NETS_SERVERIO_H
#define NETS_SERVERIO_H


#include <vector>

#include "ServerNetDelegate.h"

class ServerIODelegate;
class ClientSession;


class ServerIO : public ServerNetDelegate {
 private:
    ServerIODelegate *delegate;
    bool isRunning;
    std::vector<ClientSession> clients;

 public:
    ServerIO();
    void start();
    void stop();
    void setDelegate(ServerIODelegate *delegate);
    void netDidFailWithError(ServerNet *net, ServerNetError error) override;
};


#endif //NETS_SERVERIO_H
