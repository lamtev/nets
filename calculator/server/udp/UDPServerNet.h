//
// Created by anton.lamtev on 10/11/2018.
//

#ifndef NETS_UDPSERVERNET_H
#define NETS_UDPSERVERNET_H

#include "ClientHandler.h"

#include <unordered_map>
#include <shared_mutex>
#include <cstdint>

#include <nets_lib/SockAddr.h>

#include <calculator/server/commons/ServerIODelegate.h>
#include <calculator/server/commons/ServerNet.h>
#include <calculator/server/commons/Client.h>

class ServerNetDelegate;


class UDPServerNet : public ServerNet, public ServerIODelegate {
public:
    explicit UDPServerNet(uint16_t port);

    void setDelegate(ServerNetDelegate *delegate) override;

    void start() override;

    void stop() override;

    void ioWantsToKillClientWithId(ServerIO *io, uint64_t id) override;

    std::vector<Client> ioWantsToListClients(ServerIO *io) override;

    void ioWantsToExit(ServerIO *io) override;

private:
    uint16_t port;
    int socket;
    ServerNetDelegate *delegate;
    std::unordered_map<SockAddr, ClientHandler *> clientHandlers;
    std::shared_mutex clientHandlersMutex;
    std::atomic<uint64_t> clientCounter;
    std::vector<std::thread> joinThreads;
};

#endif //NETS_UDPSERVERNET_H
