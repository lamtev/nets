//
// Created by anton.lamtev on 30.09.2018.
//

#ifndef NETS_TCPSERVERNET_H
#define NETS_TCPSERVERNET_H

#include "ClientSession.h"

#include <cstdint>
#include <shared_mutex>
#include <vector>
#include <thread>

#include <calculator/server/commons/ServerIODelegate.h>
#include <calculator/server/commons/ServerNet.h>
#include <calculator/server/commons/Client.h>

class ServerNetDelegate;
class Message;
class Operation;


class TCPServerNet : public ServerNet, public ServerIODelegate {
private:
    const uint16_t port;
    int listeningSocket;
    ServerNetDelegate *delegate;
    std::vector<ClientSession> clients;
    std::shared_mutex clientsMutex;
    std::atomic<uint64_t> idCounter;

    std::mutex clientThreadsMutex;
    std::vector<std::thread> clientThreads;

public:
    explicit TCPServerNet(uint16_t port);

    void setDelegate(ServerNetDelegate *delegate) override;

    void start() override;

    void stop() override;

    void ioWantsToKillClientWithId(ServerIO *io, uint64_t id) override;

    std::vector<Client> ioWantsToListClients(ServerIO *io) override;

    void ioWantsToExit(ServerIO *io) override;

private:
    uint64_t nextId() noexcept;

    static void closeSocket(int socket);

    Message *handleRequest(Message *request, int socket);

    void submitHardOperation(const Operation &operation, int socket);
};

#endif //NETS_TCPSERVERNET_H
