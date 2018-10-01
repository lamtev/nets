//
// Created by anton.lamtev on 30.09.2018.
//

#ifndef NETS_SERVERNET_H
#define NETS_SERVERNET_H

#include <cstdint>
#include <shared_mutex>
#include <vector>

#include "ClientSession.h"
#include "ServerIODelegate.h"

class ServerNetDelegate;

class ServerNet : public ServerIODelegate {
 private:
    const uint16_t port;
    int listeningSocket{};
    ServerNetDelegate *delegate;
    std::vector<ClientSession> clients;
    std::shared_mutex clientsMutex;
    std::atomic<uint64_t> idCounter;

 public:
    explicit ServerNet(uint16_t port);
    void setDelegate(ServerNetDelegate *delegate);
    void start();
    void stop();
    void ioWantsToKillClientWithId(ServerIO *io, uint64_t id) override;
    std::vector<ClientSession> ioWantsToListClients(ServerIO *io) override;
    void ioWantsToExit(ServerIO *io) override;

 private:
    uint64_t nextId() noexcept;
    static void closeSocket(int socket);
};


#endif //NETS_SERVERNET_H
