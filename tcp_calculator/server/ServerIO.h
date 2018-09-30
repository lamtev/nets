//
// Created by anton.lamtev on 30.09.2018.
//

#ifndef NETS_SERVERIO_H
#define NETS_SERVERIO_H

#include "ServerNet.h"

class ServerIO : public ServerNetDelegate {
 private:
    bool isRunning;
 public:
    ServerIO();
    void start();
    void stop();
    void netDidFailWithError(ServerNet *net, ServerNetError error) override;
};


#endif //NETS_SERVERIO_H
