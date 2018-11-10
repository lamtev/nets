//
// Created by anton.lamtev on 10/11/2018.
//

#ifndef NETS_UDPSERVERNET_H
#define NETS_UDPSERVERNET_H

#include "ClientHandler.h"

#include <unordered_map>
#include <mutex>
#include <cstdint>

#include <nets_lib/SockAddr.h>

#include <calculator/server/commons/ServerIODelegate.h>
#include <calculator/server/commons/Client.h>

class ServerNetDelegate;


class UDPServerNet : public ServerIODelegate {
public:
    explicit UDPServerNet(uint16_t port);

    void setDelegate(ServerNetDelegate *delegate);

    void start();

    void stop();

    void ioWantsToKillClientWithId(ServerIO *io, uint64_t id) override;

    std::vector<Client> ioWantsToListClients(ServerIO *io) override;

    void ioWantsToExit(ServerIO *io) override;

private:
    uint16_t port;
    ServerNetDelegate *delegate;
    std::unordered_map<SockAddr, ClientHandler *> clientHandlers;
    std::mutex clientHandlersMutex;

};

#endif //NETS_UDPSERVERNET_H
